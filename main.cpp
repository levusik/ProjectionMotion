#include "SFMLApplication.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lCmdLine, int cmdShow)
{
	SFMLApplication app(hInstance,hPrevInstance,lCmdLine,cmdShow);
	app.run();
	return EXIT_SUCCESS;
}