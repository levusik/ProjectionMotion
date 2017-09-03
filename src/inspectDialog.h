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
	//														 tragiczne rozw¹zanie TODO: poprawiæ !
	inspectDialog(QVector<step*> &List, QGraphicsView &view, MainWindow *mainWindowPtr);
	
	// metoda która jest wywo³ywana podczas inicjalizacji symulacji 
	void setNewBegginingValues(double startingV, double startingAngle);

	void updateValues(bool needToCenter);	// metoda wywo³ywana gdy w g³ównym obiekcie symulacji 
											// u¿ytkownik wybierze nowy "krok"
	
	// getter
	step *getCurrentlyInspectedItem();

public slots:
	// sloty które wywo³a g³ówne okienko
	void nextValue();
	void prevValue();


private:
	// indeks "kroku" którego parametry aktualnie wyœwietlamy
	int currentItemIndex = 0;

	// metoda która ustawi nowego teksty
	void setNewTextsToLabel(const QString& posX, const QString& posY, const QString& time,
							const QString& ang, const QString& vecX, const QString & vecY);

	double vOnBegginingOfThrow	 = 0;
	double angleOnBeggingOfThrow = 0;
	double actualVX   = 0;
	double startingVY = 0;

	// wskaŸnik do g³ównego okienka 
	MainWindow *windowPtr = nullptr;


	// adresy naszych widgetów
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

	// wskaŸnik do listy z krokami
	QVector<step*>	   &listRef;	// dlaczego wektor ? bo qLinkedList mi nie dzia³a³o ;_;

	// wskaŸnik do sceny graficznej
	QGraphicsView	   &viewRef;
};
