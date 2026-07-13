#ifndef AUTHDIALOG_H
#define AUTHDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QString>

class AuthDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AuthDialog(QWidget *parent = nullptr);
private slots:
    void onAuthClicked();

private:
    QLineEdit *m_key_edit;
    int m_attempts;
};

#endif // AUTHDIALOG_H
