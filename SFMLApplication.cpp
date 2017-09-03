#include "SFMLApplication.h"
#include <Windows.h>

#define GETSEC(time) (time.getElapsedTime().asSeconds())

////////////////////////////////////////////////////////////////
//					winAPI (globalne)
# define SETPOSX						0x500
# define SETPOSY						0x501
# define SETV							0x502
# define SETANGLE						0x503
# define RUNANIMATION					0x504
# define SETSCALE						0x505

# define RESETANIMATION					0x506
# define PAUSEANIMATION					0x507
# define SHOWSELECTEDPOINTS				0x508

# define SELECTALLPOINTS				0x509
# define SHOWVECTORS					0x50A
# define SHOWROAD						0x50B
# define SHOWALLPOINTS					0x50C
# define NEXTPOINT						0x50D
# define PREVIOUSPOINT					0x50F

# define SHOWLEGEND						0x510
# define GETPARAMSBYTIME				0x511
#define  LEGENDCLOSED					0x512



int	SIGNALFROMOCONTROLS = 0;
////////////////////////////////////////////////////////////////


INT_PTR CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//				metody oraz funkcje ogólne

void SFMLApplication::run()
{
	MSG message;
	message.message = ~WM_QUIT;
	while (message.message != WM_QUIT)
	{
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			// if a message was waiting in the message queue, process it 
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else
		{
			stateMachine();
			handleRequestsFromButtons();
		}
	}

}
SFMLApplication::SFMLApplication(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lCmdLine, int cmdShow)
	: object(CIRCLERADIUS, sf::Vector2f(0, SFMLWINDOWHEIGHT - 2 * CIRCLERADIUS), sf::Color::Color(0xff, 0x0, 0x0)),
	triangle(sf::Lines, 6), linesToCenterOfObject(sf::Lines, 4), zoomBy(1.01f), moveScreenBy(5.f), line(sf::Lines,2)
{
	// wywo³ujemy metodê która pobawi siê z WinAPI za nas ) 
	Initialize(hInstance, hPrevInstance, lCmdLine, cmdShow);

	ShowWindow(hWnd, cmdShow);
	UpdateWindow(hWnd);

	// wywo³ujemy metodê która zainicjalizuje wszystkie rzeczy potrzebne do SFML'a
	InitializeSFML();

	highlighted = sf::Color::Color(0x0, 0xff, 0x0);
	defaultColor = sf::Color::Color(0xff, 0x0, 0x0);
	maxHeightColor = sf::Color::Color(0xff, 0xff, 0x0);

	srand(time(NULL));
	staticView = window.getView();
	DynamicView = staticView;

	// wywo³ujemy metodê która zainicjalizuje wszystke rzeczy od winAPI 
	initializeWinAPIThings();

	scale = 1;
	showLines = true;
	switchState(statesOfMachine::INITIALIZETHROW);

}
void SFMLApplication::handleRequestsFromButtons()
{
	DWORD Value;
	LPSTR Buffor;

	try
	{
		switch (SIGNALFROMOCONTROLS)
		{
		case SETPOSX:
			Buffor = getTextFromTextBox(tb_setPosX);
			Value = atoi(Buffor);
			if (Value < 0 || Value > SFMLWINDOWWIDTH - 2 * object.getCircle().getRadius())
			{
				throw exceptionWithMsg("Wyst¹pi³ b³¹d podczas podawania Pozycji X ! (Wartoœc za du¿a lub za ma³a)",
					exceptionList::INCORRECT_INPUT);
			}
			else
			{
				object.moveObject(sf::Vector2f(Value*scale, object.getPosition().y));
				startingXPos = Value ;
			}

			break;

		case SETPOSY:
			
			Buffor = getTextFromTextBox(tb_setPosY);
			Value = atoi(Buffor);
			if (Value < 0 || Value >= 450)
			{
				throw exceptionWithMsg("Wyst¹pi³ b³¹d podczas podawania Pozycji Y ! (Wartoœc za du¿a lub za ma³a)",
					exceptionList::INCORRECT_INPUT);
			}
			else
			{
				object.moveObject(sf::Vector2f(object.getPosition().x, SFMLWINDOWHEIGHT - Value*scale - 2 * object.getCircle().getRadius()));
				startingYPos = Value ;
			}

			break;

		case SETV:
			Buffor = getTextFromTextBox(tb_setV);
			Value = atoi(Buffor);

			Value *= scale;
			if (Value < 0)
			{
				throw exceptionWithMsg("Wyst¹pi³ b³¹d podczas podawania Si³y rzutu ! (wartoœæ za ma³a)",
					exceptionList::INCORRECT_INPUT);
			}
			this->V = Value;
			break;

		case SETANGLE:
			Buffor = getTextFromTextBox(tb_setAngle);
			Value = atoi(Buffor);
			Value = Value % 360;

			this->angle = Value;
			this->sinA = sin(Value * M_PI / 180);
			this->cosA = cos(Value * M_PI / 180);

			break;

		case RUNANIMATION:
			Buffor = getTextFromTextBox(tb_setScale);
			if (Buffor != NULL)
			{
				Value = atoi(Buffor);
				this->scale = Value;

			}
			Buffor = getTextFromTextBox(tb_setAngle);
			if (Buffor != NULL)
			{
				Value = atoi(Buffor);
				Value = Value % 360;
				this->angle = Value;
				this->sinA = sin(Value * M_PI / 180);
				this->cosA = cos(Value * M_PI / 180);
			}
			Buffor = getTextFromTextBox(tb_setV);
			if (Buffor != NULL)
			{
				Value = atoi(Buffor);
				Value *= scale;
				this->V = Value;
			}


			if (V >= 0)
			{
				object.setColorOfCircle(highlighted);
				switchState(statesOfMachine::ANIMATETHROW);
			}
			else
				throw exceptionWithMsg("Nie podano si³y !", exceptionList::ERROR_DURING_STARTING_RUNNING_ANIMATION);

			break;

		case SETSCALE:
			Buffor = getTextFromTextBox(tb_setScale);
			Value = atoi(Buffor);
			scale = Value;
			break;

		case RESETANIMATION:
			resetAll();
			switchState(statesOfMachine::INITIALIZETHROW);

			break;

		case PAUSEANIMATION:
			handleTimeManagmentWhenPaused();

			if (animationPaused)
				SetWindowText(b_pauseAnimation, "Wznów");
			else
				SetWindowText(b_pauseAnimation, "Zatrzymaj");

			break;
		case SHOWSELECTEDPOINTS:

			if (!animationPaused)
				handleTimeManagmentWhenPaused();


			showSelectedPoints = !showSelectedPoints;

			if (!showSelectedPoints)
			{
				ShowWindow(animateThrowLeftTable, true);
				ShowWindow(showSelectedPointsLeftTable, false);

				SetWindowText(l_showSelectedPoints, "Poka¿ wybrane Punkty");
				SetWindowText(b_showSelectedPoints, "Poka¿");
			}
			else
			{
				indexOfCurrentlyInspectedPoint = 0;
				setupShowSelectedPointsTexts();
				ShowWindow(animateThrowLeftTable, false);
				ShowWindow(showSelectedPointsLeftTable, true);
				SetWindowText(l_showSelectedPoints, "Powrót do animacji");
				SetWindowText(b_showSelectedPoints, "Powrót");
			}
			break;
		case SHOWVECTORS:

			showVectors = !showVectors;
			break;
		case SHOWROAD:

			showRoad = !showRoad;
			if (showRoad)
			{
				SetWindowText(b_showRoad, "Schowaj");
			}
			else
			{
				SetWindowText(b_showRoad, "Poka¿");
			}
			break;
		case SHOWALLPOINTS:
			showAllPoints = !showAllPoints;
			if (showAllPoints)
			{
				SetWindowText(b_showAllPoints, "Schowaj");
			}
			else {
				SetWindowText(b_showAllPoints, "Poka¿");
			}
			break;
		case NEXTPOINT:
			if (indexOfCurrentlyInspectedPoint < selectedPoints.size() - 1)
			{
				indexOfCurrentlyInspectedPoint++;
				setupShowSelectedPointsTexts();
				delayBtwChangingPoints.restart();
			}

			break;

		case PREVIOUSPOINT:
			if (indexOfCurrentlyInspectedPoint > 0)
			{
				indexOfCurrentlyInspectedPoint--;
				setupShowSelectedPointsTexts();
				delayBtwChangingPoints.restart();
			}
			break;
		case SELECTALLPOINTS:

			selectedAllPoints = !selectedAllPoints;

			if (selectedAllPoints)
			{
				selectedPoints.clear();

				std::vector<circleWithParameters>::iterator iter;
				int i = 0;
				for (iter = road.begin(); iter != road.end(); ++iter)
				{

					// tworzymy nasz specjany punkt który bêdzie zawiera³ jeszcze kilka przydatnych informacji
					selectedCircle.create(road[i], iter, i, highlightedPointColor, this->scale);

					// dodajmy punkt do vektora, usuwamy z wektora punkt ( chocia¿ bardziej jest odpowiedne ¿e przenieœliœmy
					// punkt do innego wektora)
					selectedPoints.push_back(selectedCircle);
					road[i].setFillColor(selectedPointsColor);

					// dodajemy 1 do iloœci zaznaczonych punktów, update'ujemy tekst oraz odczekujemy chwilê by dodaæ tylko jeden punkt
					howManySelectedPoints++;
					++i;
				}
				SetWindowText(b_selectAllPoints, "Odznacz");
			}
			else
			{
				selectedPoints.clear();
				for (int i = 0; i < road.size(); ++i)
					road[i].setFillColor(defaultColor);

				SetWindowText(b_selectAllPoints, "Zaznacz");
			}
			animateThrowTexts[7].setString("Zaznaczono " + std::to_string(howManySelectedPoints) + "pkt.");
			break;

		case SHOWLEGEND:
			if (this->isLegendBoxShowed)
			{
				ShowWindow(legendBox, SW_HIDE);
			}
			else
			{
				ShowWindow(legendBox, SW_SHOW);
			}
			isLegendBoxShowed = !isLegendBoxShowed;
			break;

		case LEGENDCLOSED:
			this->isLegendBoxShowed = false;
			ShowWindow(legendBox, SW_HIDE);
			break;

		}
		SIGNALFROMOCONTROLS = 0;
	}
	catch (exceptionWithMsg exception)
	{
		handleExceptions(exception);
	}
}
void SFMLApplication::stateMachine()
{
	switch (stateOfMachine)
	{
		case statesOfMachine::INITIALIZETHROW:
			handleStateInitializeThrow();
			break;
		case statesOfMachine::ANIMATETHROW:
			handleStateAnimateThrow();
			break;
	}
}
void SFMLApplication::switchState(statesOfMachine State)
{
	stateOfMachine = State;

	switch (State)
	{
	case statesOfMachine::INITIALIZETHROW:
		InitializeThrow();
		break;
	case statesOfMachine::ANIMATETHROW:
		initializeAnimateThrow();
		break;
	default:
		break;
	}
}
void SFMLApplication::resetAll()
{
	InitializeThrowTexts.clear();
	animateThrowGUI.clear();
	animateThrowTexts.clear();
	gui.clear();
	selectedPoints.clear();

	// reset absolutnie wszystkiego
	V = Vx = Vy = angle = sinA = cosA = startingXPos = startingYPos = ClickedOnBall = moveBall = isPaused
		= timeToReachMaxHeight = timeToReachStartingPos = range = maxHeight = xViewMove = yViewMove = height = width
		= timeElapsedFromLastPause = zoomedBy = prevXChange = prevYChange = addedMaxHeight = animationPaused =
		indexOfMaxHeight = indexOfCurrentlySelectedPoint = howManySelectedPoints = xChangeByZooming = yChangeByZooming =
		showAllPoints = selectedAllPoints = 0;


	window.setView(staticView);
	DynamicView = staticView;
	xViewMove = yViewMove = showVectors = 0;
	road.clear();
}
void SFMLApplication::handleExceptions(exceptionWithMsg exception)
{
	MessageBox(NULL, exception.lMsg, "Ups...", NULL);
	std::fstream file("debug.txt", std::ios::app | std::ios::out);

	file << "Wyrzucono wyj¹tek ! \n";
	file << "Kod B³êdu : " << ExceptionHashArray[exception.Val] << "\n";
	file << "Opis b³êdu : " << exception.msg << "\n";
	file << "Be³kot windowsowy : " << exception.what() << "\n";
	file << "****************************************************************\n";

}
void SFMLApplication::InitializeSFML()
{
	windowWithTexts.create(HWNDWithParameters);

	window.create(View);
	window.setFramerateLimit(FPS);

	ExceptionHashArray[exceptionList::ERROR_DURING_CLASS_INITIALIZATION] = "ERROR_DURING_CLASS_INITIALIZATION";
	ExceptionHashArray[exceptionList::ERROR_DURING_CREATING_HWND] = "ERROR_DURING_CREATING_HWND";
	ExceptionHashArray[exceptionList::ERROR_DURING_LOADING_FONT] = "ERROR_DURING_LOADING_FONT";
	ExceptionHashArray[exceptionList::ERROR_DURING_STARTING_RUNNING_ANIMATION] = "ERROR_DURING_STARTING_RUNNING_ANIMATION";
	ExceptionHashArray[exceptionList::INCORRECT_INPUT] = "INCORRECT_INPUT";
	ExceptionHashArray[exceptionList::ERROR_DURING_OPENING_FILE] = "ERROR_DURING_OPENING_FILE";

	try
	{
		if (!font.loadFromFile("BebasNeue.otf"))
		{
			throw exceptionWithMsg("Coœ posz³o nie tak podczas ³adowania Czcionki :/. SprawdŸ czy plik BebasNeue.otf znajduje siê w folderze z aplikacj¹",
				exceptionList::ERROR_DURING_LOADING_FONT);
		}
	}
	catch (exceptionWithMsg msg)
	{
		handleExceptions(msg);
	}


	switchState(statesOfMachine::INITIALIZETHROW);
}
void SFMLApplication::updateLinesToCircle()
{
	linesToCenterOfObject[0].position.x = object.getCenterOfCircle().x;
	linesToCenterOfObject[0].position.y = SFMLWINDOWHEIGHT;
	linesToCenterOfObject[1].position = object.getCenterOfCircle();
	linesToCenterOfObject[2].position.x = 0;
	linesToCenterOfObject[2].position.y = object.getCenterOfCircle().y;
	linesToCenterOfObject[3].position = object.getCenterOfCircle();
	window.draw(linesToCenterOfObject);

}
bool SFMLApplication::mouseIntersectWithGivenCircle(sf::CircleShape circle)
{

	// mo¿na to zmieœciæ w jednej linijce ale postawmy na syntatic sugar
	double xDistance = sf::Mouse::getPosition(window).x + xViewMove - (circle.getPosition().x + circle.getRadius());
	double yDistance = sf::Mouse::getPosition(window).y + yViewMove - (circle.getPosition().y + circle.getRadius());
	if (sqrt(pow(xDistance, 2) + pow(yDistance, 2)) <= circle.getRadius())
	{
		return true;
	}
	return false;
}
void SFMLApplication::initializeWinAPIThings()
{
	// Inicjalizacja ca³ego GUI ;_;
	// nie warto nawet na to patrzeæ ...

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////// kontrolki  INITIALIZETHROW ///////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		// przewspania³a inicjalizacja kontrolek winAPI 
		// https://msdn.microsoft.com/pl-pl/library/windows/desktop/ms632680(v=vs.85).aspx
		intializeThrowLeftTable = CreateWindowEx(WS_EX_CLIENTEDGE,
			windowsApplication::className,
			"left Table",
			WS_CHILD | WS_VISIBLE,
			CW_USEDEFAULT, CW_USEDEFAULT,
			XMARGIN, SFMLWINDOWHEIGHT + YMARGINUP + GUIHEIGHT,
			windowsApplication::hWnd,
			NULL, windowsApplication::hInstance,
			NULL);

		intializeThrowRightTable = CreateWindowEx(WS_EX_CLIENTEDGE,
			windowsApplication::className,
			"right Table",
			WS_CHILD | WS_VISIBLE,
			XMARGIN + SFMLWINDOWWIDTH, 0,
			XMARGIN, SFMLWINDOWHEIGHT + YMARGINUP + GUIHEIGHT,
			windowsApplication::hWnd,
			NULL, windowsApplication::hInstance,
			NULL);

		// zaczyna siê rzeŸ ...
		///////////////////////////////////////////////////////////////////////
		// label z napisem ustaw poz X 
		l_SetPositionX = CreateWindowEx(
			0, "STATIC", NULL,
			WS_CHILD | WS_VISIBLE | SS_CENTER,
			0, YMARGINUP, XMARGIN, 50, intializeThrowLeftTable,
			NULL, windowsApplication::hInstance, NULL);

		SetWindowText(l_SetPositionX, "Ustaw Pozycjê X :");

		// text box z wprowadzeniem pozycji X 
		tb_setPosX = CreateWindowEx(
			0, "EDIT", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER | ES_NUMBER,
			XMARGIN / 4, YMARGINUP + 50, XMARGIN / 2, XMARGIN / 4, intializeThrowLeftTable,
			NULL, windowsApplication::hInstance, NULL);

		// guziczek z ustawieniem pozycji X 
		b_setPositionX = CreateWindowEx(
			0, "BUTTON", NULL,
			WS_CHILD | WS_VISIBLE | BS_PUSHLIKE,
			XMARGIN / 4, YMARGINUP + 50 + XMARGIN / 4, XMARGIN / 2, XMARGIN / 4,
			intializeThrowLeftTable, (HMENU)SETPOSX, windowsApplication::hInstance, NULL);

		SetWindowText(b_setPositionX, "Ustaw ");


		// label z ustawieniem pozycji Y
		l_SetPositionY = CreateWindowEx(
			0, "STATIC", NULL,
			WS_CHILD | WS_VISIBLE | SS_CENTER,
			0, YMARGINUP + DISTANCEBTWSECTIONS + 50 + 2 * XMARGIN / 4, XMARGIN, 50,
			intializeThrowLeftTable, NULL, windowsApplication::hInstance, NULL);

		SetWindowText(l_SetPositionY, "Ustaw Pozycjê Y:");

		// texbox z ustawieniem pozycji Y
		tb_setPosY = CreateWindowEx(
			0, "EDIT", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER | ES_NUMBER,
			XMARGIN / 4, YMARGINUP + DISTANCEBTWSECTIONS + 2 * 50 + 2 * XMARGIN / 4, XMARGIN / 2, XMARGIN / 4,
			intializeThrowLeftTable, NULL, windowsApplication::hInstance, NULL);

		// guzik z ustawieniem pozycji Y
		b_setPositionY = CreateWindowEx(
			0, "BUTTON", NULL,
			WS_CHILD | WS_VISIBLE | BS_PUSHLIKE,
			XMARGIN / 4, YMARGINUP + DISTANCEBTWSECTIONS + 2 * 50 + 3 * XMARGIN / 4, XMARGIN / 2, XMARGIN / 4,
			intializeThrowLeftTable, (HMENU)SETPOSY, windowsApplication::hInstance, NULL);

		SetWindowText(b_setPositionY, "Ustaw");

		// label z ustawieniem si³y 
		l_setV = CreateWindowEx(
			0, "STATIC", NULL,
			WS_CHILD | WS_VISIBLE | SS_CENTER,
			0, YMARGINUP + 2 * DISTANCEBTWSECTIONS + 2 * 50 + 4 * XMARGIN / 4, XMARGIN, 50,
			intializeThrowLeftTable, NULL, windowsApplication::hInstance, NULL);
		SetWindowText(l_setV, "Ustaw si³ê rzutu (m/s) :");

		// texbox z ustawieniem si³y
		tb_setV = CreateWindowEx(
			0, "EDIT", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER | ES_NUMBER,
			XMARGIN / 4, YMARGINUP + 2 * DISTANCEBTWSECTIONS + 3 * 50 + 4 * XMARGIN / 4, XMARGIN / 2, XMARGIN / 4,
			intializeThrowLeftTable, NULL, windowsApplication::hInstance, NULL);

		// guzik z ustawieniem si³y
		b_setV = CreateWindowEx(
			0, "BUTTON", NULL,
			WS_CHILD | WS_VISIBLE | BS_PUSHLIKE,
			XMARGIN / 4, YMARGINUP + 2 * DISTANCEBTWSECTIONS + 3 * 50 + 5 * XMARGIN / 4, XMARGIN / 2, XMARGIN / 4,
			intializeThrowLeftTable, (HMENU)SETV, windowsApplication::hInstance, NULL);
		SetWindowText(b_setV, "Ustaw");

		// label z ustawieniem k¹tu 
		l_setAngle = CreateWindowEx(
			0, "STATIC", NULL,
			WS_CHILD | WS_VISIBLE | SS_CENTER,
			0, YMARGINUP + 3 * DISTANCEBTWSECTIONS + 3 * 50 + 6 * XMARGIN / 4, XMARGIN, 50,
			intializeThrowLeftTable, NULL, windowsApplication::hInstance, NULL);
		SetWindowText(l_setAngle, "Ustaw k¹t: ");

		// textbox z ustawieniem k¹tu
		tb_setAngle = CreateWindowEx(
			0, "EDIT", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER | ES_NUMBER,
			XMARGIN / 4, YMARGINUP + 3 * DISTANCEBTWSECTIONS + 4 * 50 + 6 * XMARGIN / 4, XMARGIN / 2, XMARGIN / 4,
			intializeThrowLeftTable, NULL, windowsApplication::hInstance, NULL);

		// guzik z ustawieniem k¹tu 
		b_setAngle = CreateWindowEx(
			0, "BUTTON", NULL,
			WS_CHILD | WS_VISIBLE | BS_PUSHLIKE,
			XMARGIN / 4, YMARGINUP + 3 * DISTANCEBTWSECTIONS + 4 * 50 + 7 * XMARGIN / 4, XMARGIN / 2, XMARGIN / 4,
			intializeThrowLeftTable, (HMENU)SETANGLE, windowsApplication::hInstance, NULL);
		SetWindowText(b_setAngle, "Ustaw");

		// guzik który uruchomii animacjê 
		b_runAnimation = CreateWindowEx(
			0, "BUTTON", NULL,
			WS_VISIBLE | WS_CHILD | BS_PUSHLIKE,
			XMARGIN / 8, YMARGINUP + 4 * DISTANCEBTWSECTIONS + 4 * 50 + 8 * XMARGIN / 4, XMARGIN - XMARGIN / 4, XMARGIN / 4,
			intializeThrowLeftTable, (HMENU)RUNANIMATION, windowsApplication::hInstance, NULL);
		SetWindowText(b_runAnimation, "URUCHOM");

		// label z napisem "Ustaw Text"
		l_setScale = CreateWindowEx(
			0, "STATIC", NULL,
			WS_VISIBLE | WS_CHILD | SS_LEFT,
			0, 0, XMARGIN - 5, 30, intializeThrowRightTable,
			NULL, windowsApplication::hInstance, NULL);
		SetWindowText(l_setScale, "Ustaw Skalê :");

		// label z napisem 1 m = ? px 
		l_setScale2 = CreateWindowEx(
			0, "STATIC", NULL,
			WS_VISIBLE | WS_CHILD | SS_LEFT,
			0, 30, XMARGIN - 80, 30, intializeThrowRightTable,
			NULL, windowsApplication::hInstance, NULL);

		SetWindowText(l_setScale2, "1 m = ");

		// text box który otrzyma wartoœæ 
		tb_setScale = CreateWindowEx(
			0, "EDIT", NULL,
			WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER | ES_NUMBER,
			XMARGIN - 85, 30, 40, 30, intializeThrowRightTable,
			NULL, windowsApplication::hInstance, NULL);

		SetWindowText(tb_setScale, "1");

		// label który wyœwietli napis "px"
		l_setScale3 = CreateWindowEx(
			0, "STATIC", NULL,
			WS_VISIBLE | WS_CHILD | SS_LEFT,
			XMARGIN - 40, 30, 40, 30, intializeThrowRightTable,
			NULL, windowsApplication::hInstance, NULL);

		SetWindowText(l_setScale3, "px");


		// guzik który ustawi skalê
		b_setScale = CreateWindowEx(
			0, "BUTTON", NULL,
			WS_VISIBLE | WS_CHILD | BS_PUSHLIKE,
			XMARGIN / 8, 2 * 30 + 5 // ma³y odstêp pomiêdzy guzikiem
			, XMARGIN / 2, XMARGIN / 4, intializeThrowRightTable,
			(HMENU)SETSCALE, windowsApplication::hInstance, NULL);

		SetWindowText(b_setScale, "Ustaw");

	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////// kontrolki  ANIMATETHROW //////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	isLegendBoxShowed = false;


	legendBox = CreateDialog(windowsApplication::hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);

	// lewa tablica z guziczkami do ANIMATETHROW
	animateThrowLeftTable = CreateWindowEx(WS_EX_CLIENTEDGE,
		windowsApplication::className,
		"left Table Animate Throw",
		WS_CHILD | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		XMARGIN, SFMLWINDOWHEIGHT + YMARGINUP + GUIHEIGHT,
		windowsApplication::hWnd,
		NULL, windowsApplication::hInstance,
		NULL);

	animateThrowRightTable = CreateWindowEx(WS_EX_CLIENTEDGE,
		windowsApplication::className,
		"right Table Animate Throw",
		WS_CHILD | WS_VISIBLE,
		XMARGIN + SFMLWINDOWWIDTH, 0,
		XMARGIN, SFMLWINDOWHEIGHT + YMARGINUP + GUIHEIGHT,
		windowsApplication::hWnd,
		NULL, windowsApplication::hInstance,
		NULL);

	// label z napisem zatrzymaj pozycjê/ wznów animacjê
	l_pauseAnimation = CreateWindowEx(
		0, "STATIC", NULL,
		WS_CHILD | WS_VISIBLE | SS_CENTER,
		0, DISTANCEBTWSECTIONS, XMARGIN, 50,
		animateThrowLeftTable, NULL, windowsApplication::hInstance, NULL);

	SetWindowText(l_pauseAnimation, "Zatrzymaj Animacjê");

	// guzik z zatrzymaniem animacji
	b_pauseAnimation = CreateWindowEx(
		0, "BUTTON", NULL,
		WS_CHILD | WS_VISIBLE | BS_PUSHLIKE | WS_BORDER,
		XMARGIN / 8, DISTANCEBTWSECTIONS + 50, XMARGIN*6.0 / 8, XMARGIN / 3,
		animateThrowLeftTable, (HMENU)PAUSEANIMATION, windowsApplication::hInstance, NULL);

	SetWindowText(b_pauseAnimation, "Zatrzymaj");

	// label z napisem Reset Animacji
	l_resetAnimation = CreateWindowEx(
		NULL, "STATIC", NULL,
		WS_CHILD | WS_VISIBLE | SS_CENTER,
		0, DISTANCEBTWSECTIONS + 50 + XMARGIN / 1.5, XMARGIN, XMARGIN / 2,
		animateThrowLeftTable, NULL, windowsApplication::hInstance, NULL);

	SetWindowText(l_resetAnimation, "Zresetuj  Animacjê!");

	// guzik z napisem Reset Animacji
	b_resetAnimation = CreateWindowEx(
		NULL, "BUTTON", NULL,
		WS_CHILD | WS_VISIBLE | BS_PUSHLIKE | WS_BORDER,
		XMARGIN / 8, DISTANCEBTWSECTIONS + 50 + XMARGIN / 1.5 + XMARGIN / 2, XMARGIN*6.0 / 8, XMARGIN / 3,
		animateThrowLeftTable, (HMENU)RESETANIMATION, windowsApplication::hInstance, NULL);

	SetWindowText(b_resetAnimation, "Zresetuj");

	// label z napisem poka¿ Parametry punktów
	l_showSelectedPoints = CreateWindowEx(
		NULL, "STATIC", NULL,
		WS_CHILD | WS_VISIBLE | SS_CENTER,
		0, 0, XMARGIN - 20, XMARGIN / 2,
		animateThrowRightTable, NULL, windowsApplication::hInstance, NULL);


	SetWindowText(l_showSelectedPoints, "Poka¿ wybrane Punkty");

	// button z napisem "poka¿"
	b_showSelectedPoints = CreateWindowEx(
		NULL, "BUTTON", NULL,
		WS_CHILD | WS_VISIBLE | WS_BORDER | BS_PUSHLIKE,
		XMARGIN / 8, XMARGIN / 2, XMARGIN / 2, XMARGIN / 4,
		animateThrowRightTable, (HMENU)SHOWSELECTEDPOINTS, windowsApplication::hInstance, NULL);

	SetWindowText(b_showSelectedPoints, "Poka¿");


	l_showLegend = CreateWindowEx(
		0, "STATIC", NULL,
		WS_VISIBLE | WS_CHILD | SS_CENTER | WS_BORDER,
		-15, 3 * XMARGIN, XMARGIN, XMARGIN / 2,
		animateThrowRightTable, NULL, windowsApplication::hInstance, NULL);

	SetWindowText(l_showLegend, "Poka¿ Legendê");

	b_showLegend = CreateWindowEx(
		NULL, "BUTTON", NULL,
		WS_VISIBLE | WS_BORDER | WS_CHILD | BS_PUSHLIKE,
		XMARGIN / 6, 3*XMARGIN + XMARGIN/2, XMARGIN /2, XMARGIN / 4,
		animateThrowRightTable, HMENU(SHOWLEGEND), windowsApplication::hInstance, NULL);

	SetWindowText(b_showLegend, "legenda");


	l_showLegend_i = CreateWindowEx(
		0, "STATIC", NULL,
		WS_VISIBLE | WS_CHILD | SS_CENTER | WS_BORDER,
		-15, 3 * XMARGIN, XMARGIN, XMARGIN / 2,
		intializeThrowRightTable, NULL, windowsApplication::hInstance, NULL);

	SetWindowText(l_showLegend_i, "Poka¿ Legendê");

	b_showLegend_i = CreateWindowEx(
		NULL, "BUTTON", NULL,
		WS_VISIBLE | WS_BORDER | WS_CHILD | BS_PUSHLIKE,
		XMARGIN / 6, 3 * XMARGIN + XMARGIN / 2, XMARGIN / 2, XMARGIN / 4,
		intializeThrowRightTable, HMENU(SHOWLEGEND), windowsApplication::hInstance, NULL);

	SetWindowText(b_showLegend_i, "legenda");



	showSelectedPointsLeftTable = CreateWindowEx(WS_EX_CLIENTEDGE,
		windowsApplication::className,
		"left Table Show Selected Throw",
		WS_CHILD | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		XMARGIN, SFMLWINDOWHEIGHT + YMARGINUP + GUIHEIGHT,
		windowsApplication::hWnd,
		NULL, windowsApplication::hInstance,
		NULL);

	// label który zawiera napis "poka¿ wektory"
	l_showVectors = CreateWindowEx(
		NULL, "STATIC", NULL,
		WS_VISIBLE | WS_CHILD | SS_CENTER,
		0, 50 + XMARGIN / 3 + DISTANCEBTWSECTIONS, XMARGIN, XMARGIN / 2,
		showSelectedPointsLeftTable, NULL, windowsApplication::hInstance, NULL);

	SetWindowText(l_showVectors, "Poka¿ wektory sk³adowe");

	// guzik który poka¿e wektory
	b_showVectors = CreateWindowEx(
		NULL, "BUTTON", NULL,
		WS_VISIBLE | WS_CHILD | BS_PUSHLIKE | WS_BORDER,
		XMARGIN / 4, 50 + XMARGIN / 3 + XMARGIN / 2 + DISTANCEBTWSECTIONS, XMARGIN / 2, XMARGIN / 4,
		showSelectedPointsLeftTable, (HMENU)SHOWVECTORS, windowsApplication::hInstance, NULL);
	SetWindowText(b_showVectors, "Poka¿");

	// pokazanie ca³ej drogi
	l_showRoad = CreateWindowEx(
		NULL, "STATIC", NULL,
		WS_VISIBLE | WS_CHILD | SS_CENTER,
		0, 50 + XMARGIN / 3 + 2 * XMARGIN / 2 + DISTANCEBTWSECTIONS, XMARGIN, XMARGIN / 2,
		showSelectedPointsLeftTable, NULL, windowsApplication::hInstance, NULL);
	SetWindowText(l_showRoad, "Poka¿ ca³¹ trasê");

	// guzik który je¿eli wciœniêto to schowa lub poka¿e drogê
	b_showRoad = CreateWindowEx(
		NULL, "BUTTON", NULL,
		WS_VISIBLE | WS_CHILD | WS_BORDER | BS_PUSHLIKE,
		XMARGIN / 4, 50 + XMARGIN / 3 + 3 * XMARGIN / 2 + DISTANCEBTWSECTIONS, XMARGIN / 2, XMARGIN / 4,
		showSelectedPointsLeftTable, (HMENU)SHOWROAD, windowsApplication::hInstance, NULL
	);
	SetWindowText(b_showRoad, "Poka¿");

	// label który wyœwietli napis "poka¿ wszysatkiew zaznaczone punkty"
	l_showAllPoints = CreateWindowEx(
		NULL, "STATIC", NULL,
		WS_VISIBLE | WS_CHILD | SS_CENTER,
		0, 50 + XMARGIN / 3 + 4 * XMARGIN / 2 + DISTANCEBTWSECTIONS, XMARGIN, XMARGIN / 2,
		showSelectedPointsLeftTable, NULL, windowsApplication::hInstance, NULL);

	SetWindowText(l_showAllPoints, "Poka¿ wszystkie zaznaczone Punkty");

	// guzik który odpowiednia schowa lub wyœwietli wszystkie zaznaczone punkty
	b_showAllPoints = CreateWindowEx(
		NULL, "BUTTON", NULL,
		WS_VISIBLE | WS_CHILD | WS_BORDER | BS_PUSHLIKE,
		XMARGIN / 4, 50 + XMARGIN / 3 + 5 * XMARGIN / 2 + DISTANCEBTWSECTIONS, XMARGIN / 2, XMARGIN / 4,
		showSelectedPointsLeftTable, (HMENU)SHOWALLPOINTS, windowsApplication::hInstance, NULL);

	SetWindowText(b_showAllPoints, "Poka¿");

	// label który wyœwietli napis "manipulowanie punktami"
	l_manipulateShowedPoints = CreateWindowEx(
		NULL, "STATIC", NULL,
		WS_VISIBLE | WS_CHILD | SS_CENTER | WS_BORDER,
		0, 0, XMARGIN, XMARGIN / 4, showSelectedPointsLeftTable,
		NULL, windowsApplication::hInstance, NULL);

	SetWindowText(l_manipulateShowedPoints, "Manipulowanie punktami");


	// poprzedni punkt
	b_previousPoint = CreateWindowEx(
		NULL, "BUTTON", NULL,
		WS_VISIBLE | WS_CHILD | BS_PUSHLIKE | WS_BORDER,
		XMARGIN / 8, XMARGIN / 4, XMARGIN / 1.5, XMARGIN / 3, showSelectedPointsLeftTable,
		(HMENU)PREVIOUSPOINT, windowsApplication::hInstance, NULL);

	SetWindowText(b_previousPoint, "Poprzedni");

	// nastêpny punkt
	b_nextPoint = CreateWindowEx(
		NULL, "BUTTON", NULL,
		WS_VISIBLE | WS_CHILD | BS_PUSHLIKE | WS_BORDER,
		XMARGIN / 8, XMARGIN / 4 + XMARGIN / 3, XMARGIN / 1.5, XMARGIN / 3, showSelectedPointsLeftTable,
		(HMENU)NEXTPOINT, windowsApplication::hInstance, NULL);

	SetWindowText(b_nextPoint, "Nastêpny");

	// label z napisem "Poka¿ wszystkie obliczone punkty"
	l_selectAllPoints = CreateWindowEx(
		0, "STATIC", NULL,
		WS_VISIBLE | WS_CHILD | SS_CENTER | WS_BORDER,
		0, 50 + XMARGIN / 3 + 6 * XMARGIN / 2 + DISTANCEBTWSECTIONS, XMARGIN, XMARGIN / 3,
		showSelectedPointsLeftTable, NULL, windowsApplication::hInstance, NULL);

	SetWindowText(l_selectAllPoints, "Poka¿ wszystkie obliczone punkty");

	b_selectAllPoints = CreateWindowEx(
		0, "BUTTON", NULL,
		WS_VISIBLE | WS_CHILD | BS_PUSHLIKE | WS_BORDER,
		XMARGIN / 6, 50 + XMARGIN / 3 + 6 * XMARGIN / 2 + DISTANCEBTWSECTIONS + XMARGIN / 3, 2 * XMARGIN / 3, XMARGIN / 3,
		showSelectedPointsLeftTable, (HMENU)SELECTALLPOINTS, windowsApplication::hInstance, NULL);
	SetWindowText(b_selectAllPoints, "Zaznacz");


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
LPSTR SFMLApplication::getTextFromTextBox(HWND hwnd)
{
	DWORD length = GetWindowTextLength(hwnd);
	LPSTR Buffor = (LPSTR)GlobalAlloc(GPTR, length + 1);
	GetWindowText(hwnd, Buffor, length + 1);
	return Buffor;
}
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:

		break;
		// Quit when we close the main window
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		exit(0);
	}
	case WM_COMMAND:
		switch (wParam)
		{
			// sygna³ od guzika który ustawia pozycjê X
		case SETPOSX:
			SIGNALFROMOCONTROLS = SETPOSX;
			break;

		case SETPOSY:
			SIGNALFROMOCONTROLS = SETPOSY;
			break;

		case SETV:
			SIGNALFROMOCONTROLS = SETV;
			break;

		case SETANGLE:
			SIGNALFROMOCONTROLS = SETANGLE;
			break;

		case SETSCALE:
			SIGNALFROMOCONTROLS = SETSCALE;
			break;

		case RUNANIMATION:
			SIGNALFROMOCONTROLS = RUNANIMATION;
			break;

		case RESETANIMATION:
			SIGNALFROMOCONTROLS = RESETANIMATION;
			break;

		case PAUSEANIMATION:
			SIGNALFROMOCONTROLS = PAUSEANIMATION;
			break;

		case SHOWSELECTEDPOINTS:
			SIGNALFROMOCONTROLS = SHOWSELECTEDPOINTS;
			break;

		case SHOWVECTORS:
			SIGNALFROMOCONTROLS = SHOWVECTORS;
			break;
		case SHOWROAD:
			SIGNALFROMOCONTROLS = SHOWROAD;
			break;
		case SHOWALLPOINTS:
			SIGNALFROMOCONTROLS = SHOWALLPOINTS;
			break;
		case NEXTPOINT:
			SIGNALFROMOCONTROLS = NEXTPOINT;
			break;
		case PREVIOUSPOINT:
			SIGNALFROMOCONTROLS = PREVIOUSPOINT;
			break;
		case SELECTALLPOINTS:
			SIGNALFROMOCONTROLS = SELECTALLPOINTS;
			break;
		case SHOWLEGEND:
			SIGNALFROMOCONTROLS = SHOWLEGEND;
			break;
		}
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
INT_PTR CALLBACK DlgProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_COMMAND:
	{
		SIGNALFROMOCONTROLS = LEGENDCLOSED;
	}
	break;

	default:
		return FALSE;
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//				metody oraz funkcje do INITIALIZETHROW

