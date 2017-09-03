#include "mainwindow.h"
#include "qpushbutton.h"
#include "calculatordialog.h"
#include "scalingLineStruct.h"
#include "step.h"
#include "ball.h"
#include "inspectDialog.h"

#include <QGraphicsEllipseItem>
#include <QLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPainterPath>
#include <QLineEdit>
#include <qdebug.h>
#include <QTimer>
#include <QKeyEvent>
#include <QApplication>
#include <qscrollbar.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qpair.h>




/* Główne okienko/widget składa się z layoutu który wygląda tak :
*
*  [ left Widget] | [ graphics View ] | [ right Widget]
*
*  leftWidget   - stały widget który zawiera guzik do rzeczy które warto
*                 mieć pod ręką ( Kalkulator, legenda, reset)
*
*  graphicsView - widok na którym dzieje się symulacja
*
*  rightWidget  - widget który zmienia się w zależności od stanu FSM'a
*/

// ifndef'y dodane bo w kilku IDE takich jak vs i qt creator można sobie to elegancko zwinąć :> 

#ifndef CONSTRUCTOR_AND_DESTRUCTOR

/*-----------------------------------------------------------------------*/
MainWindow::MainWindow(QWidget *parent) :
	QWidget(parent)
{
	// ustawienie głównego layout'u który będziemy update'ować
	this->leftWidget = new QWidget(this);
	this->rightWidget = new QWidget(this);

	// main layout
	mainLayout = new QHBoxLayout;

	// zainicjalizowanie widoku i sceny
	initializeSceneAndView();

	mainLayout->addWidget(this->leftWidget);
	mainLayout->addWidget(this->view);
	mainLayout->addWidget(this->rightWidget);


	// ustawienie layoutu
	this->setLayout(mainLayout);

	// ustawienie lewego stałego layout'u
	this->setLeftStaticWidget();

	this->setupInitThrowWidgets();

	this->view->setOptimizationFlags(QGraphicsView::DontClipPainter);
	this->view->setOptimizationFlags(QGraphicsView::DontSavePainterState);
	this->view->setOptimizationFlags(QGraphicsView::DontAdjustForAntialiasing);
	this->view->setCacheMode(QGraphicsView::CacheBackground);

	inspectValuesDialog = new inspectDialog(selectedSteps, *this->view, this);
	inspectValuesDialog->hide();
}

/*-----------------------------------------------------------------------*/
MainWindow::~MainWindow()
{
	this->deleteLines();
	this->deleteScalingLines();

	for (auto * step : road)
		delete step;
	road.clear();

	if (inspectValuesDialog != nullptr)
		delete inspectValuesDialog;
}

/*-----------------------------------------------------------------------*/
#endif 

#ifndef  LEFT_WIDGET_SLOTS
/*-----------------------------------------------------------------------*/
void MainWindow::openCalculator()
{
	// inicjalizacja, wyświetlenie i usunięcie okienka
	calculatorDialog *dial = new calculatorDialog();

	dial->exec();

	delete dial;
}

/*-----------------------------------------------------------------------*/
void MainWindow::resetAnimation()
{
	// metoda która zresetuje wszystkie ustawienie na domyślne
	// i zwolni wszystkie zasoby

	// powrót do domyślnych ustawień
	this->angle = 0;
	this->updateLines = 0;
	this->velocityInM = 0;
	this->ball->setPos(0, 0);
	this->ball->setBrush(this->idleColor);
	this->currentState = simulationState::INTIALIZE;
	this->range = 0;
	this->maxHeigth = 0.;
	this->prevTime = 0.;
	this->isAnimationPaused = false;
	this->hasMarkedMaxHeigth = false;
	this->animationEnded = false;
	this->showComponentVectors = false;
	this->selectingStepLock = false;
	this->selectedAllPoints = false;
	this->isMotionProjection = false;
	this->showRoadVector = false;
	this->isRoadVisible = true;
	this->showSingleVector = true;
	this->updateLines = false;


	try
	{
		// zwolnienie zasobów
		this->deleteLines();
		this->deleteScalingLines();
		this->view->centerOn(this->ball);
		this->setupInitThrowWidgets();

		if (this->inspectValuesDialog != nullptr)
			this->inspectValuesDialog->hide();

		if (this->isAnimationPaused && this->inspectGroupBox != nullptr)
		{
			delete this->inspectGroupBox;
			this->inspectGroupBox = nullptr;
		}

		if (this->inspectValuesDialog != nullptr)
			this->inspectValuesDialog->updateValues(false);
	}
	catch (std::exception e)
	{
	}
}

/*-----------------------------------------------------------------------*/
void MainWindow::openLegend()
{

}

/*-----------------------------------------------------------------------*/
void MainWindow::zoomIn()
{
	this->view->scale(ZOOMIN, ZOOMIN);
	this->view->centerOn(ball);
}

/*-----------------------------------------------------------------------*/
void MainWindow::zoomOut()
{
	this->view->scale(ZOOMOUT, ZOOMOUT);
	this->view->centerOn(ball);
}

/*-----------------------------------------------------------------------*/
#endif

#ifndef INITIALIZE_THROW_SLOTS 
/*-----------------------------------------------------------------------*/
void MainWindow::setNewPosY(const QString &newText)
{
	this->ball->setY(-newText.toDouble()  * this->mToPxls);
	this->startingPos.setY(-newText.toDouble() * this->mToPxls);

	// jeżeli linijki są wyświetlone to chcemy je przesunąć o przyrost pozycji startowe
	moveLines();
}

/*-----------------------------------------------------------------------*/
void MainWindow::setNewPosX(const QString &newText)
{
	this->ball->setX(newText.toDouble() * this->mToPxls);
	this->startingPos.setX(newText.toDouble() * this->mToPxls);

	// jeżeli linijki są wyświetlone to chcemy je przesunąć o przyrost pozycji startowe
	moveLines();
}

/*-----------------------------------------------------------------------*/
void MainWindow::setNewVelocity(const QString &newText)
{
	this->velocityInM = newText.toDouble();

	// wywołujemy metodę ktora ustawi linie gdy mamy V i angle
	this->setupLinesByVAndAngle();
}

/*-----------------------------------------------------------------------*/
void MainWindow::setNewAngle(const QString &newText)
{
	// pobieramy kąt z qString'a 
	this->angle = newText.toDouble();

	// modulo na kątach > 360.
	if (newText.toDouble() > 360.)
	{
		this->angle = int(this->angle) % 360;
		this->setAngle->lineEdit->setText(QString::number(this->angle));
	}

	this->setupLinesByVAndAngle();
}

