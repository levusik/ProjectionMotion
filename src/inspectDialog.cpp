#include "inspectDialog.h"
#include "mainwindow.h"
#include "entities.h"
#include "step.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qgraphicsview.h>
#include <qpushbutton.h>
#include <qmath.h>
using namespace constants;

// C++ modulo z ujemnej liczby zwraca ujemn¹ liczbê
// a my chcemy zwróciæ (range - 1)
void modulo(int &value, const int &range)
{
	if (value < 0)
		value = range - 1;
	value %= range;
}

/*---------------------------------------------------------------------------*/
inspectDialog::inspectDialog(QVector<step*> &list, QGraphicsView &view, MainWindow *mainWindowPtr)
	: listRef(list), viewRef(view)
{
		this->windowPtr = mainWindowPtr;

		// widget niemodalny [ nie czekamy na odpowiedŸ u¿ytkownika, mo¿na je zminimalizowaæ]
		this->setModal(false);

		// inicjalizacja g³ównego layout'u
		this->mainLayout = new QVBoxLayout;

		// inicjalizacja group boxa
		this->groupBox   = new QGroupBox("0/0");

		// inicjalizacja layoutu group box'a
		QVBoxLayout *groupBoxLayout = new QVBoxLayout;

		// inicjalizacja labeli które wyœwietl¹ wartoœci
		this->posXLabel   = new QLabel;
		this->posYLabel   = new QLabel;
		this->angleLabel  = new QLabel;
		this->timeLabel   = new QLabel;
		this->vecXLabel   = new QLabel;
		this->vecYLabel	  = new QLabel;

		// dodanie labelów do layoutu group box'a
		groupBoxLayout->addWidget(this->posXLabel);
		groupBoxLayout->addWidget(this->posYLabel);
		groupBoxLayout->addWidget(this->angleLabel);
		groupBoxLayout->addWidget(this->timeLabel);
		groupBoxLayout->addWidget(this->vecXLabel);
		groupBoxLayout->addWidget(this->vecYLabel);

		this->groupBox->setLayout(groupBoxLayout);

		// wype³nienie domyœlnymi wartoœciami
		this->setNewTextsToLabel("-", "-", "-", "-", "-", "-");

		// inicjalizacja guzików 
		this->nextValueButton = new QPushButton("nastepny");
		this->nextValueButton->setFixedSize(100, 50);
		connect(this->nextValueButton, &QPushButton::clicked, this, &inspectDialog::nextValue);

		this->prevValueButton = new QPushButton("poprzedni");
		this->prevValueButton->setFixedSize(100, 50);
		connect(this->prevValueButton, &QPushButton::clicked, this, &inspectDialog::prevValue);

		QHBoxLayout *buttonLayout = new QHBoxLayout;
		
		buttonLayout->addWidget(this->prevValueButton);
		buttonLayout->addWidget(this->nextValueButton);


		// okienko sta³ych rozmiarów
		this->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

		// dodanie guzików do layoutu group box'a
		groupBoxLayout->addLayout(buttonLayout);

		this->mainLayout->addWidget(this->groupBox);

		// ustawienie layout'u
		this->setLayout(this->mainLayout);
}

/*---------------------------------------------------------------------------*/
void inspectDialog::setNewBegginingValues(double startingV, double startingAngle)
{
	this->vOnBegginingOfThrow   = startingV;
	this->angleOnBeggingOfThrow = startingAngle;

	// Vx jest sta³e podczas ca³ego rzutu tak¿e nie musimy go potem liczyæ
	this->actualVX = qCos(this->angleOnBeggingOfThrow * M_PI / 180.) * this->vOnBegginingOfThrow;

	// obliczamy pocz¹tkowe Vy [ dziêki czemu mo¿emy go potem u¿yæ w obliczeniach]
	this->startingVY = qSin(this->angleOnBeggingOfThrow * M_PI / 180.) * this->vOnBegginingOfThrow;
}

