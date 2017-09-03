#include "windowsApplication.h"



void windowsApplication::Initialize(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lCmdLine, int cmdShow)
{
	className = "myClass";


	this->hInstance		=	 hInstance;
	this->hPrevInstance =	 hPrevInstance;
	this->lCmdLine		=	 lCmdLine;
	this->cmdShow		=	 cmdShow;


	// https://msdn.microsoft.com/pl-pl/library/windows/desktop/ms633576(v=vs.85).aspx

	wc.cbSize			=	 sizeof(WNDCLASSEX);
	wc.style			=	 CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		=	 WindowProc;
	wc.cbClsExtra		=	 NULL;
	wc.cbWndExtra		=	 NULL;
	wc.hInstance		=	 hInstance;
	wc.hIcon			=	 LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wc.hCursor			=	 LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	=	 (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName		=	 NULL;
	wc.lpszClassName	=	 className;
	wc.hIconSm			=	 LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));




	try
	{
		if (!RegisterClassEx(&wc))
			throw exceptionWithMsg("Szanowny Mr. Windows odrzuci³ nasz wniosek o rejstracjê klasy windowsowej",exceptionList::ERROR_DURING_CLASS_INITIALIZATION);

	//	https://msdn.microsoft.com/pl-pl/library/windows/desktop/ms632679(v=vs.85).aspx
		hWnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		className,
		"Symulacja",
		WS_SYSMENU | WS_VISIBLE,
		0,0,
		WINDOWWIDTH,WINDOWHEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

		if (!hWnd)
		{
			throw exceptionWithMsg("Niestety szanowny pan W. odrzuci³ nasz¹ proœbê o utworzenie okienka :/", exceptionList::ERROR_DURING_CREATING_HWND);
		}

		View = CreateWindow(
			"STATIC",
			NULL,
			WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
			XMARGIN, YMARGINUP + GUIHEIGHT,
			SFMLWINDOWWIDTH, SFMLWINDOWHEIGHT,
			hWnd,
			NULL,
			hInstance,
			NULL);
		if (!View)
			throw exceptionWithMsg("Niestety szanowny pan W. odrzuci³ podanie o utworzenie drugiego SFML'owego Okienka :/ ",
									exceptionList::ERROR_DURING_CREATING_HWND);
		
			HWNDWithParameters = CreateWindow(
			"STATIC",
			NULL,
			WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
			XMARGIN, YMARGINUP,
			SFMLWINDOWWIDTH, GUIHEIGHT,
			hWnd,
			NULL,
			hInstance,
			NULL);
		if (!HWNDWithParameters)
			throw exceptionWithMsg("Niestety nie uda³o siê utworzyæ drugiego widoku(parametry) :/",
				exceptionList::ERROR_DURING_CREATING_HWND);



	}
	catch (exceptionWithMsg exc)
	{
		MessageBox(NULL, NULL, NULL, NULL);
		handleExceptions(exc);
	}

	ShowWindow(hWnd, true);

}