/*-----------------------------------------------------------------------*/
void MainWindow::setNewScale(const QString &newText)
{
	this->mToPxls = newText.toDouble();

	if (newText == "")
	{
		this->mToPxls = 1;
		this->setScale->lineEdit->setText("1");
	}
	this->setupLinesByVAndAngle();

}

/*-----------------------------------------------------------------------*/
void MainWindow::runAnimation()
{
	this->currentState = simulationState::SIMULATE;
	this->clock.start();
	this->deleteLines();
	this->setupSimWidgets();
}

/*-----------------------------------------------------------------------*/
#endif

#ifndef  SIMULATE_SLOTS
/*-----------------------------------------------------------------------*/
void MainWindow::pauseOrRunAnim()
{
	if (isAnimationPaused)
	{
		// przechodzimy w stan odpauzowania
		isAnimationPaused = false;

		//resetujemy zegar [ bo mamy zapisane w prevTime czas który upłynął do momentu wciśnięcia guzika ]
		this->clock.restart();

		// aktualizacja tesktów
		this->runOrPause->setText("Zatrzymaj");

		// usunięcie opcji z wyświetlaniem wektorów itp.
		if (this->inspectGroupBox != nullptr)
		{
			delete this->inspectGroupBox;
			inspectGroupBox = NULL;
		}
	}
	else
		changeToPauseState();
}

/*-----------------------------------------------------------------------*/
void MainWindow::changeToPauseState(bool addHoardedTime)
{
	// przechodzimy w stan pauzowania
	isAnimationPaused = true;

	if (addHoardedTime)
		// zapisujemy sobie czas który upłynął do momentu wciśnięcia guzika dzięki czemu możemy sobie  
		this->prevTime += this->clock.elapsed() / 1000.;

	// aktualizacja tekstów 
	this->runOrPause->setText("Wznów");

	// dodanie do prawego widgetu opcji wyświetlania
	this->setupInspectWidgets();
}

/*-----------------------------------------------------------------------*/
void MainWindow::changeShowComponentVect()
{
	if (this->showComponentVectors)
	{
		this->showComponentVectors = false;
		this->showComponentVectorsButton->setText("pokaż wektory składowe");
		this->deleteLines();

	}
	else
	{
		this->showComponentVectors = true;
		this->showComponentVectorsButton->setText("schowaj wektory składowe");

		showVectorsFromBallToPoint(this->velocityX * this->mToPxls,
			-(this->velocityY - (this->prevTime) * G_CONST) * mToPxls);
	}
}

/*-----------------------------------------------------------------------*/
#endif 

#ifndef  INSPECT_SLOTS
/*-----------------------------------------------------------------------*/
void MainWindow::showOrHideRoad()
{
	if (this->isRoadVisible)
		this->showWholeRoadButton->setText("pokaż całą trasę");
	else
		this->showWholeRoadButton->setText("schowaj całą trasę");

	// negacja
	this->isRoadVisible = this->isRoadVisible == true ? false : true;

	// chowamy lub pokazujemy tylko te punkty które nie zostały zaznaczone
	for (auto* step : road)
		if (!step->isSelected())
			step->setVisible(this->isRoadVisible);
}

/*-----------------------------------------------------------------------*/
void MainWindow::selectAllPoints()
{
	// aktualizacja tekstów
	if (selectedAllPoints)
		showAllPointsButton->setText(SHOW_ALL_POINTS_STR);
	else
		showAllPointsButton->setText(HIDE_ALL_POINTS_STR);

	// negacja lock'a
	selectedAllPoints = selectedAllPoints == 0 ? true : false;

	// przechodzimy przez cały wektor drogi i zmieniamy stan punktów
	for (auto * item : road)
	{
		if (selectedAllPoints && !item->isSelected())
			this->selectStep(item);


		else if (!selectedAllPoints && item->isSelected())
			this->unselectItem(item);
	}

	// update dialogu
	if (this->inspectValuesDialog != nullptr)
		this->inspectValuesDialog->updateValues(false);

	// aktualizacja 
	this->updateVectors();
}

/*-----------------------------------------------------------------------*/
void MainWindow::changeShowingVectorState()
{
	// zmiana tekstów
	if (this->showSingleVector)
		this->showSingleVecButton->setText(SHOW_SINGLE_VECTOR_STR);
	else
		this->showSingleVecButton->setText(SHOW_ALL_VECTORS_STR);

	// negacja
	this->showSingleVector = showSingleVector == 0 ? 1 : 0;

	// aktualizacja 
	this->updateVectors();
}

/*-----------------------------------------------------------------------*/
void MainWindow::showVectors()
{
	if (this->showVectorsButton != nullptr)
	{
		// aktualizacja tekstów
		if (this->showRoadVector)
			showVectorsButton->setText(SHOW_ROAD_VECTORS_STR);
		else
			showVectorsButton->setText(HIDE_ROAD_VECTORS_STR);
	}
	// negacja 
	this->showRoadVector = this->showRoadVector == 0 ? 1 : 0;

	updateVectors();

}

/*-----------------------------------------------------------------------*/
void MainWindow::updateVectors()
{
	if (selectedSteps.size())
	{
		// iterujemy przez wszystkie kroki w wektorze kroków
		for (auto* item : this->selectedSteps)
		{
			// pokazujemy
			if (!showSingleVector && (this->showRoadVector && !item->hasShowedVector()))
				displayVectorsOfItem(item);

			else if (showSingleVector || (!this->showRoadVector && item->hasShowedVector()))
				item->hideVectors();
		}

		if (showSingleVector && this->showRoadVector)
		{
			step *item = this->inspectValuesDialog->getCurrentlyInspectedItem();
			this->displayVectorsOfItem(item);
		}
	}
}

/*-----------------------------------------------------------------------*/
void MainWindow::displayVectorsOfItem(step * item)
{
	item->showVectors(this->mToPxls);

	this->scene->addItem(item->getVx());
	this->scene->addItem(item->getVy());
	this->scene->addItem(item->getV());
}

/*-----------------------------------------------------------------------*/
void MainWindow::unselectItem(step * castedItem)
{
	selectingStepLock = true;
	castedItem->unSelect();

	// TODO: nie znalazłem ładniejszego rozwiązania :(
	for (auto* item : this->selectedSteps)
	{
		if (castedItem->getIndex() == item->getIndex())
			selectedSteps.removeOne(item);
	}

	// aktualizujemy
	if (this->inspectValuesDialog != nullptr)
		this->inspectValuesDialog->updateValues(false);

	this->updateVectors();
}

