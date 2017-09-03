#ifndef LINEEDITSETTER_H
#define LINEEDITSETTER_H
#include "entities.h"
#include <QString>
#include <QWidget>

class QLineEdit;
class QLabel;

/*      Widget typu :
 *       [----------]
 *       [  label   ]
 *       [ lineEdit ]
 *       [----------]
 */

using namespace constants;

class lineEditSetter : public QWidget
{
public:
    // Konstruktor
    lineEditSetter(QString str, QString regExp = ".*",QString *thirdString = nullptr,QSize fixedSize = QSize(LINEEDITWIDTH, LINEEDITHEIGHT), QWidget *parent = 0);

    // publiczne widget'y
    QLineEdit *lineEdit;
    QLabel    *label;

};

#endif // LINEEDITSETTER_H
