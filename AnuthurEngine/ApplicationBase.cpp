//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#include "ApplicationBase.h"
#include "FileSystem.h"
#include "ApplicationHelper.h"

Luxko::Application::BaseApp::BaseApp(UINT width, UINT height, std::wstring name):_title(name),_width(width),_height(height)
{
	_assetsPath = FileSystem::Directory::GetExePath();
}

Luxko::Application::BaseApp::~BaseApp()
{

}

int Luxko::Application::BaseApp::Run(HINSTANCE hInstance, int nCmdShow)
{
	//**********************************************************************
	// @Luxko: 1stly, register the window class.
	//			This job is delegates to the virtual function RegisterWindowClass.
	auto windowClass = RegisterWindowClass(hInstance);
	if (windowClass == NULL) {
		throw "Window class Registration failed!";
	}
	//**********************************************************************


	//**********************************************************************
	// @Luxko: 2ndly, create a window instance with the help of a WindowDescriptor,
	//			which encapsulates essential details for D3D window creation.
	WindowDescriptor wd(WindowStyle::OverlappedWindow, ExtendedWindowStyle::OverlappedWindow,
		0, 0,_width,_height);
	_hWindow = wd.GenerateWindowByATOM(hInstance, windowClass, _title);
	if (_hWindow == NULL) {
		throw "Window Creation Fucking failed!";
	}
	//**********************************************************************

	//**********************************************************************
	// @Luxko: Now we're ready to show the window.
	ShowWindow(_hWindow, nCmdShow);
	//UpdateWindow(_hWindow);
	//**********************************************************************

	//**********************************************************************
	// @Luxko: Do the initialization necessary for the application.
	OnInit();

	//**********************************************************************

	//**********************************************************************
	// @Luxko: go to the main loop.
	MSG msg = { 0 };
	while (true) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT) {
				break;
			}

			OnEvent(msg);
		}

		OnUpdate();
		OnRender();
	}

	OnDestroy();
	//**********************************************************************

	//**********************************************************************
	// @Luxko: the returned value is the wParam of the last received
	//			WM_QUIT message.
	return static_cast<char>(msg.wParam);
	//**********************************************************************
}

void Luxko::Application::BaseApp::SetCustomWindowText(const std::wstring& text)
{
	std::wstring windowText = _title + L": " + text;
	SetWindowText(_hWindow, windowText.c_str());
}

ATOM Luxko::Application::BaseApp::RegisterWindowClass(HINSTANCE hInstance)
{
	WNDCLASSEX windowClass = { 0 };
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindProc;
	windowClass.hInstance = hInstance;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	windowClass.lpszClassName = (L"WindowClass" + _title).c_str();
	windowClass.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	//windowClass.lpszClassName = L"WindowClass028";
	return RegisterClassEx(&windowClass);
}


std::wstring Luxko::Application::BaseApp::GetAssetFullPath(const std::wstring& assetName) const
{
	return _assetsPath + assetName;
}

LRESULT CALLBACK Luxko::Application::BaseApp::WindProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Handle destroy/shutdown messages.
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	// Handle any messages the switch statement didn't.
	return DefWindowProc(hWnd, message, wParam, lParam);
}
