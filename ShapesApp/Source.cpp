#include "ShapeApp.h"

int _stdcall WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
	ShapeApp sa(1600, 900, L"fuck shape");
	auto t = sizeof(Vector4f);
	auto bl = sizeof(LightPack::BasicLight);
	auto lp = sizeof(LightPack);
	return sa.Run(hInstance, nShowCmd);
}