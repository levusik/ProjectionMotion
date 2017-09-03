#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QTime>
#include <qvector.h>
#include <qlist.h>
#include "entities.h"
#include "ball.h"


class QGraphicsEllipseItem;
class QGraphicsLineItem;
class QGraphicsPathItem;
class QGraphicsScene;
class QGraphicsView;
class inspectDialog;
class QPainterPath;
class QMouseEvent;
class QHBoxLayout;
class QPushButton;
class QGroupBox;
class QLabel;

class lineEditSetter;
class step;

using namespace constants;


class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


    QSize sizeHint() const override;

	 void updateVectors();

public slots:
    
	/*Sloty wykorzystywane przez lewy widget*/
	// slot który otworzy kalkulator
    void openCalculator();
 
	// slot który zresetuje animację
	void resetAnimation();

	// slot który otworzy legendę ( który objaśnia co i jak)
    void openLegend();

	// sloty które przybliżą lub oddalą
    void zoomIn();
    void zoomOut();
	/*****************************************/
	/*Sloty wykorzystywane przez prawy widget podczas symluacji*/
	void pauseOrRunAnim();

	void changeToPauseState(bool addHoardedTime = true);
	void changeShowComponentVect();

	/*****************************************/
    /* sloty wykorzystywane przez prawy widget podczas inicjalizacji */
    void setNewPosY(const QString &newText);
    void setNewPosX(const QString &newText);
    void setNewVelocity(const QString &newText);
    void setNewAngle(const QString &newText);
    void setNewScale(const QString &newText);
	// uruchomienie rzutu
    void runAnimation();
	
	/*****************************************/
	/* sloty wykorzystywane przez prawy widget podczas inspect state */
	 void showVectors();
	 void showOrHideRoad();
	 void selectAllPoints();
	 void changeShowingVectorState();
	 void displayVectorsOfItem(step * item);

	/*****************************************/
    // update sceny graficznej
    void update();

protected:
    virtual void mousePressEvent(QMouseEvent *event);

private:
    // główny layout
    QHBoxLayout *mainLayout = nullptr;

    // stan FSM
    simulationState currentState = simulationState::INTIALIZE;

    // metody aktualizujące scenę w zależności od stanu
	void showVectorsFromBallToPoint(double xPos, double yPos, bool addBallPosToGivenPos = true);
	void manageAddingStepToRoad(double yPos, double xPos);
    void manageMouseEventsInitState(QMouseEvent *event);
    void manageMouseEventsSimState(QMouseEvent *event);
	void setupLinesByVAndAngle();
	void updateThrowEquation(double xPos, double yPos);
    void updateInitializeState();
    void updateSimulateState();
	void updateDynamicTexts(double xPos, double yPos);
	void unselectItem(step * castedItem);
	void selectStep(step * castedItem);
	void deleteScalingLines();
	void AddScalingLines();
    void deleteLines();
    void moveLines();

    // setup różnych widgetów
	void initializeSceneAndView();
    void setupInitThrowWidgets();
    void setupInspectWidgets();
    void setLeftStaticWidget();
    void setupSimWidgets();

    // lewy i prawy pasek które zawierają różne akcje
    QWidget *leftWidget  = nullptr;
    QWidget *rightWidget = nullptr;

    // wskaźniki do setterów za pomocą których można będzie coś ustawić
   lineEditSetter *setPosX      = nullptr;
   lineEditSetter *setPosY      = nullptr;
   lineEditSetter *setV         = nullptr;
   lineEditSetter *setAngle     = nullptr;
   lineEditSetter *setScale     = nullptr;
   
   // wskaźniki do guzików
   QPushButton *runButton				   = nullptr;
   QPushButton *runOrPause				   = nullptr;
   QPushButton *showComponentVectorsButton = nullptr;
   QPushButton *showVectorsButton   	   = nullptr;
   QPushButton *showWholeRoadButton	       = nullptr;
   QPushButton *showAllPointsButton		   = nullptr;
   QPushButton *showSingleVecButton		   = nullptr;


   // wskaźnik  do label'a który wyświetla czas
   QLabel *velocitiesLabel		= nullptr;
   QLabel *posLabel				= nullptr;
   QLabel *timeLabel			= nullptr;
   QLabel *throwEquationLabel	= nullptr;


   // group box z opcjami do pokazania wartości
   QGroupBox *inspectGroupBox		= nullptr;
   
	// wskaźniki do dialogów
   inspectDialog *inspectValuesDialog = nullptr;

   // boolean który jeżeli ustawiony na true, pokaże wektory składowe
   bool updateLines = false;
   bool isFirstStep = true;

   // pozycja kursora wedle sceny
   QPointF scenePoint = QPointF(0,0);

   // parametry do obliczeń
   QPointF startingPos = QPointF(0,0);
   QPointF prevStartingPos = QPointF(0,0);
   double  velocityInM = 0., velocityX = 0., velocityY = 0.;
   double  mToPxls = DEFAULT_SCALE;
   double  prevVx = 0, prevVy;
   double  angle = 0.;
   double range					= 0.;
   double maxHeigth				= 0.;

   // zmienne i zegar potrzebny do kontroli czasu
   QTime clock;
   double prevTime				= 0.;
   int prevFrame				= 0;

  
   bool	  isAnimationPaused		= false;
   bool   hasMarkedMaxHeigth	= false;
   bool   animationEnded		= false;
   bool   showComponentVectors	= false;
   bool   selectingStepLock		= false;
   bool   selectedAllPoints		= false;
   bool	  isMotionProjection	= false;
   bool	  showRoadVector		= false;
   bool	  isRoadVisible			= true;
   bool	  showSingleVector		= true;

   // magiczna kulka która będzie się poruszać
   throwableBall *ball;
   QColor idleColor = QColor(0xff, 0, 0);
   QColor markedColor = QColor(0, 0xff, 0);

   typedef QPair<QGraphicsLineItem *, QGraphicsTextItem*> lineAndTextPairPtr;
   // wektory które zawierają linie które wyświetlają skalowanie 
   QVector <lineAndTextPairPtr> horizontalLines;
   QVector <lineAndTextPairPtr> verticalLines;

   // wizulizacja (?) wektora V
   QGraphicsLineItem *velocityVec = nullptr;
   // wizualizacja wektora Vx
   QGraphicsLineItem *velocityVecVX = nullptr;
   // wizualizacja wektora Vy
   QGraphicsLineItem *velocityVecVY = nullptr;

   // deklaracja  widoku i  sceny
   QGraphicsScene *scene = nullptr;
   QGraphicsView  *view = nullptr;

   // podłoga
   QGraphicsRectItem *floor = nullptr;
   // wektor z "krokami" [ te mniejsze kulki które zostawia większa kulka, zawierające informacje
   //					   odnośnie czasu, pozycji ]
   QVector	   <step*>    road;
   QVector<step*>	      selectedSteps;
   QTime			  addStepToRoadClock;
   int				  stepCount = 0;
	
};

#endif
