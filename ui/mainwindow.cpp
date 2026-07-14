#include "mainwindow.h"
#include "../ui_mainwindow.h"
#include "password_delegate.h"
#include "../settings/settings.h"
#include "../kas/aes256.h"
#include "../kas/base64.h"
#include "../kas/SafeData.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>
#include <QLineEdit>
#include <QSet>
#include <QStatusBar>

extern const settings::Settings& SETTINGS();

namespace GlobalVariables
{
kas::utils::SafeData<QSet<int>> ACTUAL_UNIQUE_ID_LIST{};
}

namespace detail
{
//-----------------

void addIdToList(int id)
{
    GlobalVariables::ACTUAL_UNIQUE_ID_LIST.modify([id](QSet<int>& ids){
        ids.insert(id);
    });
}

//-----------------

void deleteIdFromList(int id)
{
    GlobalVariables::ACTUAL_UNIQUE_ID_LIST.modify([id](QSet<int>& ids){
        if(!ids.remove(id))
            ids.insert(id);
    });
}

//-----------------

void clearIdList()
{
    GlobalVariables::ACTUAL_UNIQUE_ID_LIST.modify([](QSet<int>& ids){
        ids.clear();
    });
}

//-----------------

qsizetype getSizeIdList()
{
    qsizetype size{};
    GlobalVariables::ACTUAL_UNIQUE_ID_LIST.modify([&size](QSet<int>& ids){
        size = ids.size();
    });
    return size;
}

//-----------------

bool isEmptyIdList()
{
    bool is_empty{};
    GlobalVariables::ACTUAL_UNIQUE_ID_LIST.modify([&is_empty](QSet<int>& ids){
        is_empty = ids.isEmpty();
    });
    return is_empty;
}

//-----------------

QString getDBAbsolutePath()
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
    + "/"
        + SETTINGS().get<settings::tags::db_name_t>();
}

//-----------------

bool initDataBase(QSqlDatabase& db)
{
    const QString db_path { getDBAbsolutePath() };

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(db_path);
    if(! db.open())
    {
        qWarning() <<  "Cannot open database:" << db.lastError().text();
        return false;
    }

    QSqlQuery query { db };
    const bool success {
        query.exec(
            "CREATE TABLE IF NOT EXISTS passwords ("
            "id INTEGER PRIMARY KEY, "
            "description TEXT NOT NULL, "
            "password TEXT NOT NULL"
            ")"
            )
    };

    if (!success)
    {
        qWarning() << "Cannot create table:" << query.lastError().text();
        return false;
    }

    qDebug() << "Database initialized:" << db_path;
    return true;
}

//-----------------

bool
loadFromDatabase(QSqlDatabase& db, QTableWidget* table_widget)
{
    if(! db.isOpen())
    {
        qWarning() << "Database is not open!";
        return false;
    }

    QSqlQuery query { db };
    if( ! query.exec("SELECT id, description, password FROM passwords ORDER BY id"))
    {
        qWarning() << "Select failed:" << query.lastError().text();
        return false;
    }

    while (table_widget->rowCount() > 0)
        table_widget->removeRow(0);

    const kas::crypto::Aes256 aes {
        SETTINGS().get<settings::tags::private_key_t>(),
        kas::crypto::base64::encode(
            SETTINGS().get<settings::tags::salt_t>() +
            SETTINGS().get<settings::tags::application_author_t>() +
            SETTINGS().get<settings::tags::application_name_t>()
            )
    };
    int row {};
    while (query.next())
    {
        const int id { query.value(0).toInt() };
        auto description { query.value(1).toString() };
        auto password { aes.decrypt( query.value(2).toString()) };

        table_widget->insertRow(row);

        auto* id_item { new QTableWidgetItem(QString::number(id)) };
        id_item->setFlags(id_item->flags() & ~Qt::ItemIsEditable);

        auto* desc_item { new QTableWidgetItem(description) };
        desc_item->setFlags(desc_item->flags() | Qt::ItemIsEditable);

        auto* pass_item { new QTableWidgetItem(password) };
        pass_item->setFlags(pass_item->flags() | Qt::ItemIsEditable);

        table_widget->setItem(row, 0, id_item);
        table_widget->setItem(row, 1, desc_item);
        table_widget->setItem(row, 2, pass_item);
        ++row;
    }

    clearIdList();

    qDebug() << "Loaded" << row << "records from database";

    return true;
}

