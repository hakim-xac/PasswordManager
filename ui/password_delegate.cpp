#include "password_delegate.h"
#include <QLineEdit>

namespace GlobalVariables {

static const int NUMBER_STARS { 10 };

}

PasswordDelegate::PasswordDelegate(QWidget *parent) :
    QStyledItemDelegate { parent }
{
}

QWidget* PasswordDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QLineEdit* editor { new QLineEdit(parent) };
    editor->setEchoMode(QLineEdit::Normal);
    editor->setPlaceholderText("Введите пароль");
    return editor;
}

void PasswordDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    QString value { index.model()->data(index, Qt::EditRole).toString() };
    QLineEdit* lineEdit { static_cast<QLineEdit*>(editor) };
    lineEdit->setText(value);
}

void PasswordDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    QLineEdit* lineEdit { static_cast<QLineEdit*>(editor) };
    model->setData(index, lineEdit->text(), Qt::EditRole);
}

void PasswordDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyleOptionViewItem opt = option;
    opt.text = QString (GlobalVariables::NUMBER_STARS, '*');

    QStyledItemDelegate::paint(painter, opt, index);
}

QString PasswordDelegate::displayText(const QVariant& value, const QLocale& locale) const
{
    std::ignore = value;
    return QString(GlobalVariables::NUMBER_STARS, '*');
}
