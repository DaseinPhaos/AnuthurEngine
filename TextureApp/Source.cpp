#include "Crate.h"

int _stdcall WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	CrateApp crateApp(1600, 900, L"crate");
	return crateApp.Run(hInstance, nShowCmd);
}