//-----------------

bool saveToDataBase(QSqlDatabase& db, QTableWidget* table_widget )
{
    if(! db.isOpen())
    {
        qWarning() << "Database is not open!";
        return false;
    }

    QSqlQuery query { db };

    db.transaction();

    if(! query.exec("DELETE FROM passwords"))
    {
        qWarning() << "[delete] " << query.lastError().text();
        return false;
    }

    if(! query.prepare("INSERT INTO passwords (id, description, password) VALUES (?, ?, ?)"))
    {
        qWarning() << "[insert] "<< query.lastError().text();
        return false;
    }

    int data_base_counter {};
    const kas::crypto::Aes256 aes {
        SETTINGS().get<settings::tags::private_key_t>(),
        kas::crypto::base64::encode(
            SETTINGS().get<settings::tags::salt_t>() +
            SETTINGS().get<settings::tags::application_author_t>() +
            SETTINGS().get<settings::tags::application_name_t>()
            )
    };

    for (int row{}; row < table_widget->rowCount(); ++row)
    {
        QTableWidgetItem* id_item { table_widget->item(row, 0) };
        QTableWidgetItem* desc_item { table_widget->item(row, 1) };
        QTableWidgetItem* pass_item { table_widget->item(row, 2) };

        if (!desc_item || desc_item->text().isEmpty())
            continue;
        if (!pass_item || pass_item->text().isEmpty())
            continue;

        query.addBindValue(id_item ? id_item->text().toInt() : row + 1);
        query.addBindValue(desc_item->text());
        query.addBindValue( aes.encrypt(pass_item->text()));

        if (!query.exec())
        {
            qWarning() << "Insert failed:" << query.lastError().text();
            db.rollback();
            return false;
        }
        ++data_base_counter;
    }

    if (!db.commit())
    {
        qWarning() << "Commit failed:" << db.lastError().text();
        db.rollback();
        return false;
    }

    clearIdList();

    qDebug() << "Saved" << data_base_counter << "records to database";
    return true;
}

//-----------------

bool clearDataBase(QSqlDatabase& db, QTableWidget* table_widget)
{
    if (!db.isOpen())
    {
        qWarning() << "Database is not open!";
        return false;
    }

    QSqlQuery query(db);

    if (!query.exec("DELETE FROM passwords"))
    {
        qWarning() << "Clear failed:" << query.lastError().text();
        return false;
    }

    query.exec("DELETE FROM sqlite_sequence WHERE name='passwords'");
    query.exec("VACUUM");

    while (table_widget->rowCount() > 0)
        table_widget->removeRow(0);

    clearIdList();

    qDebug() << "Database cleared. Rows deleted:" << query.numRowsAffected();
    return true;
}

//-----------------

void updateStatusBar(QStatusBar * status_bar)
{
    if(!status_bar)
        return;
    const auto size_list { getSizeIdList() };
    if(size_list > 0)
    {
        status_bar->setVisible(true);
        status_bar->showMessage(
            QString{ "Внимание! У вас есть не сохраненные записи в количестве - %1 шт."}
                .arg(size_list)
            );
    }
    else
        status_bar->showMessage("Изменения отсутствуют.", 0);
}

//-----------------

}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_db{},
    m_status_bar_timer{}

