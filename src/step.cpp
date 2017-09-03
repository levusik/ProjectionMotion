#include "step.h"
#include "entities.h"

#include <qbrush.h>
#include <QGraphicsLineItem>
#include <qpen.h>

using namespace constants;

// globalne zmienne statyczne które bêd¹ kontrolowaæ ile 
// mamy zmiennych i aktualny indeks
static int howManyStepsInitialized = 0;
static int selectedIndexCounter	   = 0;

/*---------------------------------------------------------------*/
//			pozycja w systemie				pozycja w systemie 
//				informatycznym					fizyczny
step::step(double canvasPosX, double canvasPosY, double pPosX, double pPosY, double Time, double VecXValue, double VecYValue, bool IsMaxHeigth)
	: posX(pPosX), posY(pPosY), time(Time), isMaxHeigth(IsMaxHeigth), vecXValueInM(VecXValue), vecYValueInM(VecYValue)
{
	index = howManyStepsInitialized;
	howManyStepsInitialized++;

	double radiusSize = STEP_DIAMETER;

	if (IsMaxHeigth)
	{
		// je¿eli "krok" jest na najwy¿ej wysokoœci to chcemy go nieco wyró¿niæ 
		radiusSize *= 2;
		this->setBrush(QBrush(MAX_HEIGTH_COLOR));
		this->setZValue(1);
	}
	// w przeciwnym wypadku dajemy mu wartoœci defaultowe
	else
		this->setBrush(QBrush(DEF_STEP_COLOR));

	this->setRect(canvasPosX  + BALL_DIAMETER/ 2. - radiusSize / 2., canvasPosY + BALL_DIAMETER /2. - radiusSize / 2., radiusSize, radiusSize);
}

/*---------------------------------------------------------------*/
step::~step()
{
	howManyStepsInitialized--;
	
	if (this->selectedIndex != INT_MIN)
		selectedIndexCounter--;

	this->hideVectors();
}

/*---------------------------------------------------------------*/
void step::select()
{
	this->isSelectedBoolean = true;
	this->setBrush(QBrush(STEP_SELECTED_COLOR));

	this->selectedIndex = selectedIndexCounter;
	selectedIndexCounter++;
}

/*---------------------------------------------------------------*/
void step::unSelect()
{
	this->isSelectedBoolean = false;
	if (!this->isMaxHeigth)
	{
		this->setBrush(QBrush(DEF_STEP_COLOR));
		this->setZValue(0);
	}
	else
	{
		this->setZValue(2);
		this->setBrush(QBrush(MAX_HEIGTH_COLOR));
	}
	this->hideVectors();
	this->selectedIndex = INT_MIN;
	selectedIndexCounter--;
}

/*---------------------------------------------------------------*/
void step::showVectors( double scale)
{
	// pobranie œrodka kulki
	double ballCenterX = this->rect().x() + this->rect().width() / 2.;
	double ballCenterY = this->rect().y() + this->rect().height() / 2.;

	// obliczenie koñców wektorów
	double vecXLengthInPixels = this->vecXValueInM * scale;
	double vecYLengthInPixels = -this->vecYValueInM * scale;


	// dodanie wektora sk³adowego X
	this->vx = new QGraphicsLineItem(ballCenterX, ballCenterY,
									 ballCenterX + vecXLengthInPixels, ballCenterY );
	this->vx->setPen(QPen(VX_LINE_VEC_COLOR));

	// dodanie wektora sk³adowego Y
	this->vy = new QGraphicsLineItem(ballCenterX, ballCenterY,
									 ballCenterX,ballCenterY  + vecYLengthInPixels);
	this->vy->setPen(QPen(VY_LINE_VEC_COLOR));

	// dodanie wektora sk³adowego XY
	this->v = new QGraphicsLineItem(ballCenterX , ballCenterY,
									ballCenterX + vecXLengthInPixels, ballCenterY + vecYLengthInPixels);

	this->showedVectors = true;
}

/*---------------------------------------------------------------*/
void step::hideVectors()
{
	// usuniêcie wektorów
	if (this->vx != nullptr)
	{
		delete this->vx;
		this->vx = nullptr;
	}
	
	if (this->vy != nullptr)
	{
		delete this->vy;
		this->vy = nullptr;
	}

	if (this->v != nullptr)
	{
		delete this->v;
		this->v = nullptr;
	}

	// ustawienie locka
	this->showedVectors = false;
}

/*---------------------------------------------------------------*/
double step::getPosX()
{
	return posX;
}

/*---------------------------------------------------------------*/
double step::getPosY()
{
	return posY;
}

/*---------------------------------------------------------------*/
double step::getVecXValue()
{
	return this->vecXValueInM;
}

/*---------------------------------------------------------------*/
double step::getVecYValue()
{
	return this->vecYValueInM;
}

/*---------------------------------------------------------------*/
double step::getTime()
{
	return time;
}

/*---------------------------------------------------------------*/
bool step::isSelected()
{
	return this->isSelectedBoolean;
}

/*---------------------------------------------------------------*/
bool step::hasShowedVector()
{
	return showedVectors;
}

/*---------------------------------------------------------------*/
int step::getIndexSelected()
{
	return this->selectedIndex;
}

/*---------------------------------------------------------------*/
int step::getIndex()
{
	return index;
}

/*---------------------------------------------------------------*/
QGraphicsLineItem * step::getVx()
{
	return this->vx;
}

/*---------------------------------------------------------------*/
QGraphicsLineItem * step::getVy()
{
	return this->vy;
}

/*---------------------------------------------------------------*/
QGraphicsLineItem * step::getV()
{
	return this->v;
}

/*---------------------------------------------------------------*/
