#pragma once
#include "entity.hpp"


class windowsApplication
{
protected:
	void Initialize(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lCmdLine, int cmdShow);
	friend LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void stateMachine() = 0;
	virtual void handleExceptions(exceptionWithMsg exception) = 0;


	HINSTANCE		hInstance;
	HINSTANCE		hPrevInstance;
	LPSTR			lCmdLine;
	int				cmdShow;
	WNDCLASSEX		wc;
	HWND			hWnd, View,	HWNDWithParameters;


	LPCSTR className;

};