/*-----------------------------------------------------------------------*/
void MainWindow::selectStep(step * castedItem)
{
	selectingStepLock = true;
	castedItem->select();

	// dodajemy do linked list'a zaznaczony element dzięki czemu 
	// możemy go sobie potem elegancko podejrzeć
	this->selectedSteps.push_back(castedItem);

	// aktualizujemy
	if (this->inspectValuesDialog != nullptr)
		this->inspectValuesDialog->updateValues(false);

	this->updateVectors();
}

/*-----------------------------------------------------------------------*/
#endif

#ifndef  MOUSE_INPUT

/*-----------------------------------------------------------------------*/
void MainWindow::mousePressEvent(QMouseEvent *event)
{
	switch (this->currentState)
	{
	case simulationState::INTIALIZE:
		this->manageMouseEventsInitState(event);
		break;
	case simulationState::SIMULATE:
		this->manageMouseEventsSimState(event);
		break;
	}

}

/*-----------------------------------------------------------------------*/
void MainWindow::manageMouseEventsInitState(QMouseEvent *event)
{
	QPoint viewPoint = this->view->mapFromGlobal(QCursor::pos());
	scenePoint = this->view->mapToScene(viewPoint);
	this->scenePoint -= this->startingPos;

	if (this->ball->contains(scenePoint))
	{
		this->updateLines = true;
		this->ball->setBrush(QBrush(markedColor));
	}
}

/*-----------------------------------------------------------------------*/
void MainWindow::manageMouseEventsSimState(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton && this->isAnimationPaused)
	{
		// mapujemy kursor na scenę graficzną
		QPoint viewPoint = this->view->mapFromGlobal(QCursor::pos());
		scenePoint = this->view->mapToScene(viewPoint);

		// tworzymy graficzną reprezentację kursora [ trochę większego ] przez co będziemy mogli wywołać metodę collidingItems
		QGraphicsEllipseItem *visualCursor = new QGraphicsEllipseItem(scenePoint.x() - CURSOR_CIRC_DIAMETER / 2., scenePoint.y() - CURSOR_CIRC_DIAMETER / 2.,
			CURSOR_CIRC_DIAMETER, CURSOR_CIRC_DIAMETER);
		// dodajemy do sceny
		this->scene->addItem(visualCursor);

		// iterujemy przez wszystkie graficzne przedmioty i sprawdzamy czy 
		// nie mamy doczynienia z "krokiem" [ dynamic_cast ] oraz czy nie został
		// kliknięty LPM
		for (auto *item : visualCursor->collidingItems())
		{
			step *castedItem = dynamic_cast<step*>(item);
			if (castedItem != NULL)
			{
				// zaznaczenie kroku
				if (!castedItem->isSelected() && !this->selectingStepLock)
					selectStep(castedItem);

				// odznaczenie kroku
				else if (castedItem->isSelected() && !this->selectingStepLock)
					unselectItem(castedItem);

				if (this->inspectValuesDialog != nullptr)
					this->inspectValuesDialog->updateValues(false);
			}
		}

		delete visualCursor;
	}
	else
		selectingStepLock = false;
}

/*-----------------------------------------------------------------------*/
#endif 

#ifndef LINE_MANAGING

/*-----------------------------------------------------------------------*/
void MainWindow::moveLines()
{
	if (this->velocityVec != NULL && this->velocityVecVX != NULL && this->velocityVecVY != NULL)
	{
		QPointF dPos = this->startingPos - this->prevStartingPos;
		this->velocityVec->moveBy(dPos.x(), dPos.y());
		this->velocityVecVX->moveBy(dPos.x(), dPos.y());
		this->velocityVecVY->moveBy(dPos.x(), dPos.y());
	}

	// zapisujemy poprzednią pozycję startową ze względu na obliczenie dPos
	this->prevStartingPos = this->startingPos;
}

/*-----------------------------------------------------------------------*/
void MainWindow::setupLinesByVAndAngle()
{
	// jeżeli podano siłę oraz kąt to chcemy wyświetlić 'wizualizację' (?) jako linie
	if (this->setAngle->lineEdit->text() != "" && this->setV->lineEdit->text() != "")
	{
		// obliczamy funkcje trygonometryczne
		double sin = qSin(this->angle * M_PI / 180.);
		double cos = qCos(this->angle * M_PI / 180.);

		// usuwamy zbędne linie
		this->deleteLines();

		double vxEndPos = cos * this->velocityInM * this->mToPxls;
		double vyEndPos = sin * this->velocityInM * this->mToPxls;

		// wywołanie metody która narysuje wektory składowe
		this->showVectorsFromBallToPoint(vxEndPos, -vyEndPos);
	}
	else
		deleteLines();
}

/*-----------------------------------------------------------------------*/
void MainWindow::deleteLines()
{
	if (this->velocityVec != NULL)
	{
		delete this->velocityVec;
		this->velocityVec = NULL;
	}

	if (this->velocityVecVX != NULL)
	{
		delete this->velocityVecVX;
		this->velocityVecVX = NULL;
	}

	if (this->velocityVecVY != NULL)
	{
		delete this->velocityVecVY;
		this->velocityVecVY = NULL;
	}
}

/*-----------------------------------------------------------------------*/

#endif

#ifndef  UPDATE

/*-----------------------------------------------------------------------*/
void MainWindow::update()
{
	// w zależności od stanu FSM'a wywołujemy odpowiednie akcje
	switch (this->currentState) {
	case simulationState::INTIALIZE:
		this->updateInitializeState();
		break;
	case simulationState::SIMULATE:
		this->updateSimulateState();
		break;
	}


}

/*-----------------------------------------------------------------------*/
void MainWindow::updateInitializeState()
{
	if (this->updateLines)
	{
		// mapujemy kursor na scenę graficzną
		QPoint viewPoint = this->view->mapFromGlobal(QCursor::pos());
		scenePoint = this->view->mapToScene(viewPoint);

		this->showVectorsFromBallToPoint(scenePoint.x(), scenePoint.y(), false);

		if (!(QApplication::mouseButtons() & Qt::MouseButton::LeftButton))
		{
			// nie chemy update'ować linijek i ustawiamy kolor na default'owy
			this->updateLines = false;
			this->ball->setBrush(QBrush(idleColor));
		}


		// obliczamy sobie dystans od środka kulki
		double vx = scenePoint.x() - (this->ball->pos().x() + BALL_DIAMETER / 2.);
		double vy = (this->ball->pos().y() + BALL_DIAMETER / 2.) - scenePoint.y();

		if (vx != prevVx && vy != prevVy)
		{
			double vInPxls = sqrt(vx * vx + vy * vy);
			//
			this->velocityInM = vInPxls / mToPxls;
			this->angle = qAsin(vy / vInPxls) * 180. / M_PI;
			this->setV->lineEdit->setText(QString::number(vInPxls / mToPxls));
			this->setAngle->lineEdit->setText(QString::number(qAsin(vy / vInPxls) * 180. / M_PI));

			prevVx = vx;
			prevVy = vy;
		}

		this->view->viewport()->update();

	}
}

