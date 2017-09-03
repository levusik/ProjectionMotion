#pragma once

#include <QGraphicsEllipseItem>

class QGraphicsLineItem;

class step : public QGraphicsEllipseItem
{
public:
	//		  pozycja w systemie		pozycja w systemie 
	//			informatycznym				fizycznym
	step(double PosX, double PosY, double pPosX, double pPosY,  double time,double vecXValue, double vecYValue, bool isMaxHeigth = false);
	~step();

	// metody kt�re zajm� si� wy�wietlaniem i wyr�nianiem kropki
	void   select();
	void   unSelect();

	// metody kt�re poka�� i usun� wektory sk�adowe 
	void   showVectors( double scale);
	void   hideVectors();

	// gettery
	double getPosX();
	double getPosY();
	double getVecXValue();
	double getVecYValue();
	double getTime();
	bool   isSelected();
	bool   hasShowedVector();
	int	   getIndexSelected();
	int	   getIndex();
	QGraphicsLineItem *getVx();
	QGraphicsLineItem *getVy();
	QGraphicsLineItem *getV();

private:
	// wektory sk�adowe kt�re b�dziemy rysowa� na scenie
	QGraphicsLineItem *v  = nullptr;
	QGraphicsLineItem *vx = nullptr;
	QGraphicsLineItem *vy = nullptr;


	
	int	    selectedIndex	  = INT_MIN;
	int     index			  = 0;
	double  posX		      = 0;
	double	posY			  = 0;
	double  vecXValueInM	  = 0;
	double  vecYValueInM	  = 0;
	double  scenePosX		  = 0;
	double	scenePosY		  = 0;
	double  time		      = 0;
	bool    isSelectedBoolean = false;
	bool    isMaxHeigth		  = false;
	bool    showedVectors	  = false;
};