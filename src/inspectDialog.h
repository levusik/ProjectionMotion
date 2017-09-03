#pragma once

#include <qlist.h>
#include <qvector.h>
#include <QDialog>
#include "entities.h"

using namespace constants;

class step;
class QLabel;
class QGroupBox;
class QVBoxLayout;
class QGraphicsView;
class MainWindow;

class inspectDialog : public QDialog
{
	Q_OBJECT

public:
	//														 tragiczne rozw�zanie TODO: poprawi� !
	inspectDialog(QVector<step*> &List, QGraphicsView &view, MainWindow *mainWindowPtr);
	
	// metoda kt�ra jest wywo�ywana podczas inicjalizacji symulacji 
	void setNewBegginingValues(double startingV, double startingAngle);

	void updateValues(bool needToCenter);	// metoda wywo�ywana gdy w g��wnym obiekcie symulacji 
											// u�ytkownik wybierze nowy "krok"
	
	// getter
	step *getCurrentlyInspectedItem();

public slots:
	// sloty kt�re wywo�a g��wne okienko
	void nextValue();
	void prevValue();


private:
	// indeks "kroku" kt�rego parametry aktualnie wy�wietlamy
	int currentItemIndex = 0;

	// metoda kt�ra ustawi nowego teksty
	void setNewTextsToLabel(const QString& posX, const QString& posY, const QString& time,
							const QString& ang, const QString& vecX, const QString & vecY);

	double vOnBegginingOfThrow	 = 0;
	double angleOnBeggingOfThrow = 0;
	double actualVX   = 0;
	double startingVY = 0;

	// wska�nik do g��wnego okienka 
	MainWindow *windowPtr = nullptr;


	// adresy naszych widget�w
	QVBoxLayout		  *mainLayout		 = nullptr;
	QGroupBox		  *groupBox			 = nullptr;
	QLabel			  *mainLabel		 = nullptr;
	QLabel			  *posXLabel		 = nullptr;
	QLabel			  *posYLabel		 = nullptr;
	QLabel			  *angleLabel		 = nullptr;
	QLabel			  *timeLabel		 = nullptr;
	QLabel			  *vecXLabel		 = nullptr;
	QLabel			  *vecYLabel		 = nullptr;
	QPushButton		  *nextValueButton   = nullptr;
	QPushButton		  *prevValueButton   = nullptr;

	// wska�nik do listy z krokami
	QVector<step*>	   &listRef;	// dlaczego wektor ? bo qLinkedList mi nie dzia�a�o ;_;

	// wska�nik do sceny graficznej
	QGraphicsView	   &viewRef;
};
