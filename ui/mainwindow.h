#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QSqlDatabase>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void onItemChanged(QTableWidgetItem* item);

    void on_action_triggered();

    void on_action_2_triggered();

    void on_action_4_triggered();

private:
    Ui::MainWindow *ui;
    QSqlDatabase m_db;
    QTimer* m_status_bar_timer;
};
#endif // MAINWINDOW_H