/*-----------------------------------------------------------------------*/
void MainWindow::updateSimulateState()
{
	// jeżeli nie skończyliśmy animacji i nie mamy pauzy to pozwalamy kulce się poruszać
	if (!this->isAnimationPaused && !animationEnded)
	{
		// korzystamy ze wzorów zawartych w singleton'ie 
		double xPos = throwMath::calculateXPos(this->prevTime + this->clock.elapsed() / 1000.,
			this->velocityInM * qCos(this->angle * M_PI / 180.), startingPos.x() / mToPxls);

		double yPos = throwMath::calculateYPos(this->prevTime + this->clock.elapsed() / 1000.,
			this->velocityInM * qSin(this->angle * M_PI / 180.), -startingPos.y() / mToPxls);


		// aktualizacja pozycji kulki
		this->ball->setPos(xPos * mToPxls,
			-yPos * mToPxls);					  // qt ma system koordynatów informatyczny ( oś Y w dół)
												  // więc mnożymy przez -1 żeby przekonwertować tę wartość z fizycznego na informatyczny

		this->updateThrowEquation(xPos, yPos);

		// jeżeli kulka przeszła poniżej linii 0 na osi Y to zatrzymujemy animację 
		if (this->ball->pos().y() > 0)
		{
			// ustawiamy na pozycję 0 żeby zapobiec zapadnięciu się
			// poza deskę 
			this->ball->setY(0);

			this->animationEnded = true;

			// zmiana na stan pauzy 
			this->changeToPauseState(false);
		}

		manageAddingStepToRoad(yPos, xPos);

		updateDynamicTexts(xPos, yPos);

		if (this->showComponentVectors)
			showVectorsFromBallToPoint(this->velocityX * this->mToPxls,
				-(this->velocityY - (this->prevTime + this->clock.elapsed() / 1000.) * G_CONST) * mToPxls);

		this->view->centerOn(this->ball);
	}
	// w przeciwnym przypadku jesteśmy w stanie podglądania wartości
	// sprawdzamy wtedy czy LPM nie jest kliknięty [ ? ] i jeżeli tak
	// to usuwamy blokadę 
	else
	{
		// 
		if (!(QApplication::mouseButtons() & Qt::LeftButton))
			this->selectingStepLock = false;
	}
}

/*-----------------------------------------------------------------------*/
void MainWindow::updateDynamicTexts(double xPos, double yPos)
{

	// ustawienie czasu 
	this->timeLabel->setText(BOLDFACE + TIME_LABEL_STR + QString::number(double(this->prevTime + this->clock.elapsed() / 1000.)) + " s" + END_BOLDFACE);
	this->timeLabel->update();

	// ustawienie pozycji 
	this->posLabel->setText(BOLDFACE + POS_X_STR + QString::number(correctParams(xPos)) + "m  :  " + POS_Y_STR + QString::number(correctParams(yPos)) + "m" + END_BOLDFACE);

	//ustawienie nowych wektorów składowych
	this->velocitiesLabel->setText(BOLDFACE + VELOCITY_X_STR + QString::number(correctParams(velocityX)) + "m&frasl;s  | " + VELOCITY_Y_STR +
		QString::number(correctParams(velocityY - G_CONST * (this->prevTime + this->clock.elapsed() / 1000.))) + "m&frasl;s" + END_BOLDFACE);

}

/*-----------------------------------------------------------------------*/
void MainWindow::showVectorsFromBallToPoint(double xPos, double yPos, bool addBallPosToGivenPos)
{
	this->deleteLines();

	// obliczenie środka kółka
	double ballMiddleX = this->ball->x() + BALL_DIAMETER / 2.;
	double ballMiddleY = this->ball->y() + BALL_DIAMETER / 2.;

	xPos += addBallPosToGivenPos == true ? ballMiddleX : 0;
	yPos += addBallPosToGivenPos == true ? ballMiddleY : 0;

	// dodajemy linijki poprawdzone od środka kulki do wskazanego punktu
	this->velocityVecVX = new QGraphicsLineItem(ballMiddleX, ballMiddleY,
		xPos, ballMiddleY);
	this->velocityVecVX->setPen(QPen(VX_LINE_VEC_COLOR));
	this->velocityVecVX->setZValue(2);
	this->scene->addItem(this->velocityVecVX);


	this->velocityVecVY = new QGraphicsLineItem(ballMiddleX, ballMiddleY,
		ballMiddleX, yPos);
	this->velocityVecVY->setPen(QPen(VY_LINE_VEC_COLOR));
	this->velocityVecVY->setZValue(2);
	this->scene->addItem(this->velocityVecVY);

	this->velocityVec = new QGraphicsLineItem(ballMiddleX, ballMiddleY,
		xPos, yPos);
	this->velocityVec->setPen(QPen(V_LINE_VEC_COLOR));
	this->velocityVec->setZValue(2);
	this->scene->addItem(this->velocityVec);

}

/*-----------------------------------------------------------------------*/
void MainWindow::manageAddingStepToRoad(double yPos, double xPos)
{
	// jeżeli czas na zegarze jest większy od pewnej stałej to chcemy dodać krok do naszej drogi [ żółtą kulkę do wektora żółtych kulek ]
	// i zresetować zegar
	if (this->isFirstStep || double(this->addStepToRoadClock.elapsed()) / 1000. > ADD_STEP_TO_ROAD_TIME)
	{
		bool isMaxHeigth = !this->hasMarkedMaxHeigth && yPos >= this->maxHeigth * APPROX ? true : false;
		if (isMaxHeigth == true)
			this->hasMarkedMaxHeigth = true;

		// inicjalizacja kulki
		double actualTime = this->prevTime + this->clock.elapsed() / 1000.;
		step *actualStep = new step(this->ball->x(), this->ball->y(),							// wyliczona pozycja kroku
			xPos, yPos,													// pozycja na scenie 
			actualTime,													// przekazanie aktualnej wartości czasu
			this->velocityX, this->velocityY - G_CONST * actualTime,	// wartość wektorów
			isMaxHeigth);												// czy krok jest maksymalną wysokością

																		// ustawienie visiblity "kroku"
		actualStep->setVisible(this->isRoadVisible);

		// dodanie kulki do drogi
		this->road.push_back(actualStep);

		// dodanie kulki do sceny
		this->scene->addItem(actualStep);

		// reset zegara
		this->addStepToRoadClock.restart();

		// wyjątkowo głupie rozwiązanie :/
		if (this->isFirstStep)
			this->isFirstStep = false;
	}
}

