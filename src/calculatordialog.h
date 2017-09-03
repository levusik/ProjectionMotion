#ifndef CALCULATORDIALOG_H
#define CALCULATORDIALOG_H

#include <QWidget>
#include <QObject>
#include <QDialog>
#include "lineeditsetter.h"
#include "entities.h"

class QLabel;

class calculatorDialog  : public QDialog
{
public:
    // konstruktor
    explicit calculatorDialog();
	~calculatorDialog();

    void setupEditables(int smallWidgetHeigth, int smallWidgetWidth);

private slots:
    void changedComponentVec();
    void changedVOrAngle();
    void changedPosX();
    void changedPosY();
    void changedTime();

private:
    // prywatne metody
    void solveEquations();
    void initializeSetters();
    void setupLayouts();

    // deklaracja stałych
    const int smallWidgetWidth = 100, smallWidgetHeigth = 50;
    const int bigWidgetWidth = 125, bigWidgetHeigth = 50;

    // deklaracje lineEditów
    lineEditSetter *setVx    = nullptr;
    lineEditSetter *setVy    = nullptr;
    lineEditSetter *setV     = nullptr;
    lineEditSetter *setAngle = nullptr;
    lineEditSetter *setPosX  = nullptr;
    lineEditSetter *setPosY  = nullptr;
    lineEditSetter *setTime  = nullptr;

    // wyniki
    QLabel *range = nullptr;
    QLabel *maxHeigth = nullptr;
    QLabel *sinText = nullptr;
    QLabel *cosText = nullptr;
    QLabel *posXResult = nullptr;
    QLabel *posYResult = nullptr;
    QLabel *timeOfThrow = nullptr;
    QLabel *timeToReachMaxHeigth = nullptr;
};


#endif // CALCULATORDIALOG_H
