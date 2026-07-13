#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)

{
    ui->setupUi(this);

    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setHorizontalHeaderLabels({"Описание", "Пароль"});
    ui->tableWidget->setColumnWidth(0, 400);
    ui->tableWidget->setColumnWidth(1, 200);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    // ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    // ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setEditTriggers(QAbstractItemView::AnyKeyPressed);
    ui->tableWidget->verticalHeader()->setVisible(true);
    setMinimumWidth(650);

    connect(ui->tableWidget, &QTableWidget::itemChanged, this, &MainWindow::onItemChanged);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    // disconnect(ui->tableWidget, &QTableWidget::itemChanged, this, &MainWindow::onItemChanged);

    const int row { ui->tableWidget->rowCount() };
    ui->tableWidget->insertRow(row);
    ui->tableWidget->selectRow(row);
    ui->tableWidget->scrollToBottom();

    auto* description {  new QTableWidgetItem { "" } };
    description->setFlags(description->flags() | Qt::ItemIsEditable);

    auto* password {  new QTableWidgetItem { "" } };
    password->setFlags(password->flags() | Qt::ItemIsEditable);

    ui->tableWidget->setItem(row, 0, description);
    ui->tableWidget->setItem(row, 1, password);

    ui->tableWidget->setCurrentCell(row, 0);
    ui->tableWidget->editItem(description);
}

void MainWindow::onItemChanged(QTableWidgetItem* item)
{
    if (item->column() == 0 && !item->text().isEmpty())
    {
        const int row { item->row() };

        ui->tableWidget->setCurrentCell(row, 1);
        ui->tableWidget->editItem(ui->tableWidget->item(row, 1));
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    const int row { ui->tableWidget->currentRow() };

    const auto result { QMessageBox::question(
        this,
        "Удаление пароля",
        "Вы действительно хотите удалить пароль?",
        QMessageBox::Ok | QMessageBox::Cancel,
        QMessageBox::Cancel
        ) };
    if(result == QMessageBox::Ok)
        ui->tableWidget->removeRow(row);
}