/*-----------------------------------------------------------------------*/
void MainWindow::updateThrowEquation(double xPos, double yPos)
{
	// inicjalizacja nowego tekstu
	QString Equation = BOLDFACE + CURSIVE;
	Equation += QString::number(yPos) + " = ";

	// jeżeli mamy doczynienia z rzutem ukośnym to update'ujemy teksty wedle poniższego schematu
	if (isMotionProjection)
	{
		Equation += QString::number(xPos) + "*";
		Equation += QString::number(qTan(this->angle * M_PI / 180.)) + " - ";
		Equation += "<font size = '5'><sup>" + QString::number(G_CONST) + "</sup>&frasl;";
		Equation += "<sub>" + QString::number(2.* qPow(this->velocityX, 2)) + "</sub></font>";
		Equation += " * " + QString::number(xPos * xPos);
		Equation += END_BOLDFACE + END_CURSIVE;
	}
	else
	{
		Equation += QString::number(-this->startingPos.y() / mToPxls) + " - ";
		Equation += "<font size = '5'><sup>" + QString::number(G_CONST) + " * " + QString::number(xPos * xPos) + "</sup>&frasl;";
		Equation += "<sub>" + QString::number(2. * this->velocityInM * this->velocityInM) + "</sub></font>";
		Equation += END_BOLDFACE + END_CURSIVE;
	}
	this->throwEquationLabel->setText(Equation);
	this->throwEquationLabel->update();
}

/*-----------------------------------------------------------------------*/
#endif 

#ifndef  INITIALIZATION

/*-----------------------------------------------------------------------*/
void MainWindow::initializeSceneAndView()
{
	// inicjalizacja sceny
	this->scene = new QGraphicsScene(this);
	this->view = new QGraphicsView();
	this->view->setScene(this->scene);

	// chcemy by widok zmieniał swój rozmiar w zależności od rozmiaru okienka
	this->view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	// widok jest skierowany na scenę
	this->view->setScene(this->scene);

	// chcemy by zawsze można było pobawić się scrollbarami
	this->view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	this->view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	// aktualizujemy co 60 klatek
	QTimer *timer = new QTimer(this);
	timer->start(1000. / FPS);
	connect(timer, &QTimer::timeout, this, &MainWindow::update);

	// utworzenie kulki
	this->ball = new throwableBall;
	this->ball->setRect(0, 0, BALL_DIAMETER, BALL_DIAMETER);

	// ustawienie koloru kulki
	this->ball->setBrush(QBrush(idleColor));

	// kulka znajduje się bliżej użytkownika niż linijki
	this->ball->setZValue(1);

	// dodanie podłogi [ stały rozmiar, potem może się zmienić ]
	this->floor = new QGraphicsRectItem(0, BALL_DIAMETER + 1, 10000, 10);
	this->floor->setBrush(QBrush(QColor(0x78, 0x48, 0)));

	// dodanie kulki do sceny
	this->scene->addItem(this->ball);
	this->scene->addItem(this->floor);


	this->view->centerOn(this->ball);

}

/*-----------------------------------------------------------------------*/
void MainWindow::setLeftStaticWidget()
{
	// groupBox
	QGroupBox *leftGroupBox = new QGroupBox("akcje");
	QVBoxLayout *groupBoxLayout = new QVBoxLayout;

	// layout lewego widgetu
	QVBoxLayout *leftLayout = new QVBoxLayout(this);

	// guzik który zresetuje animację
	QPushButton *resetAnimation = new QPushButton("Reset", this);
	resetAnimation->setFixedSize(SINGLEBUTTONWIDTH, SINGLEBUTTONHEIGHT);
	connect(resetAnimation, SIGNAL(clicked(bool)), this, SLOT(resetAnimation()));

	// guzik który otworzy kalkulator
	QPushButton *openCalc = new QPushButton("Otwórz Kalkulator", this);
	openCalc->setFixedSize(SINGLEBUTTONWIDTH, SINGLEBUTTONHEIGHT);
	connect(openCalc, SIGNAL(clicked(bool)), this, SLOT(openCalculator()));

	// guzik który otworzy legendę
	QPushButton *openLegend = new QPushButton("Otwórz legendę", this);
	openLegend->setFixedSize(SINGLEBUTTONWIDTH, SINGLEBUTTONHEIGHT);
	connect(openLegend, SIGNAL(clicked(bool)), this, SLOT(openLegend()));

	// guzik który przybliży
	QHBoxLayout *zoomingLayout = new QHBoxLayout;
	QPushButton *zoomIn = new QPushButton("+");
	zoomIn->setFixedSize(SINGLEBUTTONWIDTH / 2., SINGLEBUTTONHEIGHT);
	connect(zoomIn, &QPushButton::clicked, this, &MainWindow::zoomIn);

	// guzik który oddali
	QPushButton *zoomOut = new QPushButton("-");
	zoomOut->setFixedSize(SINGLEBUTTONWIDTH / 2., SINGLEBUTTONHEIGHT);
	connect(zoomOut, &QPushButton::clicked, this, &MainWindow::zoomOut);

	// layout typu
	// [-------]
	// [ - | + ]
	// [-------]
	zoomingLayout->addWidget(zoomOut);
	zoomingLayout->addWidget(zoomIn);


	groupBoxLayout->addWidget(resetAnimation);
	groupBoxLayout->addWidget(openCalc);
	groupBoxLayout->addWidget(openLegend);
	groupBoxLayout->addLayout(zoomingLayout);

	leftGroupBox->setLayout(groupBoxLayout);

	leftLayout->addWidget(leftGroupBox);

	// ustawienie layoutu
	this->leftWidget->setLayout(leftLayout);
}

