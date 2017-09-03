#pragma once
#include "entity.hpp"
#include "windowsApplication.h"
#include <SFML\Graphics.hpp>
#include <map>
#include <string>
#include <fstream>
#include "resource.h"

class SFMLApplication : public windowsApplication
{
public:

	// metoda która uruchamia animacjê 
	void run();
	// konstruktor który zbuduje magiczne aplikacjê winAPI
	SFMLApplication(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lCmdLine, int cmdShow);
private:
//////////////////////////////////////////////////////////////////////////////////////////
//			metody oraz obiekty ogólne

	

	virtual void stateMachine() override;
	virtual void handleExceptions(exceptionWithMsg exception) override;
	void		 initializeWinAPIThings();
	LPSTR		 getTextFromTextBox(HWND hwnd);
	void		 switchState(statesOfMachine State);
	void		 resetAll();
	void		 InitializeSFML();
	void         updateLinesToCircle();
	void		 handleRequestsFromButtons();
	bool		 mouseIntersectWithGivenCircle(sf::CircleShape circle);



	double V, Vx, Vy, angle, sinA, cosA, G_const, startingXPos, startingYPos;
	sf::RenderWindow	window, windowWithTexts;
	sf::View			GUI;
	statesOfMachine		stateOfMachine;
	sf::Font			font;
	sf::Text			text;
	sf::Color			highlighted, defaultColor, maxHeightColor, highlightedPointColor;
	sf::View			staticView, DynamicView;
	sf::VertexArray		linesToCenterOfObject;
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//			metody, obiekty i zmienne stanu INITIALIZETHROW
	void InitializeThrow();
	void handleStateInitializeThrow();
	void updateInitializationThrow();
	void handleDrawingAndUpdatingTriangle();
	void manageInitializeThrowTexts();
	void drawInitializeThrowTexts();
	void setColors(int indexOfText, int indexOfSign, std::string text, int indexOfLine, sf::Color color, sf::Color outlineColor);


	bool ClickedOnBall, moveBall, isPaused;
	Ball object;
	sf::VertexArray triangle;
	std::vector<sf::Text> InitializeThrowTexts;
	std::vector<sf::RectangleShape> gui;
	sf::Color VxColor, VyColor, VColor, VxColorOutline, VyColorOutline, VColorOutline, selectedPointsColor;


	// WinAPI :>
	HWND intializeThrowLeftTable, intializeThrowRightTable;
	HWND l_SetPositionX, l_SetPositionY,b_setPositionX, b_setPositionY, tb_setPosX, tb_setPosY;
	HWND l_setV, l_setAngle, b_setV, b_setAngle, tb_setV, tb_setAngle, b_runAnimation;
	HWND l_setScale,l_setScale2, l_setScale3, tb_setScale, b_setScale;
	HWND l_showLegend, b_showLegend;
	HWND l_showLegend_i, b_showLegend_i;
	HWND l_getParamsByTime, b_getParamsByTime;

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//			metody, obiekty i zmienne stanu ANIMATETHROW
	void handleAnimateThrowTexts();
	void initializeAnimateThrow();
	void handleStateAnimateThrow();
	void manageRoad();
	void pauseAnimation();
	void updateAnimateThrow();
	void calculateVariables();
	void displayAnimateThrow();
	void manageView();
	void handleSelectingPoints();
	void manageZoom();
	void drawAnimateThrowTexts();
	void handleTimeManagmentWhenPaused();
	void manageMovingOfProjection();
	double calculateHeight(double timeElapsed, double scale,bool inPixels);
	double calcluateWidth(double timeElapsed, double scale, bool inPixels);
	void printAllValuesIntoFile();


	sf::VertexArray vectors;
	typesOfThrow typeOfThrow;
	sf::Color				colorOfStep;
	circleWithParameters	step;
	sf::Clock				timeElapsed, delayBtwAddingSteps, delayBtwChangingPauseBool;
	std::vector<sf::Text>	animateThrowTexts;
	std::vector<sf::Text>	CrosswiseEquation;
	std::vector < sf::RectangleShape >			animateThrowGUI;
	std::vector<circleWithParameters>			road;
	std::vector<showedCircleWithParameters>     selectedPoints;
	showedCircleWithParameters					selectedCircle;

	std::vector <sf::VertexArray>				squareBoardLinesX, squareBoardLinesY;
	std::vector<sf::Text>						squareBoardTextsX, squareBoardTextsY;
	sf::VertexArray								line;
	double										linesStepX, linesRangeX, linesStepY, linesRangeY;


	double	timeToReachMaxHeight, timeToReachStartingPos, range, maxHeight, xViewMove, yViewMove,
		height, width, timeElapsedFromLastPause, zoomedBy, prevXChange, prevYChange, scale, 
		xChangeByZooming,yChangeByZooming;
	
	const float zoomBy, moveScreenBy;
	bool addedMaxHeight, animationPaused, showSelectedPoints, showLines;
	int indexOfMaxHeight, indexOfCurrentlySelectedPoint, howManySelectedPoints;
	sf::Vector2f	prevPosition;

	HWND	animateThrowLeftTable, animateThrowRightTable;
	HWND	showSelectedPointsLeftTable;
	HWND	b_pauseAnimation, b_resetAnimation, b_showSelectedPoints;
	HWND	l_pauseAnimation, l_resetAnimation, l_showSelectedPoints;
	HWND legendBox;
	bool isLegendBoxShowed;


	//////////////////////////////////////
	//			SHOWSELECTEDPOINTS
	/////////////////////////////////////
	void handleShowSelectedPoints();
	void showPointByPoint();
	void managePointPosition();
	void setupShowSelectedPointsTexts();
	void manageDeletingPoints();
	void drawPoints();
	void drawLines(bool needToDrawObj);

	std::vector<sf::Text> showSelectedPointsVectorTexts,showSelectedPointsParams;
	bool showVectors, showRoad, showAllPoints, selectedAllPoints;
	int indexOfCurrentlyInspectedPoint;
	sf::Clock delayBtwChangingPoints;

	// mapa haszowana wyj¹tków
	std::map<exceptionList, std::string> ExceptionHashArray;

	HWND l_showVectors, b_showVectors;
	HWND l_showRoad        , b_showRoad;
	HWND l_showAllPoints   , b_showAllPoints;
	HWND l_manipulateShowedPoints, b_nextPoint, b_previousPoint;
	HWND l_selectAllPoints		 , b_selectAllPoints;

	double scaledRange, scaledPosX, scaledPosY, scaledHmax, scaledTimeToReachMaxHeight, scaledTimeToReachStartingPos;
	double valOfEquation;
//////////////////////////////////////////////////////////////////////////////////////////


	std::fstream fileWithValues;

};