{
    ui->setupUi(this);

    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setHorizontalHeaderLabels({"id", "Описание", "Пароль"});
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableWidget->setColumnWidth(1, 400);
    ui->tableWidget->setColumnWidth(2, 200);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked);
    ui->tableWidget->verticalHeader()->setVisible(false);
    setMinimumWidth(700);

    ui->tableWidget->setItemDelegateForColumn(2, new PasswordDelegate(this));

    if (! detail::initDataBase(m_db))
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось инициализировать базу данных!");
        close();
    }

    auto query_opt { detail::loadFromDatabase(m_db, ui->tableWidget) };
    if (! query_opt)
    {
        QMessageBox::critical(this, "Ошибка",
                              "Не удалось загрузить данные из базы данных!\n"
                              "Путь к базе данных: '" + detail::getDBAbsolutePath() + "'"
                              );
        close();
    }

    connect(ui->tableWidget, &QTableWidget::itemChanged, this, &MainWindow::onItemChanged);

    m_status_bar_timer = new QTimer { this };
    connect(m_status_bar_timer, &QTimer::timeout, this,  [this]{ detail::updateStatusBar(ui->statusbar); });
    m_status_bar_timer->start(500);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    const int row { ui->tableWidget->rowCount() };
    ui->tableWidget->insertRow(row);
    ui->tableWidget->selectRow(row);
    ui->tableWidget->scrollToBottom();

    const int new_id {
        [&]{
            int res_id { 1 };
            if(row <= 0)
                return res_id;
            QTableWidgetItem* last_id_item { ui->tableWidget->item(row - 1, 0) };
            if(! last_id_item )
                return res_id;
            bool success{};
            int last_id { last_id_item->text().toInt(&success) };
            return success ? last_id + 1 : res_id;
        }()
    };
    auto* id {  new QTableWidgetItem { QString::number(new_id) } };
    id->setFlags(id->flags() & ~Qt::ItemIsEditable);

    auto* description {  new QTableWidgetItem { "" } };
    description->setFlags(description->flags() | Qt::ItemIsEditable);

    auto* password {  new QTableWidgetItem { "" } };
    password->setFlags(password->flags() | Qt::ItemIsEditable);

    ui->tableWidget->setItem(row, 0, id);
    ui->tableWidget->setItem(row, 1, description);
    ui->tableWidget->setItem(row, 2, password);

    ui->tableWidget->setCurrentCell(row, 1);
    ui->tableWidget->editItem(description);

    detail::addIdToList(new_id);

}

void MainWindow::onItemChanged(QTableWidgetItem* item)
{
    if (item->column() == 1 && !item->text().isEmpty())
    {
        const int row { item->row() };

        ui->tableWidget->setCurrentCell(row, 2);
        ui->tableWidget->editItem(ui->tableWidget->item(row, 2));
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    const int row { ui->tableWidget->currentRow() };
    if(row < 0)
        return;

    const int select_id {
        [&]{
            int error_id { -1 };
            QTableWidgetItem* last_id_item { ui->tableWidget->item(row, 0) };
            if(! last_id_item )
                return error_id;
            bool success{};
            int current_id { last_id_item->text().toInt(&success) };
            return success ? current_id : error_id;
        }()
    };

    if(select_id < 0)
        return;

    const auto res { QMessageBox::question(
        this,
        "Удаление пароля",
        "Вы действительно хотите удалить пароль c id = '" + QString::number(select_id) + "'?",
        QMessageBox::Ok | QMessageBox::Cancel,
        QMessageBox::Cancel
        ) };

    if(res == QMessageBox::Ok)
    {
        ui->tableWidget->removeRow(row);
        detail::deleteIdFromList(select_id);
    }
}

void MainWindow::on_action_triggered()
{
    if (detail::saveToDataBase(m_db, ui->tableWidget))
        QMessageBox::information(this, "Сохранено", "Данные успешно сохранены в базу данных!");
    else
        QMessageBox::critical(this, "Ошибка", "Не удалось сохранить данные в базу данных!");
}

void MainWindow::on_action_2_triggered()
{
    if (detail::loadFromDatabase(m_db, ui->tableWidget))
        QMessageBox::information(this, "Загружено", "Данные успешно загружены!");
    else
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить данные!");
}

void MainWindow::on_action_4_triggered()
{
    const auto res { QMessageBox::question(
        this,
        "Очистка базы данных",
        "Вы действительно хотите очистить базу данных?",
        QMessageBox::Ok | QMessageBox::Cancel,
        QMessageBox::Cancel
        ) };

    if(res == QMessageBox::Ok)
    {
        if (detail::clearDataBase(m_db, ui->tableWidget))
            QMessageBox::information(this, "Очистка БД", "База данных успешно очищена!");
        else
            QMessageBox::critical(this, "Ошибка очистки БД", "Не удалось очистить базу данных!");
    }
}