/*-----------------------------------------------------------------------*/
void MainWindow::setupInitThrowWidgets()
{
	if (this->rightWidget != NULL)
	{
		delete this->rightWidget;
		this->rightWidget = NULL;
	}

	// usunięcie całej ścieżki
	for (auto *step : road)
		delete step;
	road.clear();

	this->rightWidget = new QWidget(this);

	// lewy layout
	QVBoxLayout *initThrowLeftLayout = new QVBoxLayout;
	// widget z ustawieniem pozycji X
	setPosX = new lineEditSetter("Ustaw pozycję X : ", "[0-9.]{" + HOW_MANY_VALS_IN_EDITABLE + "}", new QString("m"));
	connect(setPosX->lineEdit, &QLineEdit::textEdited, this, &MainWindow::setNewPosX);

	// widget z ustawieniem pozycji Y
	setPosY = new lineEditSetter("Ustaw pozycję Y : ", "[0-9.]{" + HOW_MANY_VALS_IN_EDITABLE + "}", new QString("m"));
	connect(setPosY->lineEdit, &QLineEdit::textEdited, this, &MainWindow::setNewPosY);

	// widget z ustawieniem siły
	setV = new lineEditSetter("Ustaw V : ", "[0-9.]{" + HOW_MANY_VALS_IN_EDITABLE + "}", new QString("m&fracls"));
	connect(setV->lineEdit, &QLineEdit::textEdited, this, &MainWindow::setNewVelocity);

	// widget z ustawieniem  kąta
	setAngle = new lineEditSetter(ANGLE_STR, "[0-9-]{4}", new QString("stopni"));
	connect(setAngle->lineEdit, &QLineEdit::textEdited, this, &MainWindow::setNewAngle);

	// skalowanie : metry na piksele
	setScale = new lineEditSetter("1 m = ", "[0-9.]{6}", new QString("pikseli"));
	setScale->lineEdit->setText(QString::number(DEFAULT_SCALE));
	connect(setScale->lineEdit, &QLineEdit::textEdited, this, &MainWindow::setNewScale);

	// guzik który uruchomi animację
	runButton = new QPushButton("Uruchom");
	connect(runButton, &QPushButton::pressed, this, &MainWindow::runAnimation);

	// dodanie widgetów do layout'u
	initThrowLeftLayout->addWidget(setPosX);
	initThrowLeftLayout->addWidget(setPosY);
	initThrowLeftLayout->addWidget(setV);
	initThrowLeftLayout->addWidget(setAngle);
	initThrowLeftLayout->addWidget(setScale);
	initThrowLeftLayout->addWidget(runButton);
	initThrowLeftLayout->addSpacing(DEFAULTSPACING);

	// ustawienie layoutu
	this->rightWidget->setLayout(initThrowLeftLayout);

	this->mainLayout->addWidget(this->rightWidget);
}

