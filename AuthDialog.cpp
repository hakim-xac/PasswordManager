#include "AuthDialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QDebug>
#include "kas/base64.h"
#include "kas/sha256.h"
#include "settings/settings.h"

namespace GlobalVariables {

static constexpr int NUMBER_LOGIN_ATTACHMENT{ 5 };
}

extern const settings::Settings& SETTINGS();

namespace detail {

//-----------------

static bool
verify( const QString& password)
{
    return kas::crypto::Sha256::verifyPasswordWithSalt(
        password,
        kas::crypto::base64::encode(
            SETTINGS().get<settings::tags::application_author_t>() +
            SETTINGS().get<settings::tags::application_name_t>()
            ),
        SETTINGS().get<settings::tags::public_hash_key_t>()
        );
}

//-----------------

}

//-----------------

AuthDialog::AuthDialog(QWidget *parent) :
    QDialog{ parent },
    m_key_edit{ new QLineEdit },
    m_attempts { GlobalVariables::NUMBER_LOGIN_ATTACHMENT }
{
    setWindowTitle("Аутентификация");
    setFixedSize(300, 150);
    auto *layout { new QVBoxLayout(this) };
    layout->addWidget( new QLabel { "Введите ключ доступа:" });
    m_key_edit->setEchoMode(QLineEdit::Password);
    layout->addWidget( m_key_edit);

    auto* buttons { new QDialogButtonBox { QDialogButtonBox::Ok | QDialogButtonBox::Cancel } };
    connect(buttons, &QDialogButtonBox::accepted, this, &AuthDialog::onAuthClicked);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    layout->addWidget(buttons);

    connect(m_key_edit, &QLineEdit::returnPressed, this, &AuthDialog::onAuthClicked);

    m_key_edit->setFocus();
}

//-----------------

void AuthDialog::onAuthClicked()
{
    if(detail::verify(m_key_edit->text()))
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

    m_key_edit->clear();
    m_key_edit->setFocus();
}

//-----------------