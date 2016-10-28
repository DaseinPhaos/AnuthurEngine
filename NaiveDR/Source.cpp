#include "NaiveDR.h"

int WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	NaiveApp app(1600, 900, L"Ass we can");
	app.Run(hInstance, nShowCmd);
}