/*-----------------------------------------------------------------------*/
void MainWindow::setupSimWidgets()
{
	this->view->setInteractive(true);

	if (this->setV->lineEdit->text() == "")
		this->velocityInM = 0;

	// obliczenie wartości
	this->velocityX = qCos(this->angle * M_PI / 180.) * this->velocityInM;
	this->velocityY = qSin(this->angle * M_PI / 180.) * this->velocityInM;

	// tworzymy nowy widget [ qt automatycznie posprząta wszystko gdy usuniemy rodzica]
	if (this->rightWidget != NULL)
		delete this->rightWidget;

	this->rightWidget = new QWidget(this);
	QVBoxLayout *rightLayout = new QVBoxLayout();

	// group box dla guzików
	QGroupBox *buttonsGroupBox = new QGroupBox("Guziki");

	// ustawienie stałej wielkości
	buttonsGroupBox->setFixedHeight(100);

	// layout guzików
	QVBoxLayout *buttonsLayout = new QVBoxLayout;

	// guzik który wznowi/pauzuje animację
	this->runOrPause = new QPushButton("Pauza");

	// guzik który pokaże parametry guzików
	showComponentVectorsButton = new QPushButton("Pokaż wektory składowe");

	// poprawiamy tekst jeżeli wcześniej mieliśmy pokazane wektory składowe
	if (showComponentVectors)
		showComponentVectorsButton->setText("schowaj wektory składowe");


	// połączenie sygnałów z odpowiednimi slotami
	connect(runOrPause, &QPushButton::clicked, this, &MainWindow::pauseOrRunAnim);
	connect(showComponentVectorsButton, &QPushButton::clicked, this, &MainWindow::changeShowComponentVect);

	// dodanie widgetów do layoutu
	buttonsLayout->addWidget(runOrPause);
	buttonsLayout->addWidget(showComponentVectorsButton);

	// ustawienie layoutu do groupBox'a
	buttonsGroupBox->setLayout(buttonsLayout);

	// groupbox dzięki któremu może nawet będzie to jakoś wyglądać
	QVBoxLayout *remindersLayout = new QVBoxLayout;
	QGroupBox *reminders = new QGroupBox("siła, kąt i funkcje tryg.");

	// ustawienie stałej wielkości [ magic number ! ]
	reminders->setFixedHeight(100);
	// layout 
	QHBoxLayout *angleAndVelocityLayout = new QHBoxLayout;
	// labele które przypominają wartości dla których jest uruchomiona symulacja
	QLabel      * vLabel = new QLabel(BOLDFACE + QString("v : ") + QString::number(this->velocityInM) + QString("m/s") + END_BOLDFACE);
	QLabel      *angleLabel = new QLabel(BOLDFACE + QString("kąt : ") + QString::number(this->angle) + QString(" stopni") + END_BOLDFACE);
	angleAndVelocityLayout->addWidget(vLabel);
	angleAndVelocityLayout->addWidget(angleLabel);

	// labele które przypomną funkcje trygonometryczne
	QHBoxLayout *trygFuncsLayout = new QHBoxLayout;
	QLabel	    *cosLabel = new QLabel(BOLDFACE + QString("cos : ") + QString::number(qCos(this->angle * M_PI / 180.)) + END_BOLDFACE);
	QLabel      *sinLabel = new QLabel(BOLDFACE + QString("sin : ") + QString::number(qSin(this->angle * M_PI / 180.)) + END_BOLDFACE);
	trygFuncsLayout->addWidget(cosLabel);
	trygFuncsLayout->addWidget(sinLabel);

	// dodanie layoutów
	remindersLayout->addLayout(angleAndVelocityLayout);
	remindersLayout->addLayout(trygFuncsLayout);
	reminders->setLayout(remindersLayout);

	// group box przechowujący wszystkie wyliczone wartości
	QGroupBox *calculatedVals = new QGroupBox("wyliczone wartości");

	QVBoxLayout *calculatedValLayout = new QVBoxLayout;
	// labele które zawierają wyliczone zmienne [ zasięg, mask. wysokość itp.]
	this->range = throwMath::range(this->velocityInM, this->angle, -startingPos.y() / mToPxls) + startingPos.x() / mToPxls;
	this->maxHeigth = throwMath::maxHeigth(this->velocityInM* qSin(this->angle * M_PI / 180.), -startingPos.y() / mToPxls);


	QLabel *rangeLabel = new QLabel(BOLDFACE + RANGE_STR + QString::number(this->range) + QString(" m") + END_BOLDFACE);

	// obliczenie i ustawienie maksymalnej wysokości
	QLabel *maxHeigthLabel = new QLabel(BOLDFACE + MAX_HEIGTH_STR + QString::number(this->maxHeigth) + QString(" m") + END_BOLDFACE);

	// obliczenie czasu do osiągnięcia maksymalnej wysokości
	QLabel *timeToReachMaxHeightLabel = new QLabel(BOLDFACE + TIME_TO_REACH_MAXHEIGTH_STR +
		QString::number(throwMath::timeToReachMaxHeigth(this->velocityInM * qSin(this->angle * M_PI / 180.), -startingPos.y() / mToPxls))
		+ QString(" s") + END_BOLDFACE);

	// obliczenie czasu rzutu
	QLabel *timeOFThrow = new QLabel(BOLDFACE + TIME_OF_THROW_STR +
		QString::number(throwMath::timeOfThrow(this->velocityInM * qSin(this->angle * M_PI / 180.), -startingPos.y() / mToPxls))
		+ QString("s") + END_BOLDFACE);

	// ustawienie labela który wyświetla czas
	this->timeLabel = new QLabel(BOLDFACE + TIME_LABEL_STR + QString(" 0") + END_BOLDFACE);

	// obliczamy do rzutu ukośnego wtedy gdy rzucamy z wysokości 0 [ można przerobić liczenie dla wszystkich wartości
	//																 dzieląc rzut na 2 etapy, ale potrzebujemy symulacji tylko 
	//																 najprostszych rzutów]

	if (this->startingPos.y() == 0)
		this->isMotionProjection = true;

	// wyświetlenie równiania [ LaTeX rulez !]  
	QLabel *equationLabel = new QLabel;

	if (this->isMotionProjection)
		equationLabel->setPixmap(QPixmap("images/projMotion.png"));
	else
		equationLabel->setPixmap(QPixmap("images/horThrow.png"));

	// label który wyświetli rozwiązanie powyższego równania
	this->throwEquationLabel = new QLabel;
	this->throwEquationLabel->setFixedWidth(300);

	// label który pokaże aktualną pozycję 
	this->posLabel = new QLabel(POS_X_STR + "0        " + POS_Y_STR + "0");

	// label który pokaże aktualne wartości Vx[ stały]  i Vy 
	this->velocitiesLabel = new QLabel(VELOCITY_X_STR + QString::number(this->velocityX) + "        " + VELOCITY_Y_STR + QString::number(this->velocityY));

	//dodanie widgetów do layoutu
	calculatedValLayout->addWidget(this->posLabel);
	calculatedValLayout->addWidget(this->velocitiesLabel);
	calculatedValLayout->addWidget(rangeLabel);
	calculatedValLayout->addWidget(maxHeigthLabel);
	calculatedValLayout->addWidget(timeToReachMaxHeightLabel);
	calculatedValLayout->addWidget(timeOFThrow);
	calculatedValLayout->addWidget(this->timeLabel);
	calculatedValLayout->addWidget(equationLabel);
	calculatedValLayout->addWidget(this->throwEquationLabel);

	// ustawienie layoutu group box'a
	calculatedVals->setLayout(calculatedValLayout);

	// dodanie guzików do layoutu
	rightLayout->addWidget(buttonsGroupBox);
	rightLayout->addWidget(reminders);
	rightLayout->addWidget(calculatedVals);
	//rightLayout->addSpacing(this->sizeHint().height() * 0.4);

	// ustawienie layoutu dla prawego widgetu
	this->rightWidget->setLayout(rightLayout);

	// dodanie prawego widgetu
	this->mainLayout->addWidget(this->rightWidget);

	// restart zegara 
	this->addStepToRoadClock.restart();

	// dodanie linii
	this->AddScalingLines();


}

/*-----------------------------------------------------------------------*/
void MainWindow::setupInspectWidgets()
{
	inspectGroupBox = new QGroupBox("opcje wyświetlania");

	QVBoxLayout *rightLayout = new QVBoxLayout;
	// guzik który pokaże wektory na scenie
	showVectorsButton = new QPushButton(SHOW_ROAD_VECTORS_STR);
	connect(showVectorsButton, &QPushButton::clicked, this, &MainWindow::showVectors);

	// guzik który pokaże całą drogę
	showWholeRoadButton = new QPushButton("schowaj całą drogę");
	connect(showWholeRoadButton, &QPushButton::clicked, this, &MainWindow::showOrHideRoad);

	// guzik który pokaże wszystkie zaznaczone punkty
	showAllPointsButton = new QPushButton(SHOW_ALL_POINTS_STR);
	connect(showAllPointsButton, &QPushButton::clicked, this, &MainWindow::selectAllPoints);

	// guzik który zmieni czy chcemy pokazać pojedyńcze wektory czy wszystkie wektory
	this->showSingleVecButton = new QPushButton(SHOW_SINGLE_VECTOR_STR);
	connect(showSingleVecButton, &QPushButton::clicked, this, &MainWindow::changeShowingVectorState);

	// następny i poprzedni punkty
	QHBoxLayout *manipulatePoints = new QHBoxLayout;

	// jeżeli okieno jest ukryte to wyświetlamy je z powrotem
	if (inspectValuesDialog->isHidden())
		inspectValuesDialog->show();

	// ustawienie nowych wartości początkowych
	this->inspectValuesDialog->setNewBegginingValues(this->velocityInM, this->angle);

	// inicjalizacja guzików dzięki którymi będzie można zarządzać widokiem
	QPushButton *nextPoint = new QPushButton("następny");
	connect(nextPoint, &QPushButton::clicked, this->inspectValuesDialog, &inspectDialog::nextValue);

	QPushButton *previousPoint = new QPushButton("poprzedni");
	connect(previousPoint, &QPushButton::clicked, this->inspectValuesDialog, &inspectDialog::prevValue);

	// dodanie do layoutu
	manipulatePoints->addWidget(previousPoint);
	manipulatePoints->addWidget(nextPoint);


	// posklejanie widgetów i layoutów w jeden
	rightLayout->addWidget(showVectorsButton);
	rightLayout->addWidget(showSingleVecButton);
	rightLayout->addWidget(showWholeRoadButton);
	rightLayout->addWidget(showAllPointsButton);
	rightLayout->addLayout(manipulatePoints);
	rightLayout->addSpacing(DEFAULTSPACING);

	inspectGroupBox->setLayout(rightLayout);

	this->rightWidget->layout()->addWidget(inspectGroupBox);
}

