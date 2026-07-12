#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QString>

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
private slots:
    void onLoginClicked();

    QString getLogin() const;
    QString getPassword() const;

private:
    QLineEdit *m_login_edit;
    QLineEdit *m_password_edit;
    int m_attempts;
};

#endif // LOGINDIALOG_H
