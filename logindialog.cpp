#include "logindialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QMessageBox>

namespace GlobalVariables {

static constexpr int NUMBER_LOGIN_ATTACHMENT{ 5 };
}
namespace detail {

//-----------------

static bool
checkInputData(const QString& login, const QString& password)
{
    static int n{};
    n++;

    return n > 7;
}

//-----------------

}

//-----------------

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog{ parent },
    m_login_edit{ new QLineEdit },
    m_password_edit{ new QLineEdit },
    m_attempts { GlobalVariables::NUMBER_LOGIN_ATTACHMENT }
{
    setWindowTitle("Аутентификация");
    setFixedSize(300, 150);
    auto *layout { new QVBoxLayout(this) };
    layout->addWidget( new QLabel { "Логин:" });
    layout->addWidget( m_login_edit);
    layout->addWidget( new QLabel { "Пароль:" });
    m_password_edit->setEchoMode(QLineEdit::Password);
    layout->addWidget( m_password_edit);

    auto* buttons { new QDialogButtonBox { QDialogButtonBox::Ok | QDialogButtonBox::Cancel } };
    connect(buttons, &QDialogButtonBox::accepted, this, &LoginDialog::onLoginClicked);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    layout->addWidget(buttons);

    connect(m_login_edit, &QLineEdit::returnPressed, [this] { m_password_edit->setFocus(); });
    connect(m_password_edit, &QLineEdit::returnPressed, this, &LoginDialog::onLoginClicked);

    m_login_edit->setFocus();
}

//-----------------


void LoginDialog::onLoginClicked()
{
    if(detail::checkInputData(m_login_edit->text(), m_password_edit->text()))
    {
        accept();
        return;
    }

    if( m_attempts <= 0)
    {
        QMessageBox::critical(
            this,
            "Доступ запрещен",
            "Исчерпаны все попытки входа.\n"
            "Приложение будет закрыто."
            );
        reject();
        return;
    }

    QMessageBox::warning(
        this,
        "Ошибка входа",
        QString{
            "Неверный логин или пароль.\n"
            "Осталось попыток: %1"
        }.arg(m_attempts--)
    );

    m_password_edit->clear();
    m_password_edit->setFocus();
}

//-----------------

QString
LoginDialog::getLogin() const
{
    return m_login_edit->text();
}

//-----------------

QString
LoginDialog::getPassword() const
{
    return m_password_edit->text();
}

//-----------------