// OPIS : Stan w którym podajemy wartoœci oraz parametry niezbêdne do rzutu (poprzez guziki albo interakcjê z obiektem)
void SFMLApplication::InitializeThrow()
{
	ClickedOnBall = false;
	moveBall = false;
	startingXPos = startingYPos = 0;
	V = Vx = Vy = angle = sinA = cosA = startingXPos = startingYPos = 0;

	object.moveObject(sf::Vector2f(0, SFMLWINDOWHEIGHT - 2 * object.getCircle().getRadius()));

	// utworzenie paneli guziczków oraz innej czarnej magii winAPI :>


	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(34);
	text.setFillColor(sf::Color::Color(0x0, 0x80, 0xff));
	text.setOutlineColor(sf::Color::Color(0x0, 0x7A, 0xff));
	text.setOutlineThickness(0.5);

	VxColor = sf::Color::Color(0xff, 0x80, 0x0);
	VxColorOutline = sf::Color::Color(0xff, 0x78, 0x0);
	VyColor = sf::Color::Color(0x0, 0xD0, 0x0);
	VyColorOutline = sf::Color::Color(0x0, 0xDA, 0x0);
	VColor = sf::Color::Color(0xff, 0xff, 0x0);
	VColorOutline = sf::Color::Color(0xff, 0xf0, 0x0);

	// 0. pozycja X 
	// 1. pozycja Y
	// 2. wartoœæ wektora X 
	// 3. wartoœæ wektora Y
	// 4. Si³a rzutu
	// 5. k¹t
	// 6. sinus
	// 7. cosinus
	// 8. napis Vx
	// 9. napis Vy
	// 10. napis V


	// Zabawa pozycj¹ oraz kolorkami

	for (int i = 0; i < 4; ++i)
	{
		text.setPosition((i / 2) * 5.5 * text.getCharacterSize(), (i % 2) * text.getCharacterSize());
		InitializeThrowTexts.push_back(text);
	}
	for (int i = 4; i < 8; ++i)
	{
		text.setPosition((i / 2) * (5.6 + (i / 6) / 3.0) * text.getCharacterSize() - 50, (i % 2) * text.getCharacterSize());
		InitializeThrowTexts.push_back(text);
	}

	text.setPosition(0, 0);
	for (int i = 0; i < 3; ++i)
		InitializeThrowTexts.push_back(text);

	// Wektor sk³adowy Vx
	setColors(2, 8, "Vx", 0, VxColor, VxColorOutline);

	// Wektor sk³adowy Vy
	setColors(3, 9, "Vy", 2, VyColor, VyColorOutline);

	// Wektor sk³adowy V
	setColors(4, 10, "V", 4, VColor, VColorOutline);


	// ustawienie GUI

	sf::RectangleShape line;
	line.setFillColor(sf::Color::Color(0x0, 0x7A, 0xff));
	line.setOutlineColor(sf::Color::Color(0x0, 0x7f, 0xff));
	line.setOutlineThickness(1);

	line.setPosition(0, 2 * text.getCharacterSize() + 10);
	line.setSize(sf::Vector2f(SFMLWINDOWWIDTH, 5));
	gui.push_back(line);
	for (int i = 0; i < 3; ++i)
	{
		line.setPosition(InitializeThrowTexts[(i + 1) * 2].getPosition().x - 7, 0);
		line.setSize(sf::Vector2f(5, 2 * text.getCharacterSize() + 10));
		gui.push_back(line);
	}


	ShowWindow(animateThrowLeftTable, false);
	ShowWindow(animateThrowRightTable, false);
	ShowWindow(showSelectedPointsLeftTable, false);
	ShowWindow(intializeThrowLeftTable, true);
	ShowWindow(intializeThrowRightTable, true);
}
void SFMLApplication::handleStateInitializeThrow()
{

	window.clear();

	window.draw(object.getCircle());
	updateInitializationThrow();
	drawInitializeThrowTexts();

	window.display();

}
void SFMLApplication::updateInitializationThrow()
{
	if (!ClickedOnBall)
	{
		// interkakcja z graczem oraz wyœwietlanie tego magicznego trójk¹ta
		if (mouseIntersectWithGivenCircle(object.getCircle()))
		{
			object.setColorOfCircle(highlighted);
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				ClickedOnBall = true;
			}
			else if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
			{
				moveBall = true;
			}
			else
			{
				moveBall = false;
			}
		}
		else if (object.getCircle().getFillColor() == highlighted)
		{
			object.setColorOfCircle(defaultColor);
		}
	}
	else
	{
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !mouseIntersectWithGivenCircle(object.getCircle()))
		{
			switchState(statesOfMachine::ANIMATETHROW);
			return;
		}

		//  je¿eli naciœniemy prawy guzik to chcemy ¿eby nasza kulka by³a "odciœniêta"
		if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
			ClickedOnBall = false;


		handleDrawingAndUpdatingTriangle();


		window.draw(triangle);
	}
	if (moveBall && sf::Mouse::getPosition(window).x >= object.getCircle().getRadius()
		&& sf::Mouse::getPosition(window).x <= SFMLWINDOWWIDTH - object.getCircle().getRadius()
		&& sf::Mouse::getPosition(window).y >= object.getCircle().getRadius()
		&& sf::Mouse::getPosition(window).y <= SFMLWINDOWHEIGHT - object.getCircle().getRadius())
	{
		object.moveObject(sf::Vector2f(sf::Mouse::getPosition(window).x - CIRCLERADIUS, sf::Mouse::getPosition(window).y - CIRCLERADIUS));
	}
	manageInitializeThrowTexts();
}
void SFMLApplication::handleDrawingAndUpdatingTriangle()
{
	// zajmujemy siê rysowaniem naszego trójk¹ta 
	triangle[0] = object.getCenterOfCircle();
	triangle[1].position.x = sf::Mouse::getPosition(window).x;
	triangle[1].position.y = triangle[0].position.y;
	triangle[2] = triangle[0];
	triangle[3].position.x = triangle[2].position.x;
	triangle[3].position.y = sf::Mouse::getPosition(window).y;
	triangle[4] = triangle[0];
	triangle[5].position.x = triangle[1].position.x;
	triangle[5].position.y = triangle[3].position.y;
}
void SFMLApplication::manageInitializeThrowTexts()
{
	startingXPos = object.getPosition().x;
	startingYPos = SFMLWINDOWHEIGHT - object.getPosition().y - 2 * object.getCircle().getRadius();
	InitializeThrowTexts[0].setString("Pozycja X : " + std::to_string(int(startingXPos/scale)) + "m");
	InitializeThrowTexts[1].setString("Pozycja Y : " + std::to_string(int(startingYPos/scale)) + "m");


	if (ClickedOnBall)
	{
		windowWithTexts.clear();


		// syntatic sugar 
		Vx = (sf::Mouse::getPosition(window).x - object.getCenterOfCircle().x);
		Vy = (object.getCenterOfCircle().y - sf::Mouse::getPosition(window).y);
		V = sqrt(pow(Vx, 2) + pow(Vy, 2)) ;
		sinA = Vy / V;
		cosA = Vx / V;
		angle = atan2(Vy, Vx) * 180 / M_PI;

		// si³a wekora x
		InitializeThrowTexts[2].setString("Vx = " + std::to_string(static_cast<int>(Vx / scale)) + "m/s");
		InitializeThrowTexts[3].setString("Vy = " + std::to_string(static_cast<int>(Vy / scale)) + "m/s");
		// si³a z jak¹ rzucamy
		InitializeThrowTexts[4].setString("V = " + std::to_string(V / scale) + "m/s");

		// k¹t
		InitializeThrowTexts[5].setString("kat = " + std::to_string(angle));

		// sinus
		InitializeThrowTexts[6].setString("sin A = " + std::to_string(sinA));

		//cosinuis
		InitializeThrowTexts[7].setString("cos A = " + std::to_string(cosA));

		// tekst Vx
		InitializeThrowTexts[8].setString("Vx");
		InitializeThrowTexts[8].setPosition(sf::Vector2f((sf::Mouse::getPosition(window).x - object.getCenterOfCircle().x) / 2 + object.getPosition().x
			, object.getCenterOfCircle().y - InitializeThrowTexts[8].getCharacterSize() / 1.5));
		// tekst Vy
		InitializeThrowTexts[9].setString("Vy");
		InitializeThrowTexts[9].setPosition(sf::Vector2f(object.getCenterOfCircle().x - InitializeThrowTexts[9].getCharacterSize() / 2
			, sf::Mouse::getPosition(window).y + (object.getCenterOfCircle().y - sf::Mouse::getPosition(window).y) / 2));

		InitializeThrowTexts[10].setString("V");
		InitializeThrowTexts[10].setPosition(InitializeThrowTexts[8].getPosition().x, InitializeThrowTexts[9].getPosition().y);

	}
	else
	{
		for (int i = 2; i < InitializeThrowTexts.size(); ++i)
		{
			InitializeThrowTexts[i].setString("");
		}
	}

}
void SFMLApplication::drawInitializeThrowTexts()
{
	windowWithTexts.clear();
	windowWithTexts.draw(InitializeThrowTexts[0]);
	windowWithTexts.draw(InitializeThrowTexts[1]);
	if (ClickedOnBall)
	{
		for (int i = 0; i < gui.size(); ++i)
			windowWithTexts.draw(gui[i]);

		for (int i = 2; i < 8; ++i)

			windowWithTexts.draw(InitializeThrowTexts[i]);

		for (int i = 8; i < 11; ++i)
		{
			window.draw(InitializeThrowTexts[i]);
		}
	}
	windowWithTexts.display();
}
void SFMLApplication::setColors(int indexOfText, int indexOfSign, std::string text, int indexOfLine, sf::Color color, sf::Color outlineColor)
{
	InitializeThrowTexts[indexOfText].setFillColor(color);
	InitializeThrowTexts[indexOfText].setOutlineColor(outlineColor);
	InitializeThrowTexts[indexOfSign].setFillColor(color);
	InitializeThrowTexts[indexOfSign].setOutlineColor(outlineColor);
	InitializeThrowTexts[indexOfSign].setString(text);
	triangle[indexOfLine].color = color;
	triangle[indexOfLine + 1].color = color;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//				metody oraz funkcje do ANIMATETHROW
void   SFMLApplication::initializeAnimateThrow()
{
	squareBoardLinesX.clear();
	squareBoardTextsX.clear();

	squareBoardLinesY.clear();
	squareBoardTextsY.clear();

	ShowWindow(intializeThrowLeftTable, false);
	ShowWindow(intializeThrowRightTable, false);
	ShowWindow(animateThrowLeftTable, true);
	ShowWindow(animateThrowRightTable, true);

	addedMaxHeight = animationPaused = showSelectedPoints = false;

	//zmienna potrzebna do zooma
	zoomedBy = 1;
	this->timeElapsedFromLastPause = 0;

	prevXChange = prevYChange = xChangeByZooming = yChangeByZooming = 0;

	// okreœlamy jakiego typu jest to rzut 
	// je¿eli k¹t == 90* to mamy rzut pionowy (thx cpt. Obvious)
	if (angle == 90)
	{
		typeOfThrow = typesOfThrow::VERTICALTHROW;
	}
	// je¿eli k¹t bêdzie wiêkszy od 0 to mamy rzut ukoœny
	else if (angle > 0)
	{
		typeOfThrow = typesOfThrow::CROSSWISETHROW;
	}
	// w przeciwnym wypadku mamy k¹t mniejszy lub równy 0 czy rzut poziomy
	else
	{
		typeOfThrow = typesOfThrow::HORIZONTALTHROW;
	}

	// domyœlna sta³a grawitacyjna to sta³a grawitacyjna ziemi
	G_const = EARTHGRAVITY;

	//  zabawa kolorkami
	colorOfStep = sf::Color::Color(defaultColor);
	highlightedPointColor = sf::Color::Color(0x0, 0x80, 0xff);
	selectedPointsColor = sf::Color::Color(0xA0, 0xA0, 0xA0);

	// ustawienie podstawowych parametrów "kroku"
	step.setParameters(sf::Vector2f(0, 0), defaultColor, 4.f);

	// zmienne i obiekty do zarz¹dzania widokiem
	xViewMove = yViewMove = indexOfMaxHeight = howManySelectedPoints = 0;
	staticView = window.getView();
	DynamicView = window.getView();

	// ustawienie widoku
	window.setView(DynamicView);

	// obliczenie Hmax, Ts, Tw i zasiêgu
	calculateVariables();

	//ustawienie podstawowych atrybutów tekstu
	text.setCharacterSize(28);
	text.setFont(font);
	text.setFillColor(sf::Color::Color(0x0, 0x80, 0xff));

	// ustawienie pozycji tekstów oraz wrzucenie ich do wektora
	for (int i = 0; i < 9; ++i)
	{
		text.setPosition((i / 3) * 10 * text.getCharacterSize(), i % 3 * text.getCharacterSize() * 1.2);
		text.setString("");
		animateThrowTexts.push_back(text);
		showSelectedPointsParams.push_back(text);
	}


	// ustawienie przedzia³ek GUI
	sf::RectangleShape rect;
	rect.setFillColor(text.getFillColor());
	rect.setOutlineColor(text.getOutlineColor());

	// dolna przedzia³ka
	rect.setPosition(sf::Vector2f(0, 2 * text.getCharacterSize() * 1.2 + 1.5*text.getCharacterSize()));
	rect.setSize(sf::Vector2f(SFMLWINDOWWIDTH, 5));
	animateThrowGUI.push_back(rect);

	for (int i = 0; i < 3; ++i)
	{
		rect.setPosition((i + 1) * 10 * text.getCharacterSize() - 10, 0);
		rect.setSize(sf::Vector2f(5, 3 * text.getCharacterSize() + text.getCharacterSize()));
		animateThrowGUI.push_back(rect);
	}

	sf::Text Text = text;
	Text.setFillColor(sf::Color::Color(0x0, 0xB0, 0xf));
	Text.setCharacterSize(18.f);
	Text.setFont(font);

	for (int i = 0; i < 6; ++i)
	{
		CrosswiseEquation.push_back(Text);
	}
	CrosswiseEquation[0].setPosition(0, 120);
	CrosswiseEquation[0].setString("                                               g");
	CrosswiseEquation[1].setPosition(0, 140);
	CrosswiseEquation[1].setString("y = x * tg a -  ------------------------ x^2");
	CrosswiseEquation[2].setPosition(0, 160);
	CrosswiseEquation[2].setString("                        2 * V0^2  * cos^2 * a");

	CrosswiseEquation[3].setPosition(270, 120);
	CrosswiseEquation[4].setPosition(270, 140);
	CrosswiseEquation[5].setPosition(270, 160);



	selectedCircle.setupParameters(V / scale, cosA, sinA, G_const* scale);

	// zrobienie z naszego okienka takiej strony w kratkê 
	
////////////////////////////////////////////////////////////////////////////////////////////////////////
//		Dolny przedzia³
	// korzystamy ze wzoru na zasiêg i maksymaln¹ wysokoœæ
	this->linesRangeX = (this->V*this->V) / G_const * sin(2 * angle * M_PI / 180) + 2*object.getCircle().getRadius() ;
	this->linesRangeY = this->V *this->V  * sinA * sinA / (2 * G_const) + 2 * object.getCircle().getRadius();

	this->linesStepX  = max(CIRCLERADIUS,this->linesRangeX / 400);
	this->linesStepY = max(CIRCLERADIUS, linesRangeY / 400);

	int howManyStepsX = max(std::ceil(this->linesRangeX / this->linesStepX),SFMLWINDOWWIDTH/this->linesStepX);
	int howManyStepsY = max(std::ceil(this->linesRangeY / this->linesStepY), SFMLWINDOWHEIGHT / linesStepY);

	for (int i = 0; i < howManyStepsX + SFMLWINDOWWIDTH/ linesStepX; ++i)
	{
		line[0].position.x = i * this->linesStepX + object.getCircle().getRadius();
		line[1].position.x = i * this->linesStepX + object.getCircle().getRadius();
		line[0].position.y = SFMLWINDOWHEIGHT+50;
		line[1].position.y = min(SFMLWINDOWWIDTH / 2 - (howManyStepsY + SFMLWINDOWHEIGHT / linesStepY-1) * linesStepY, 0);
		squareBoardLinesX.push_back(line);
		if (i % 4 == 0)
		{
			Text.setFillColor(sf::Color::Color(0xff,0x80,0x0));
			Text.setString(std::to_string(static_cast<float>(i * this->linesStepX/this->scale)));
			Text.setPosition(line[0].position.x-Text.getCharacterSize()/2, line[0].position.y-75);
			squareBoardTextsX.push_back(Text);
		}
	}
////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
////////////////////////////////////////////////////////////////////////////////////////////////////////
//	górny przedzia³
	// korzystamy ze wzoru ma maksymaln¹ wysokoœæ
	this->linesStepY  = max(CIRCLERADIUS,linesRangeY/ 100);

	for (int i = 0; i < howManyStepsY+SFMLWINDOWHEIGHT/linesStepY; ++i)
	{
		line[0].position.x = -50;
		line[1].position.x = max((howManyStepsX + SFMLWINDOWWIDTH / linesStepX) * linesStepX,SFMLWINDOWWIDTH);

		line[0].position.y = SFMLWINDOWHEIGHT - i * this->linesStepY - object.getCircle().getRadius();
		line[1].position.y = SFMLWINDOWHEIGHT - i * this->linesStepY - object.getCircle().getRadius();
		squareBoardLinesY.push_back(line);
		if (i % 2 == 0)
		{
			Text.setFillColor(sf::Color::Color(0xff, 0x80, 0x0));
			Text.setString(std::to_string(static_cast<float>(i * this->linesStepY / this->scale)));
			Text.setPosition(line[0].position.x - 25,line[0].position.y- Text.getCharacterSize()/2);
			squareBoardTextsY.push_back(Text);
		}
	}


////////////////////////////////////////////////////////////////////////////////////////////////////////

	printAllValuesIntoFile();
	timeElapsed.restart();

}
void   SFMLApplication::handleStateAnimateThrow()
{
	window.clear();

	if (!showSelectedPoints)
	{
		if (!animationPaused)
		{
			updateAnimateThrow();
			displayAnimateThrow();
			manageView();
		}
		else if (animationPaused)
		{
			pauseAnimation();
			displayAnimateThrow();
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::L) && GETSEC(delayBtwChangingPauseBool) > 0.5f)
		{
			this->showLines = !this->showLines;
			delayBtwChangingPauseBool.restart();
		}

		manageMovingOfProjection();
		manageZoom();
		drawAnimateThrowTexts();
	}
	else
	{
		handleShowSelectedPoints();
		manageZoom();
	}

	window.display();
}
void   SFMLApplication::manageRoad()
{
	// je¿eli natrafimy na najwiêksz¹ wysokoœæ to dodajemy specjany krok
	if (!addedMaxHeight && (GETSEC(timeElapsed) + timeElapsedFromLastPause >= timeToReachMaxHeight && height >= maxHeight - maxHeight * 1 / FPS))
	{
		step.setFillColor(maxHeightColor);
		step.setRadius(1.5 * step.getRadius());
		step.create(object.getCenterOfCircle(), (GETSEC(timeElapsed) + this->timeElapsedFromLastPause), scaledPosX, scaledHmax);
		road.push_back(step);
		indexOfMaxHeight = road.size() - 1;
		step.setFillColor(defaultColor);
		step.setRadius(step.getRadius() / 1.5);
		addedMaxHeight = true;
	}
	else if (GETSEC(delayBtwAddingSteps) >= 0.01)
	{
		step.create(object.getCenterOfCircle(), (GETSEC(timeElapsed) + this->timeElapsedFromLastPause), scaledPosX / scale, scaledPosY);
		road.push_back(step);
		delayBtwAddingSteps.restart();
	}

}
void   SFMLApplication::pauseAnimation()
{

	// je¿eli wciœniêto to przywracamy do poprzedniego stanu
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::P) && GETSEC(delayBtwChangingPauseBool) > 0.5f)
	{
		handleTimeManagmentWhenPaused();
		animationPaused = false;
		timeElapsed.restart();
		delayBtwChangingPauseBool.restart();
	}

	if (indexOfMaxHeight && road[indexOfMaxHeight].getFillColor() != maxHeightColor && road[indexOfMaxHeight].getFillColor() != selectedPointsColor)
	{
		road[indexOfMaxHeight].setFillColor(maxHeightColor);
	}


	// interakcja kó³ka z myszk¹ ¿eby pokazaæ jakieœ punkty 
	handleSelectingPoints();
}
void   SFMLApplication::updateAnimateThrow()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::L) && GETSEC(delayBtwChangingPauseBool) > 0.5f)
	{
		this->showLines = !this->showLines;
		delayBtwChangingPauseBool.restart();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::P) && GETSEC(delayBtwChangingPauseBool) > 0.5f)
	{
		handleTimeManagmentWhenPaused();
		delayBtwChangingPauseBool.restart();
		animationPaused = true;
	}
	// sprawdzamy czy nasz ruch siê nie zakoñczy³
	if (object.getPosition().y + 2*object.getCircle().getRadius() > SFMLWINDOWHEIGHT + 1  )
	{
		// nie wywo³ujemy switch'a poniewa¿ switch kasuje wszystkie napisy i pozycjê a chcemy to zachowaæ 

		animationPaused = true;
		timeElapsedFromLastPause += GETSEC(timeElapsed);
		timeElapsed.restart();
		return;
	}
	prevPosition = sf::Vector2f(object.getPosition().x, object.getPosition().y);

	// obliczamy pozycjê X i Y na którym cia³o powinno siê znajdowaæ
	height = calculateHeight((GETSEC(this->timeElapsed) + this->timeElapsedFromLastPause), this->scale, true);
	width =  calcluateWidth((GETSEC(this->timeElapsed) + this->timeElapsedFromLastPause),this->scale, true);

	// ruszamy nasz obiekt
	object.moveObject(sf::Vector2f(width, SFMLWINDOWHEIGHT - height - 2 * object.getCircle().getRadius()));

	// metoda która zarz¹dza nasz¹ drog¹ 
	manageRoad();

	// update naszych linii poprowadzonych do œrodka cia³a 
	updateLinesToCircle();


	// update naszych tekstów
	handleAnimateThrowTexts();


}
void   SFMLApplication::calculateVariables()
{
	double scaledV = V / scale;
	double delta;

	switch (typeOfThrow)
	{
		// obliczenia dla rzutu pionowego
	case typesOfThrow::VERTICALTHROW:
		// zasiêg = 0 (thx cpt. Obvious)
		range = startingXPos;
		scaledRange = startingXPos / scale;


		// maksymalna wysokoœæ pozostaje ta sama
		maxHeight = startingYPos + (V*V) / (2 * G_const);
		scaledHmax = startingYPos / scale + (scaledV*scaledV) / (2 * G_const);

		// czas do osi¹gniêcia maksymalnej wysokoœci : Tw = V/G
		timeToReachMaxHeight = V / G_const;
		scaledTimeToReachMaxHeight = scaledV / G_const;

		// czas do osi¹gniêcia pocz¹tkowej pozycji
		// otrzymujemy równanie kwadratowe 
		// gx^2 - 2*v*t - 2*y0 = 0
		delta = std::pow(2 * V, 2) + 4 * G_const * 2 * startingYPos;
		timeToReachStartingPos = ((2 * V) + sqrt(delta)) / (2 * G_const);

		delta = std::pow(2 * V / scale, 2) + 8 * G_const * startingYPos / scale;
		this->scaledTimeToReachStartingPos = ((2 * V / scale) + sqrt(delta)) / (2 * G_const);

		break;
	case typesOfThrow::HORIZONTALTHROW:
		// zasiêg  : Z = V * sqrt(2*H/G)
		range = startingXPos + V * sqrt(2 * startingYPos / G_const);
		scaledRange = startingXPos / scale + scaledV * sqrt(2 * startingYPos / scale / G_const);

		// maksymalna wysokoœæ : wysokoœæ pocz¹tkowa
		maxHeight = startingYPos;
		scaledHmax = startingYPos / scale;

		// czas do osi¹gniêcia maksymalnej wysokoœci = 0 (thx cpt. Obvious)
		timeToReachMaxHeight = 0;

		// czas do osi¹gniêcia y = 0 : Ts = sqrt(2*H/G)
		timeToReachStartingPos = sqrt(2 * startingYPos * scale / G_const);
		scaledTimeToReachStartingPos = sqrt(2 * startingYPos * scale / G_const);

		break;
	case typesOfThrow::CROSSWISETHROW:
		// zasiêg : Z = (V^2) /G_const * sin 2a  
		range = startingXPos + (V*V) / G_const * sin(2 * angle * M_PI / 180.0);
		scaledRange = startingXPos / scale + (scaledV*scaledV) / G_const * sin(2 * angle * M_PI / 180.0);

		// maksymalna wysokoœæ : Hmax = V0^2 * sinA^2/2G
		maxHeight = startingYPos + V*V * sinA*sinA / (2 * G_const);
		scaledHmax = startingYPos / scale + scaledV*scaledV * sinA * sinA / (2 * G_const);

		// czas do osi¹gniêcia startowej pozycji : Ts = 2V * sinA/ G_const
		timeToReachStartingPos = 2 * V * sinA / G_const;
		scaledTimeToReachStartingPos = 2 * scaledV * sinA / G_const;

		// czas do osi¹gniêcia maksymalnej wysokoœci 
		timeToReachMaxHeight = V * sinA / G_const;
		scaledTimeToReachMaxHeight = scaledV * sinA / G_const;
		break;
	}
}
void   SFMLApplication::displayAnimateThrow()
{
	drawLines(true);

	// rysowanie "kroków"
	for (int i = 0; i < road.size(); ++i)
		window.draw(road[i]);

	// rysowanie linii
	window.draw(linesToCenterOfObject);
}
void   SFMLApplication::manageView()
{
	// sprawdzamy czy nasz obiekt wymaga przesuniêcia (tak aby nasz obiekt by³ gdzieœ w œrodku)
	// i je¿eli tak jest to przeuwamy nasz obiekt i teksty o ró¿nicê aktualnej pozycji od pozycji klatkê wczeœniej

	if (object.getPosition().x > (SFMLWINDOWWIDTH - object.getCircle().getRadius()) / 2 + xViewMove)
	{
		double incrementalX = V * (GETSEC(timeElapsed) + timeElapsedFromLastPause) * cosA - V *
			((GETSEC(timeElapsed) + timeElapsedFromLastPause) - 1.0 / FPS)* cosA;;

		// ruszamy naszym widokiem w prawo
		DynamicView.move(incrementalX, 0);
		xViewMove += incrementalX;
	}
	else
	{
		double incrementalX = V * (GETSEC(timeElapsed) + timeElapsedFromLastPause) *
			cosA - V * ((GETSEC(timeElapsed) + timeElapsedFromLastPause) - 1.0 / FPS) * cosA;

		DynamicView.move(-incrementalX, 0);
		xViewMove -= incrementalX;
	}
	if (object.getPosition().y < (SFMLWINDOWHEIGHT - 2 * object.getCircle().getRadius()) / 3 + yViewMove)
	{
		double incrementalY = object.getPosition().y - prevPosition.y;
		yViewMove += incrementalY;

		DynamicView.move(0, incrementalY);
	}
	if (object.getPosition().y >(SFMLWINDOWHEIGHT - 2 * object.getCircle().getRadius()) + yViewMove)
	{
		double incrementalY = object.getPosition().y - prevPosition.y;
		yViewMove += incrementalY;

		DynamicView.move(0, incrementalY);
	}

	window.setView(DynamicView);
}
void   SFMLApplication::handleSelectingPoints()
{
	// wyjaœnienia : sprawdzamy czy dotkneliœmy kó³ka i je¿eli tak to 
	// "migrujemy" go z jednego wektora do drugiego (tyle ¿e w drugi wektor 
	// jest kontenerem obiektów klasy dziedziczonej i zawiera jescze kilka 
	// waznych informacji które w 1 wekotrze nie mia³by ¿adnego sensu). I 
	// odwrotnie potem bedziemy sprawdzaæ czy nie klikneliœmy kó³ka z drugiego wektora.
	// great plan ? great plan !

	// najpierw interakcja ze zwyk³ymi krokami (czerwony)
	int counter = 0;
	for (std::vector<circleWithParameters>::iterator iter = road.begin(); iter != road.end(); ++iter)
	{
		// je¿eli wyst¹pi³a interakcja to chcemy zmieniæ kolor kó³ka 
		// i sprawdziæ czy naciœniêto LPM
		if (mouseIntersectWithGivenCircle(road[counter]) && road[counter].getFillColor() != selectedPointsColor)
		{
			road[counter].setFillColor(highlightedPointColor);
			//												mo¿na dodawaæ punkty co 2/10 sekundy
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				// tworzymy nasz specjany punkt który bêdzie zawiera³ jeszcze kilka przydatnych informacji
				selectedCircle.create(road[counter], iter, counter, highlightedPointColor, this->scale);

				// dodajmy punkt do vektora, usuwamy z wektora punkt ( chocia¿ bardziej jest odpowiedne ¿e przenieœliœmy
				// punkt do innego wektora)
				selectedPoints.push_back(selectedCircle);
				road[counter].setFillColor(selectedPointsColor);

				// dodajemy 1 do iloœci zaznaczonych punktów, update'ujemy tekst oraz odczekujemy chwilê by dodaæ tylko jeden punkt
				howManySelectedPoints++;
				animateThrowTexts[7].setString("Zaznaczono " + std::to_string(howManySelectedPoints) + "pkt.");
				Sleep(200);
				break;
			}
		}
		else if (road[counter].getFillColor() == highlightedPointColor)
		{
			if (indexOfMaxHeight && counter == indexOfMaxHeight)
			{
				road[counter].setFillColor(maxHeightColor);
				continue;
			}
			road[counter].setFillColor(defaultColor);
		}
		counter++;
	}
}
void   SFMLApplication::manageZoom()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
	{
		DynamicView.zoom(zoomBy);
		zoomedBy /= zoomBy;

		xChangeByZooming = (DynamicView.getSize().x - staticView.getSize().x);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
	{
		DynamicView.zoom(1 / zoomBy);
		zoomedBy *= zoomBy;
	}

	window.setView(DynamicView);

}
void   SFMLApplication::drawAnimateThrowTexts()
{
	windowWithTexts.clear();
	// rysowanie tekstów
	for (int i = 0; i < animateThrowTexts.size(); ++i)
		windowWithTexts.draw(animateThrowTexts[i]);

	// rysowanie "GUI"
	for (int i = 0; i < animateThrowGUI.size(); ++i)
		windowWithTexts.draw(animateThrowGUI[i]);

	// rysowanie równania toru rzutu ukoœnego
	for (int i = 0; i < CrosswiseEquation.size(); ++i)
		windowWithTexts.draw(CrosswiseEquation[i]);

	windowWithTexts.display();
}
void   SFMLApplication::handleTimeManagmentWhenPaused()
{
	if (animationPaused)
		timeElapsed.restart();
	else
	{
		timeElapsedFromLastPause += GETSEC(timeElapsed);
		timeElapsed.restart();
	}

	animationPaused = !(animationPaused);
}
void   SFMLApplication::manageMovingOfProjection()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		xViewMove -= moveScreenBy;
		DynamicView.move(-moveScreenBy, 0);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		xViewMove += moveScreenBy;
		DynamicView.move(moveScreenBy, 0);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		yViewMove -= moveScreenBy;
		DynamicView.move(0, -moveScreenBy);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		yViewMove += moveScreenBy;
		DynamicView.move(0, moveScreenBy);
	}

}
void   SFMLApplication::handleAnimateThrowTexts()
{
	scaledPosX = calcluateWidth((GETSEC(timeElapsed) + timeElapsedFromLastPause), scale, false);
	scaledPosY = calculateHeight((GETSEC(timeElapsed) + timeElapsedFromLastPause), scale, false);

	animateThrowTexts[0].setString("Pozycja X = " + std::to_string(scaledPosX / scale) + " m");
	animateThrowTexts[1].setString("Pozycja Y = " + std::to_string(scaledPosY) + " m");
	animateThrowTexts[2].setString("Czas = " + std::to_string((GETSEC(timeElapsed) + timeElapsedFromLastPause)) + " s");
	animateThrowTexts[3].setString("Zasiêg = " + std::to_string(scaledRange) + " m");
	animateThrowTexts[4].setString("Hmax =  " + std::to_string(scaledHmax) + " m");
	animateThrowTexts[5].setString("Tw  = " + std::to_string(timeToReachMaxHeight   / scale) + " s");
	animateThrowTexts[6].setString("Ts  = " + std::to_string(timeToReachStartingPos / scale) + " s");


	double x = this->V/scale * (GETSEC(timeElapsed) + timeElapsedFromLastPause) * this->cosA;
	valOfEquation = x * (sinA / cosA) - G_const / (2.0 * this->Vx/scale*this->Vx/scale) * x * x;

	CrosswiseEquation[3].setString("                                                                                     " + std::to_string(G_const));
	CrosswiseEquation[4].setString(std::to_string(valOfEquation) + " = " + std::to_string(width) + " * " +
		std::to_string(sinA / cosA) + " - " + "  ----------------------- " + std::to_string(width*width));
	CrosswiseEquation[5].setString("                                                                           2 * " + std::to_string(V*V) + " * " + std::to_string(cosA*cosA));
}
double SFMLApplication::calculateHeight(double timeElapsed, double scale, bool inPixels)
{
	if (inPixels)
	{

		return startingYPos + V * timeElapsed * sinA - (G_const * scale / 2 * pow(timeElapsed, 2));
	}

	// y = V *         t           * sinA - G/2 * t^2
	return startingYPos / scale + V / scale *timeElapsed * sinA - (G_const / 2 * pow(timeElapsed, 2));
}
double SFMLApplication::calcluateWidth(double timeElapsed, double scale, bool inPixels)
{
	//if (inPixels)
	//{
	//	 return startingXPos + V/ scale * timeElapsed * cosA;
	//}

	// x = V0  * t * cosA
	return startingXPos/scale + V * timeElapsed * cosA;
}
void SFMLApplication::printAllValuesIntoFile()
{
	fileWithValues.open("Wszystkie_wartoœci.txt", std::ios::in | std::ios::out | std::ios::trunc);
	try
	{
		if (!fileWithValues.good())
		{
			throw(exceptionWithMsg("Wyst¹pi³ b³¹d podczas otwierania pliku!", exceptionList::ERROR_DURING_OPENING_FILE));
		}

		fileWithValues << "V startowe : " << this->V << "\nK¹t alfa: " << this->angle << "\nSin alfa:" << this->sinA << "\nCos alfa:" << this->cosA<<"\n\n";

		double _Vx = this->V * this->cosA;
		float t = 0;
			while (t < this->timeToReachStartingPos/ scale)
			{
				double _V, _Vy, _width, _height, _valOfEquation;
				_Vy = this->V * this->sinA - G_const *t;
				_V = sqrt(_Vx*_Vx + _Vy*_Vy);
				_width =  this->V  * cosA * t;
				_height = this->V * t * sinA - G_const / 2 * t*t;
				_valOfEquation = _width * this->sinA / this->cosA - G_const / (2 * this->V * this->V * this->cosA * this->cosA) * _width * _width;
				fileWithValues << "************************************************************\n";
				fileWithValues << "*Czas : " << t << " s \n";
				fileWithValues << "Prêdkoœæ pozioma Vx : " << _Vx << "\n";
				fileWithValues << "Prêdkoœæ pionowa Vy : " << _Vy << "\n";
				fileWithValues << "Prêdkoœæ V  : " << _V << "\n";
				fileWithValues << "pozycja X : " << _width << "\n";
				fileWithValues << "pozycja Y : " << _height << "\n";
				fileWithValues << "K¹t  : " << acos(_Vx / _V) * 180 / M_PI << "\n";
				fileWithValues << "Równanie toru ruchu : " << _valOfEquation << "\n";
				fileWithValues << "************************************************************\n\n";

				t += 0.01;
			}
	}
	catch (exceptionWithMsg exception)
	{
		handleExceptions(exception);
	}
	fileWithValues.close();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//			SHOWSELECTEDPOINTS
void   SFMLApplication::handleShowSelectedPoints()
{
	windowWithTexts.clear();

	drawLines(false);

	if (showRoad)
	{
		for (int i = 0; i < road.size(); ++i)
			window.draw(road[i]);
	}


	showPointByPoint();
	

	// rysujemy parametry 
	for (int i = 0; i < showSelectedPointsParams.size(); ++i)
		windowWithTexts.draw(showSelectedPointsParams[i]);

	windowWithTexts.display();
}
void	SFMLApplication::showPointByPoint()
{
	if (selectedPoints.size())
	{
		if (indexOfCurrentlyInspectedPoint < 0 || indexOfCurrentlyInspectedPoint > selectedPoints.size())
			indexOfCurrentlyInspectedPoint = 0;

		// metoda która bêdzie rysowaæ nasze punkty
		drawPoints();
		managePointPosition();

		// zarz¹dzanie inputem z klawiatury 
		if (GETSEC(delayBtwChangingPoints) > 0.1f)
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)
				&& indexOfCurrentlyInspectedPoint > 0)
			{
				indexOfCurrentlyInspectedPoint--;
				setupShowSelectedPointsTexts();
				delayBtwChangingPoints.restart();
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)
				&& indexOfCurrentlyInspectedPoint < selectedPoints.size() - 1)
			{
				indexOfCurrentlyInspectedPoint++;
				setupShowSelectedPointsTexts();
				delayBtwChangingPoints.restart();
			}

			// poruszanie widokiem
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			{
				xViewMove -= moveScreenBy;
				DynamicView.move(-moveScreenBy, 0);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			{
				xViewMove += moveScreenBy;
				DynamicView.move(moveScreenBy, 0);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			{
				yViewMove -= moveScreenBy;
				DynamicView.move(0, -moveScreenBy);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			{
				yViewMove += moveScreenBy;
				DynamicView.move(0, moveScreenBy);
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::L))
			{
				showLines = !showLines;
			}
			delayBtwChangingPoints.restart();
		}

		// usuwamy punkty
		manageDeletingPoints();
	}
}
void	SFMLApplication::managePointPosition()
{
	// sprawdzamy czy nasz obiekt wymaga przesuniêcia (tak aby nasz obiekt by³ gdzieœ w œrodku)
	// i je¿eli tak jest to przeuwamy nasz obiekt i teksty o ró¿nicê aktualnej pozycji od pozycji klatkê wczeœniej

	if (selectedPoints[indexOfCurrentlyInspectedPoint].getPosition().x > (SFMLWINDOWWIDTH - selectedPoints[indexOfCurrentlyInspectedPoint].getRadius()) / 2 + xViewMove)
	{
		double incrementalX = SFMLWINDOWWIDTH / selectedPoints[indexOfCurrentlyInspectedPoint].getRadius();

		// ruszamy naszym widokiem w prawo
		DynamicView.move(incrementalX, 0);
		xViewMove += incrementalX;
	}
	else if (selectedPoints[indexOfCurrentlyInspectedPoint].getPosition().x < (SFMLWINDOWWIDTH - selectedPoints[indexOfCurrentlyInspectedPoint].getRadius()) / 4 + xViewMove)
	{
		double incrementalX = SFMLWINDOWWIDTH / (selectedPoints[indexOfCurrentlyInspectedPoint].getRadius() * 2);

		DynamicView.move(-incrementalX, 0);
		xViewMove -= incrementalX;
	}
	if (selectedPoints[indexOfCurrentlyInspectedPoint].getPosition().y >(SFMLWINDOWHEIGHT - 2 * selectedPoints[indexOfCurrentlyInspectedPoint].getRadius()) / 1.2 + yViewMove)
	{
		double incrementalY = SFMLWINDOWHEIGHT / (selectedPoints[indexOfCurrentlyInspectedPoint].getRadius() * 2);
		yViewMove += incrementalY;

		DynamicView.move(0, incrementalY);
	}
	else if (selectedPoints[indexOfCurrentlyInspectedPoint].getPosition().y < (SFMLWINDOWHEIGHT - 2 * selectedPoints[indexOfCurrentlyInspectedPoint].getRadius()) / 3 + yViewMove)
	{
		double incrementalY = -SFMLWINDOWHEIGHT / (selectedPoints[indexOfCurrentlyInspectedPoint].getRadius() * 2);
		yViewMove += incrementalY;

		DynamicView.move(0, incrementalY);
	}


	window.setView(DynamicView);
}
void	SFMLApplication::setupShowSelectedPointsTexts()
{
	if (selectedPoints.size() > 0)
	{
		text.setCharacterSize(24);
		showSelectedPointsVectorTexts.clear();
		// zabawa kolorkami i pozycj¹
		text.setFillColor(VxColor);
		text.setPosition(selectedPoints[indexOfCurrentlyInspectedPoint].getPosition().x + selectedPoints[indexOfCurrentlyInspectedPoint].vectorX.getSize().x / 2,
			selectedPoints[indexOfCurrentlyInspectedPoint].getPosition().y);
		text.setString("Vx");
		showSelectedPointsVectorTexts.push_back(text);

		text.setFillColor(VyColor);
		text.setPosition(selectedPoints[indexOfCurrentlyInspectedPoint].getPosition().x - 12,
			selectedPoints[indexOfCurrentlyInspectedPoint].getPosition().y + selectedPoints[indexOfCurrentlyInspectedPoint].vectorY.getSize().y / 2 - 12);
		text.setString("Vy");
		showSelectedPointsVectorTexts.push_back(text);

		text.setFillColor(VColor);
		text.setPosition(selectedPoints[indexOfCurrentlyInspectedPoint].getPosition().x + selectedPoints[indexOfCurrentlyInspectedPoint].vectorX.getSize().x / 2 - 12,
			selectedPoints[indexOfCurrentlyInspectedPoint].getPosition().y + selectedPoints[indexOfCurrentlyInspectedPoint].vectorY.getSize().y / 2 + 12);
		text.setString("V");
		showSelectedPointsVectorTexts.push_back(text);

	}
	// ustawienie tekstów które bêd¹ zawsze 
	if (indexOfCurrentlyInspectedPoint >= 0 && indexOfCurrentlyInspectedPoint < selectedPoints.size() && selectedPoints.size())
	{
		showSelectedPointsParams[0].setFillColor(this->VxColor);
		showSelectedPointsParams[1].setFillColor(this->VyColor);
		showSelectedPointsParams[2].setFillColor(this->VColor);
		showSelectedPointsParams[0].setString("Vx = " + std::to_string(selectedPoints[indexOfCurrentlyInspectedPoint].Xsize / scale) + " m");
		showSelectedPointsParams[1].setString("Vy = " + std::to_string(selectedPoints[indexOfCurrentlyInspectedPoint].Ysize / this->scale ) + " m");
		showSelectedPointsParams[2].setString("V  = " + std::to_string(selectedPoints[indexOfCurrentlyInspectedPoint].Vsize / scale) + " m");
		showSelectedPointsParams[3].setString("T  = " + std::to_string(selectedPoints[indexOfCurrentlyInspectedPoint].getTime()) + " s");
		showSelectedPointsParams[4].setString("Pos X  = " + std::to_string(selectedPoints[indexOfCurrentlyInspectedPoint].getCalculatedPosition().x) + " m");
		showSelectedPointsParams[5].setString("Pos Y  = " + std::to_string(selectedPoints[indexOfCurrentlyInspectedPoint].getCalculatedPosition().y) + " m");
		showSelectedPointsParams[6].setString("Kat  = " + std::to_string(-selectedPoints[indexOfCurrentlyInspectedPoint].angleB) + "*");
		showSelectedPointsParams[8].setString("Punkt " + std::to_string(indexOfCurrentlyInspectedPoint + 1) + " / " + std::to_string(selectedPoints.size()));
	}
}
void	SFMLApplication::manageDeletingPoints()
{
	int modify = 1;
	if (mouseIntersectWithGivenCircle(selectedPoints[indexOfCurrentlyInspectedPoint]) && GETSEC(delayBtwChangingPoints) > 0.2f)
	{
		selectedPoints[indexOfCurrentlyInspectedPoint].setFillColor(highlighted);

		if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && selectedPoints.size())
		{
			bool canErase = false;

			if (indexOfCurrentlyInspectedPoint >= 0)
			{
				modify = -1;
				canErase = 1;
			}
			else if (indexOfCurrentlyInspectedPoint < selectedPoints.size() - 2)
			{
				canErase = 1;
			}

			if (canErase)
			{
				circleWithParameters circle = selectedPoints[indexOfCurrentlyInspectedPoint].getCircle();
				std::vector<showedCircleWithParameters>::iterator iter;
				iter = selectedPoints.begin();

				iter += indexOfCurrentlyInspectedPoint;


				road[selectedPoints[indexOfCurrentlyInspectedPoint].getIntIndex()].setFillColor(defaultColor);
				selectedPoints.erase(iter);
				indexOfCurrentlyInspectedPoint += modify;

				setupShowSelectedPointsTexts();
			}


			delayBtwChangingPoints.restart();

		}
	}
	else if (selectedPoints[indexOfCurrentlyInspectedPoint].getFillColor() == highlighted)
	{
		selectedPoints[indexOfCurrentlyInspectedPoint].setFillColor(highlightedPointColor);
	}
}
void	SFMLApplication::drawPoints()
{
	if (selectedPoints.size())
	{
		// pokazujemy tylko 1 punkt
		if (!showAllPoints)
		{
			// rysujemy dany punkt
			window.draw(selectedPoints[indexOfCurrentlyInspectedPoint]);
			// pokazujemy wektory sk³adowe danego punktu jeœli trzeba 
			if (showVectors)
			{
				selectedPoints[indexOfCurrentlyInspectedPoint].update(window);
				for (int i = 0; i < showSelectedPointsVectorTexts.size(); ++i)
				{
					window.draw(showSelectedPointsVectorTexts[i]);
				}
			}
		}
		// rysujemy wszystkie punkty
		else
		{
			for (int i = 0; i < selectedPoints.size(); ++i)
			{
				window.draw(selectedPoints[i]);
			}


			// pokazujemy wektory je¿eli jest taka potrzeba 
			if (showVectors)
			{
				for (int i = 0; i < selectedPoints.size(); ++i)
				{
					selectedPoints[i].update(window);
				}
				for (int i = 0; i < showSelectedPointsVectorTexts.size(); ++i)
				{
					window.draw(showSelectedPointsVectorTexts[i]);
				}
			}
		}
	}
}
void	SFMLApplication::drawLines(bool needToDrawObj)
{
	if (showLines)
	{
		// rysowanie linii
		for (int i = 0; i < squareBoardLinesX.size(); ++i)
			window.draw(squareBoardLinesX[i]);
		for (int i = 0; i < squareBoardLinesY.size(); ++i)
			window.draw(squareBoardLinesY[i]);
	}
		if (needToDrawObj)
		{
			window.draw(this->object.getCircle());
		}
		if (showLines)
		{

		for (int i = 0; i < squareBoardTextsX.size(); ++i)
			window.draw(squareBoardTextsX[i]);
		for (int i = 0; i < squareBoardTextsY.size(); ++i)
			window.draw(squareBoardTextsY[i]);
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