/*-----------------------------------------------------------------------*/
void MainWindow::AddScalingLines()
{
	// wektor struktur który zawiera wszystkie ważnie wartości dla linii
	QVector<scalingLineStruct> results;
	results.push_back(scalingLineStruct(this->range));
	results.push_back(scalingLineStruct(this->maxHeigth));

	// iterujemy przez wszystkie struktury
	for (auto& linesStruct : results)
	{
		// do której potęgi podnosimy 10  [ nasze x ]
		double multipler = qPow(10, qFloor(log10(linesStruct.range + BALL_DIAMETER / mToPxls)) - 1);
		bool choosedLineScale = false;

		while (!choosedLineScale)
		{
			// iterujemy przez wszystkie wartości ktore zdefiniowaliśmy jako "ładne liczby" [ 1 * 10**x, 2 * 10**x , 5 * 10**x]
			for (auto choice : NICE_LOOKING_MULTIPLERS)
			{
				// obliczamy ile linijek wyjdzie przy aktualnym mnożniku
				double howManyLines = qCeil((linesStruct.range + BALL_DIAMETER / mToPxls) / double(choice * multipler));

				// jeżeli jest mniejsze od ustalonej wartości to spełnia nasze wartości
				if (howManyLines < MAXLINES)
				{
					// ustawienie wartości
					linesStruct.howManyLines = howManyLines;
					linesStruct.scale = multipler * choice;
					choosedLineScale = true;
					break;
				}

			}
			// x += 1
			multipler *= 10;
		}
	}

	// tworzymy linie tylko wtedy gdy zasięg jest większy niż 50 pikseli 

	if (this->range * mToPxls > MIN_WIDTH_PXLS)
	{
		// TODO : można to przerobić na funkcję żeby nie powtarzać kodu	
		for (int i = 0; i < results[0].howManyLines + 1; ++i)
		{
			// dodanie linijki w pozycji X wyliczonej kilka linijek wyżej
			QGraphicsLineItem * newLine = new QGraphicsLineItem(results[0].scale * i * mToPxls + BALL_DIAMETER / 2., BALL_DIAMETER / 2.,
				results[0].scale * i * mToPxls + BALL_DIAMETER / 2., BALL_DIAMETER / 2. - (results[1].scale * results[1].howManyLines * mToPxls));
			this->scene->addItem(newLine);

			// TODO : raczej kiepskie rozwiązanie problemu
			// inicjalizujemy text by za chwilę sprawdzić czy nie nie będzie kolidować z poprzednim [ jeżeli będzie to usuwamy go ] 
			QGraphicsTextItem * scaleText = new QGraphicsTextItem(QString::number(i * results[0].scale - this->startingPos.x() / mToPxls));
			scaleText->setPos(results[0].scale * i * mToPxls - scaleText->boundingRect().width() / 2. + BALL_DIAMETER / 2., BALL_DIAMETER / 2. - scaleText->boundingRect().height() / 2.);
			scaleText->setZValue(2);

			if (i != 0 && this->horizontalLines[i - 1].second != NULL && scaleText->collidesWithItem(this->horizontalLines[i - 1].second))
			{
				delete scaleText;
				scaleText = NULL;
			}
			else
			{
				// ustawienie domyślnego koloru
				scaleText->setDefaultTextColor(GRAPHICS_TEXT_COLOR);
				// dodajemy tekst do sceny
				this->scene->addItem(scaleText);
			}


			// dodajemy parę do wektora
			this->horizontalLines.push_back(lineAndTextPairPtr(newLine, scaleText));
		}
	}

	// tworzymy pionowe linie tylko wtedy gdy maksymalna wysokość jest większa niż 20 pikseli
	if (this->maxHeigth * mToPxls > MIN_HEIGTH_PXLS)
	{
		if (this->maxHeigth * mToPxls)
			for (int i = 0; i < results[1].howManyLines + 1; ++i)
			{
				// dodanie linijki w pozycji Y wyliczonej kilka linijek wyżej
				QGraphicsLineItem *newLine = new QGraphicsLineItem(-BALL_DIAMETER, BALL_DIAMETER / 2. - results[1].scale * i * mToPxls,
					BALL_DIAMETER + results[0].howManyLines * results[0].scale * mToPxls, BALL_DIAMETER / 2. - results[1].scale * i * mToPxls);
				this->scene->addItem(newLine);

				// dodajemy tekst zawierający informację odnośnie skali
				QGraphicsTextItem *scaleText = new QGraphicsTextItem(QString::number(i * results[1].scale));
				scaleText->setPos(-BALL_DIAMETER / 2. - scaleText->boundingRect().width() / 2., -BALL_DIAMETER / 2. - results[1].scale * i * mToPxls + scaleText->boundingRect().height() / 2.);

				// 
				if (i != 0 && this->verticalLines[i - 1].second != NULL && scaleText->collidesWithItem(this->verticalLines[i - 1].second))
				{
					delete scaleText;
					scaleText = NULL;
				}
				else
				{
					// ustawienie domyślnego koloru 
					scaleText->setDefaultTextColor(GRAPHICS_TEXT_COLOR);
					// dodanie tekstu do sceny
					this->scene->addItem(scaleText);
				}

				// dodajemy parę do wektora
				this->verticalLines.push_back(lineAndTextPairPtr(newLine, scaleText));
			}
	}
}

/*-----------------------------------------------------------------------*/
void MainWindow::deleteScalingLines()
{
	for (auto& line : this->verticalLines)
	{
		if (line.first != NULL)
			delete line.first;

		if (line.second != NULL)
			delete line.second;
	}
	verticalLines.clear();

	for (auto& line : this->horizontalLines)
	{
		if (line.first != NULL)
			delete line.first;

		if (line.second != NULL)
			delete line.second;
	}
	horizontalLines.clear();
}

/*-----------------------------------------------------------------------*/
#endif 

/*-----------------------------------------------------------------------*/
QSize MainWindow::sizeHint() const
{
	return QSize(1024, 800);
}

/*-----------------------------------------------------------------------*/
