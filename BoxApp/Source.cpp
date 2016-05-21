#include "BoxApp.h"


int WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
	BoxApp ba(800, 600, L"fuckbox");

	return ba.Run(hInstance, nShowCmd);
}