/*---------------------------------------------------------------------------*/
void inspectDialog::updateValues(bool needToCenter)
{
	// update tytu³u group boxa [ wyœwietlamy go w formie  [aktualny index] / [ rozmiar listy ] 
	this->groupBox->setTitle(QString::number(this->currentItemIndex + 1) + "/" + QString::number(this->listRef.size()));
	
	if (this->listRef.size())
	{
		// upewniamy siê ¿e wszystko posz³o w porz¹dku i operujemy na dobrych indeksach
		modulo(this->currentItemIndex, this->listRef.size());

		// obliczenie nowego VY [ vy = vy0 - g * t ] 
		this->listRef[this->currentItemIndex];
		double newVY = this->listRef[this->currentItemIndex]->getVecYValue();

		// obliczenie nowego k¹ta
		double newAngle = atan(newVY / this->actualVX) * 180. / M_PI;

		// ustawienie nowych wartoœci do labela
		this->setNewTextsToLabel(QString::number(this->listRef[this->currentItemIndex]->getPosX()), // pobranie pozycji X
			QString::number(this->listRef[this->currentItemIndex]->getPosY()), // pobranie pozycji Y
			QString::number(this->listRef[this->currentItemIndex]->getTime()), // pobranie czasu
			QString::number(newAngle),											// podanie nowego k¹ta 
			QString::number(this->actualVX),									// podanie wektora sk³adowego Vx [ zawsze jest taki sam ale jest w tym
																				//								   samym labelu z tekstem] 
			QString::number(newVY));											// podanie nowego Vy

		this->listRef[this->currentItemIndex]->setBrush(STEP_INSPECTED_COLOR);
		this->listRef[this->currentItemIndex]->setZValue(4);
		if (needToCenter)
			this->viewRef.centerOn(this->listRef[this->currentItemIndex]);
	}
	else
		this->setNewTextsToLabel("-", "-", "-", "-", "-", "-");

	// wyj¹tkowo nieleganckie rozwi¹zanie ;_;
	this->windowPtr->updateVectors();
}

/*---------------------------------------------------------------------------*/
step * inspectDialog::getCurrentlyInspectedItem()
{
	// upewniamy siê ¿e operujemy na dobrych indeksach
	this->currentItemIndex %= this->listRef.size();

	return this->listRef[this->currentItemIndex];
}

/*---------------------------------------------------------------------------*/
void inspectDialog::nextValue()
{
	if (this->listRef.size())
	{
		this->currentItemIndex %= this->listRef.size();
		this->listRef[this->currentItemIndex]->setBrush(STEP_SELECTED_COLOR);
		this->listRef[this->currentItemIndex]->setZValue(0);
		this->currentItemIndex++;	// modulo zostanie wywo³ane w update
		this->updateValues(true);
	}
}

/*---------------------------------------------------------------------------*/
void inspectDialog::prevValue()
{
	if (this->listRef.size())
	{
		this->currentItemIndex %= this->listRef.size();
		this->listRef[this->currentItemIndex]->setBrush(STEP_SELECTED_COLOR);
		this->listRef[this->currentItemIndex]->setZValue(0);
		this->currentItemIndex--;   // modulo zostanie wywo³ane w update
		this->updateValues(true);
	}
}

/*---------------------------------------------------------------------------*/
void inspectDialog::setNewTextsToLabel(const QString & posX, const QString & posY, const QString & time,
									   const QString & angle, const QString & vecX, const QString & vecY)
{
	// update tytu³u group boxa [ wyœwietlamy go w formie  [aktualny index] / [ rozmiar listy ] 
	this->groupBox->setTitle(QString::number(this->currentItemIndex+1) + "/" + QString::number(this->listRef.size()));

	// metoda która ustawi teksty we wszystkich label'ach
	if (posXLabel->text() != posX)
	{
	this->vecXLabel->setText(BOLDFACE + VECX_STR + vecX + " m/s" + END_BOLDFACE);
	this->vecXLabel->update();
	}
	
	this->vecYLabel->setText( BOLDFACE + VECY_STR + vecY + " m/s" + END_BOLDFACE);
	this->vecYLabel->update();

	this->posXLabel->setText(BOLDFACE + POS_X_STR + posX +" m" + END_BOLDFACE);
	this->posXLabel->update();

	this->posYLabel->setText(BOLDFACE + POS_Y_STR + posY + " m" + END_BOLDFACE);
	this->posYLabel->update();

	this->timeLabel->setText(BOLDFACE + TIME_LABEL_STR + time + " s" + END_BOLDFACE);
	this->timeLabel->update();
	
	this->angleLabel->setText(BOLDFACE + ANGLE_STR + angle + " stopni" + END_BOLDFACE);
	this->angleLabel->update();
}

/*---------------------------------------------------------------------------*/
