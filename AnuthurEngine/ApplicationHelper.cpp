//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#include "ApplicationHelper.h"
static UINT ToUINT() {
	return 0U;
}

template<typename T, typename ... Types>
static UINT ToUINT(T t, const Types& ... types) {
	return static_cast<UINT>(t) | ToUINT(types...);
}

static DWORD ToDword() {
	return 0;
}

template<typename T, typename ... Types>
static DWORD ToDword(T t, const Types& ... types) {
	return static_cast<DWORD>(t) | ToDword(types...);
}

Luxko::Application::WindowClassStyle Luxko::Application::operator|(WindowClassStyle a, WindowClassStyle b)
{
	return static_cast<WindowClassStyle>(ToUINT(a, b));
}

Luxko::Application::WindowClassStyle Luxko::Application::operator&(WindowClassStyle a, WindowClassStyle b)
{
	return static_cast<WindowClassStyle>(ToUINT(a)&ToUINT(b));
}

Luxko::Application::WindowStyle Luxko::Application::operator|(WindowStyle a, WindowStyle b)
{
	return static_cast<WindowStyle>(ToDword(a, b));
}

Luxko::Application::WindowStyle Luxko::Application::operator&(WindowStyle a, WindowStyle b)
{
	return static_cast<WindowStyle>(ToDword(a)&ToDword(b));
}

Luxko::Application::ExtendedWindowStyle Luxko::Application::operator|(ExtendedWindowStyle a, ExtendedWindowStyle b)
{
	return static_cast<ExtendedWindowStyle>(ToDword(a, b));
}

Luxko::Application::ExtendedWindowStyle Luxko::Application::operator&(ExtendedWindowStyle a, ExtendedWindowStyle b)
{
	return static_cast<ExtendedWindowStyle>(ToDword(a)&ToDword(b));
}

Luxko::Application::WindowIcon Luxko::Application::WindowIcon::LoadFromFile(const std::wstring& fileName, int desiredWidth /*= 0*/, int desiredHeight /*= 0*/, bool useSystemSizeAsDefault /*= true*/)
{
	WindowIcon wi;
	UINT fuLoad = LR_DEFAULTCOLOR | LR_LOADFROMFILE;
	if (useSystemSizeAsDefault) {
		fuLoad = fuLoad | LR_DEFAULTSIZE;
	}

	wi._hIcon = reinterpret_cast<HICON>(LoadImage(NULL, fileName.c_str(), IMAGE_ICON, desiredWidth, desiredHeight, fuLoad));
	if (wi._hIcon == NULL) {
		wi._isValid = false;
	}
	else {
		wi._isValid = true;
	}
	return std::move(wi);
}

Luxko::Application::WindowIcon Luxko::Application::WindowIcon::LoadFromModule(HINSTANCE hModule, const std::wstring& name, int desiredWidth /*= 0*/, int desiredHeight /*= 0*/, bool useSystemSizeAsDefault /*= true*/)
{
	WindowIcon wi;
	UINT fuLoad = LR_DEFAULTCOLOR;
	if (useSystemSizeAsDefault) {
		fuLoad = fuLoad | LR_DEFAULTSIZE;
	}

	wi._hIcon = reinterpret_cast<HICON>(LoadImage(hModule, name.c_str(), IMAGE_ICON, desiredWidth, desiredHeight, fuLoad));
	if (wi._hIcon == NULL) {
		wi._isValid = false;
	}
	else {
		wi._isValid = true;
	}
	return std::move(wi);
}

void Luxko::Application::WindowIcon::Destroy()
{
	if (_isValid) {
		DestroyIcon(_hIcon);
		_isValid = false;
	}
}


Luxko::Application::WindowIcon::WindowIcon(WindowIcon&& w)
{
	_isValid = w._isValid;
	_hIcon = w._hIcon;
	w._isValid = false;

}

Luxko::Application::WindowIcon::~WindowIcon()
{
	Destroy();
}

HICON Luxko::Application::WindowIcon::Free()
{
	_isValid = false;
	return _hIcon;
}

Luxko::Application::WindowIcon& Luxko::Application::WindowIcon::operator=(WindowIcon&& w)
{
	Destroy();
	_isValid = w._isValid;
	_hIcon = w._hIcon;
	w._isValid = false;
	return *this;
}

Luxko::Application::WindowCursor Luxko::Application::WindowCursor::LoadFromFile(const std::wstring& fileName, int desiredWidth /*= 0*/, int desiredHeight /*= 0*/, bool useSystemSizeAsDefault /*= true*/)
{
	WindowCursor wi;
	wi._isSystemResource = false;
	UINT fuLoad = LR_DEFAULTCOLOR | LR_LOADFROMFILE;
	if (useSystemSizeAsDefault) {
		fuLoad = fuLoad | LR_DEFAULTSIZE;
	}

	wi._hCursor = reinterpret_cast<HCURSOR>(LoadImage(NULL, fileName.c_str(), IMAGE_CURSOR, desiredWidth, desiredHeight, fuLoad));
	if (wi._hCursor == NULL) {
		wi._isValid = false;
	}
	else {
		wi._isValid = true;
	}
	return std::move(wi);
}

