#include "lineeditsetter.h"
#include <QLineEdit>
#include <QLabel>
#include <QValidator>
#include <QLayout>

lineEditSetter::lineEditSetter(QString str, QString regExp,QString *thirdArgument, QSize fixedSize , QWidget *parent)
    : QWidget(parent)
{
    // inicjalizacja layout'u
    QHBoxLayout *layout  = new QHBoxLayout(this);

    // inicjalizacje widgetów ( labela i lineEdit'a)
    this->label = new QLabel(str);
    this->lineEdit = new QLineEdit();
    this->lineEdit->setValidator(new QRegExpValidator(QRegExp(regExp)));

    // dodanie widgetów do layout'u
    layout->addWidget(this->label);
    layout->addWidget(this->lineEdit);

    if (thirdArgument != nullptr)
    {
        QLabel *secLabel = new QLabel(*thirdArgument);
        layout->addWidget(secLabel);
    }
    // ustawienie resize policy
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // ustawienie stałego rozmiaru
    this->setFixedSize(fixedSize);

    // ustawienie layout'u
    this->setLayout(layout);
}