Luxko::Application::WindowCursor Luxko::Application::WindowCursor::LoadFromModule(HINSTANCE hModule, const std::wstring& name, int desiredWidth /*= 0*/, int desiredHeight /*= 0*/, bool useSystemSizeAsDefault /*= true*/)
{
	WindowCursor wi;
	wi._isSystemResource = false;
	UINT fuLoad = LR_DEFAULTCOLOR;
	if (useSystemSizeAsDefault) {
		fuLoad = fuLoad | LR_DEFAULTSIZE;
	}

	wi._hCursor = reinterpret_cast<HCURSOR>(LoadImage(hModule, name.c_str(), IMAGE_CURSOR, desiredWidth, desiredHeight, fuLoad));
	if (wi._hCursor == NULL) {
		wi._isValid = false;
	}
	else {
		wi._isValid = true;
	}
	return std::move(wi);
}

Luxko::Application::WindowCursor Luxko::Application::WindowCursor::LoadFromSystem(SystemCursor sc)
{
	WindowCursor wi;
	wi._isSystemResource = true;
	UINT fuLoad = LR_DEFAULTCOLOR|LR_SHARED;
	auto scInt = static_cast<int>(sc);
	wi._hCursor = reinterpret_cast<HCURSOR>(LoadImage(NULL, MAKEINTRESOURCE(scInt) , IMAGE_CURSOR, 0,0, fuLoad));
	if (wi._hCursor == NULL) {
		wi._isValid = false;
	}
	else {
		wi._isValid = true;
	}
	return std::move(wi);
}

void Luxko::Application::WindowCursor::Destroy()
{
	if (!_isSystemResource && _isValid) {
		DestroyCursor(_hCursor);
	}
	_isValid = false;
}

Luxko::Application::WindowCursor::WindowCursor(WindowCursor&& w)
{
	_isValid = w._isValid;
	_isSystemResource = w._isSystemResource;
	_hCursor = w._hCursor;
	w._isValid = false;
}

Luxko::Application::WindowCursor::~WindowCursor()
{
	Destroy();
}

HCURSOR Luxko::Application::WindowCursor::Free()
{
	_isValid = false;
	return _hCursor;
}

Luxko::Application::WindowCursor& Luxko::Application::WindowCursor::operator=(WindowCursor&& w)
{
	Destroy();
	_isValid = w._isValid;
	_isSystemResource = w._isSystemResource;
	_hCursor = w._hCursor;
	w._isValid = false;
	return *this;
}

Luxko::Application::WindowClassDescriptor::WindowClassDescriptor(const std::wstring& className, WindowClassStyle style, HCURSOR cursor, HICON icon /*= NULL*/)
	:m_className(className),m_style(style),m_hIcon(icon),m_hCursor(cursor)
{

}

WNDCLASSEX Luxko::Application::WindowClassDescriptor::GenerateWNDCLASSEX(WNDPROC lpfnWndProc, HINSTANCE hInstance) const
{
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.hCursor = m_hCursor;
	wcex.hIcon = m_hIcon;
	wcex.style = ToUINT(m_style);
	wcex.lpszClassName = m_className.c_str();
	wcex.lpfnWndProc = lpfnWndProc;
	wcex.hInstance = hInstance;
	return wcex;
}

Luxko::Application::WNDC_ID Luxko::Application::WindowClassDescriptor::Register(WNDPROC lpfnWndProc, HINSTANCE hInstance) const
{
	auto wcex = GenerateWNDCLASSEX(lpfnWndProc, hInstance);
	return RegisterClassEx(&wcex);
}

Luxko::Application::WindowDescriptor::WindowDescriptor(WindowStyle style, ExtendedWindowStyle extendedStyle, const RECT& rect)
	:m_style(style),m_extendedStyle(extendedStyle),m_rect(rect)
{

}

Luxko::Application::WindowDescriptor::WindowDescriptor(WindowStyle style, ExtendedWindowStyle extendedStyle, LONG left, LONG top, LONG right, LONG bottom)
	:m_style(style),m_extendedStyle(extendedStyle),m_left(left),m_right(right),m_top(top),m_bottom(bottom)
{

}

HWND Luxko::Application::WindowDescriptor::GenerateWindowByClassName(HINSTANCE hInstance, std::wstring& windowClassName, std::wstring& windowTitle, bool useSystemDefaultPosition/*=true*/) const
{
	RECT temp = m_rect;
	DWORD style = ToDword(m_style);
	DWORD exStyle = ToDword(m_extendedStyle);
	if (AdjustWindowRectEx(&temp, style, FALSE, exStyle) == 0) {
		throw  "WTF? Even this can go wrong?";
	}
	int x, y;
	if (useSystemDefaultPosition) {
		x = y = CW_USEDEFAULT;
	}
	else {
		x = temp.left;
		y = temp.top;
	}
	return CreateWindowEx(exStyle, windowClassName.c_str(), windowTitle.c_str(), style, x, y,
		temp.right - temp.left, temp.bottom - temp.top, NULL, NULL, hInstance, NULL);
}

HWND Luxko::Application::WindowDescriptor::GenerateWindowByATOM(HINSTANCE hInstance, ATOM atom, std::wstring& windowTitle, bool useSystemDefaultPosition /*= true*/) const
{
	RECT temp = m_rect;
	DWORD style = ToDword(m_style);
	DWORD exStyle = ToDword(m_extendedStyle);
	if (AdjustWindowRectEx(&temp, style, FALSE, exStyle) == 0) {
		throw "WTF? Even this can go wrong?";
	}
	int x, y;
	if (useSystemDefaultPosition) {
		x = y = CW_USEDEFAULT;
	}
	else {
		x = temp.left;
		y = temp.top;
	}
	return CreateWindowEx(exStyle, MAKEINTRESOURCE(atom), windowTitle.c_str(), style, x, y,
		temp.right - temp.left, temp.bottom - temp.top, NULL, NULL, hInstance, NULL);
}
