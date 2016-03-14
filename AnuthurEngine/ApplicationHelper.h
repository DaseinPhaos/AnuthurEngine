//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
//
// This header states helper classes and methods which aim at simplifying
// window class/window management relating to a common D3D12 application.
//
//**********************************************************************

#pragma once
#ifdef ANUTHURAPPLICATION_EXPORTS
#define ANUTHURAPPLICATION_API __declspec(dllexport)
#else
#define ANUTHURAPPLICATION_API __declspec(dllimport)
#endif
#include "CommonHeader.h"


namespace Luxko {
	namespace Application {
#pragma region Styles related enumerations
		enum class ANUTHURAPPLICATION_API WindowClassStyle :UINT {
			AlignClientAreaOnByteBoundary = CS_BYTEALIGNCLIENT, // Aligns the window's client area on a byte boundary (in the x direction). This style affects the width of the window and its horizontal placement on the display.
			
			AlignWindowOnByteBoundary = CS_BYTEALIGNWINDOW, // Aligns the window on a byte boundary (in the x direction). This style affects the width of the window and its horizontal placement on the display.
			
			SharedDeviceContext = CS_CLASSDC, // Allocates one device context to be shared by all windows in the class. Because window classes are process specific, it is possible for multiple threads of an application to create a window of the same class. It is also possible for the threads to attempt to use the device context simultaneously. When this happens, the system allows only one thread to successfully finish its drawing operation.

			UniqueDeviceContext = CS_OWNDC, // Allocates a unique device context for each window in the class.

			ReceiveDoubleClicks = CS_DBLCLKS, // Sends a double-click message to the window procedure when the user double-clicks the mouse while the cursor is within a window belonging to the class.

			EnableDropShadow = CS_DROPSHADOW, // Enables the drop shadow effect on a window. The effect is turned on and off through SPI_SETDROPSHADOW. Typically, this is enabled for small, short-lived windows such as menus to emphasize their Z-order relationship to other windows. Windows created from a class with this style must be top-level windows; they may not be child windows.

			RedrawOnHorizontalChange = CS_HREDRAW, // Redraws the entire window if a movement or size adjustment changes the width of the client area.

			RedrawOnVerticalChange = CS_VREDRAW, // Redraws the entire window if a movement or size adjustment changes the height of the client area.

			CacheScreenAreaUnderneath = CS_SAVEBITS // Saves, as a bitmap, the portion of the screen image obscured by a window of this class. When the window is removed, the system uses the saved bitmap to restore the screen image, including other windows that were obscured. Therefore, the system does not send WM_PAINT messages to windows that were obscured if the memory used by the bitmap has not been discarded and if other screen actions have not invalidated the stored image.
		};

		ANUTHURAPPLICATION_API WindowClassStyle operator|(WindowClassStyle a, WindowClassStyle b);
		ANUTHURAPPLICATION_API WindowClassStyle operator&(WindowClassStyle a, WindowClassStyle b);

		enum class ANUTHURAPPLICATION_API WindowStyle :DWORD {
			Bordered = WS_BORDER, // The window has a thin-line border.
			
			TitledAndBordered = WS_CAPTION, // The window has a title bar. It is also bordered.

			InitiallyDisabled = WS_DISABLED, // The window is initially disabled, thus cannot receive input from the user until a corresponding call to EnableWindow function.

			HasHorizontalScrollBar = WS_HSCROLL, // The window has a horizontal scroll bar.

			HasVerticalScrollBar = WS_VSCROLL, // The window has a vertical scroll bar.

			InitiallyMinimized = WS_MINIMIZE, // The window is initially minimized.

			InitiallyMaximized = WS_MAXIMIZE, // The window is initially maximized.

			InitiallyVisible = WS_VISIBLE, // The window is initially visible. Can be turned off using ShowWindow or SetWindowPos.

			HasMinimizeButton = WS_MINIMIZEBOX, // THe window has a minimize button. Should be used along with WS_SYSMENU.

			HasMaximizeButton = WS_MAXIMIZEBOX, // THe window has a maximize button. Should be used along with WS_SYSMENU.

			Overlapped = WS_OVERLAPPED, // The window has a title bar and a border.

			OverlappedWindow = WS_OVERLAPPEDWINDOW, // The window is an overlapped window.

			HasSizeBox = WS_SIZEBOX, // The window has a sizing box. == WS_THICKFRAME

			HasSystemMenu = WS_SYSMENU, // The window has a window menu on its title bar. Should be used along with WS_CAPTION.
		};

		ANUTHURAPPLICATION_API WindowStyle operator|(WindowStyle a, WindowStyle b);
		ANUTHURAPPLICATION_API WindowStyle operator&(WindowStyle a, WindowStyle b);

		enum class ANUTHURAPPLICATION_API ExtendedWindowStyle : DWORD {
			AcceptDragDropping = WS_EX_ACCEPTFILES, // The window accepts drag-drop files.

			HasSunkenEdge = WS_EX_CLIENTEDGE, // The window has a border with a sunken edge.

			HasRaisedEdge = WS_EX_WINDOWEDGE, // The window has a border with a raised edge.

			LeftAligned = WS_EX_LEFT, // The window has generic left-aligned properties, including the alignment of texts, scroll bars and so on. This is the default.

			RightAligned = WS_EX_RIGHT, // The window has generic right-aligned properties.

			NoActive = WS_EX_NOACTIVATE, // The window won't become foreground when the user clicks it, or when minimizing/closing other windows. Method SetActiveWindow and SetForegroundWindow can be used. The window doesn't appear on the taskbar by default, use WS_EX_APPWINDOW to force appearing.

			OverlappedWindow = WS_EX_OVERLAPPEDWINDOW, // This window is an overlapped window.

			PaletteWindow = WS_EX_PALETTEWINDOW, // This window is palette window, i.e. a modeless dialog box that presents an array of commands.

			StaticEdge = WS_EX_STATICEDGE, // This edge style is used to indicate that the window don't accept user input.

			ToolWindow = WS_EX_TOOLWINDOW, // The window is intended to be used as a floating toolbar. A tool window has a title bar that is shorter than a normal title bar, and the window title is drawn using a smaller font. A tool window does not appear in the taskbar or in the dialog that appears when the user presses ALT+TAB. If a tool window has a system menu, its icon is not displayed on the title bar. However, you can display the system menu by right-clicking or by typing ALT+SPACE.

			StayOnTop = WS_EX_TOPMOST, // The window should be placed above all non-topmost windows and should stay above them, even when the window is deactivated. To add or remove this style, use the SetWindowPos function.

		};

		ANUTHURAPPLICATION_API ExtendedWindowStyle operator|(ExtendedWindowStyle a, ExtendedWindowStyle b);
		ANUTHURAPPLICATION_API ExtendedWindowStyle operator&(ExtendedWindowStyle a, ExtendedWindowStyle b);
#pragma endregion Styles related enumerations

		
		enum class ANUTHURAPPLICATION_API SystemCursor {
			Arrow = 32512,
			Ibeam = 32513,
			Wait = 32514,
			Cross = 32515,
			Uparrow = 32516,
			SizeNorthwestToSoutheast = 32642,
			SizeNortheastToSouthwest = 32643,
			SizeWestToEast = 32644,
			SizeNorthToSouth = 32645,
			SizeAll = 32646,
			SlashedCircle = 32648
		};

		class ANUTHURAPPLICATION_API WindowIcon {
		public:
			static WindowIcon LoadFromFile(const std::wstring& fileName, int desiredWidth = 0, int desiredHeight = 0, bool useSystemSizeAsDefault = true);
			static WindowIcon LoadFromModule(HINSTANCE hModule, const std::wstring& name, int desiredWidth = 0, int desiredHeight = 0, bool useSystemSizeAsDefault = true);
			
			WindowIcon(const WindowIcon&) = delete;
			WindowIcon(WindowIcon&& w);
			WindowIcon& operator=(const WindowIcon&) = delete;
			WindowIcon& operator=(WindowIcon&& w);
			~WindowIcon();
			bool isValid()const { return _isValid; }
			HICON Handle()const { return _hIcon; }
			HICON Free();
		private:
			void Destroy();
			WindowIcon() {}
			HICON _hIcon;
			bool _isValid;
		};

		class ANUTHURAPPLICATION_API WindowCursor {
		public:
			static WindowCursor LoadFromFile(const std::wstring& fileName, int desiredWidth = 0, int desiredHeight = 0, bool useSystemSizeAsDefault = true);
			static WindowCursor LoadFromModule(HINSTANCE hModule, const std::wstring& name, int desiredWidth = 0, int desiredHeight = 0, bool useSystemSizeAsDefault = true);
			static WindowCursor LoadFromSystem(SystemCursor sc);

			WindowCursor(const WindowCursor&) = delete;
			WindowCursor(WindowCursor&& w);
			WindowCursor& operator=(const WindowCursor&) = delete;
			WindowCursor& operator=(WindowCursor&& w);
			~WindowCursor();
			bool isValid()const { return _isValid; }
			HCURSOR Handle()const { return _hCursor; }
			HCURSOR Free();

		private:
			void Destroy();
			WindowCursor() {}
			HCURSOR _hCursor;
			bool _isValid;
			bool _isSystemResource;
		};

		using WNDC_ID = ATOM;

		class ANUTHURAPPLICATION_API WindowClassDescriptor {
		public:
			WindowClassDescriptor(const std::wstring& className, WindowClassStyle style, 
				HCURSOR cursor, HICON icon = NULL);
			WindowClassDescriptor(const WindowClassDescriptor&) = default;
			WindowClassDescriptor& operator=(const WindowClassDescriptor&) = default;
			~WindowClassDescriptor() = default;

			WNDCLASSEX GenerateWNDCLASSEX(WNDPROC lpfnWndProc, HINSTANCE hInstance)const;
			WNDC_ID Register(WNDPROC lpfnWndProc, HINSTANCE hInstance)const;

			WindowClassStyle m_style;
			HICON m_hIcon;
			HCURSOR m_hCursor;
			std::wstring m_className;
		};

		class ANUTHURAPPLICATION_API WindowDescriptor {
		public:
			WindowDescriptor(WindowStyle style, ExtendedWindowStyle extendedStyle, const RECT& rect);
			WindowDescriptor(WindowStyle style, ExtendedWindowStyle extendedStyle,
				LONG left, LONG top, LONG right, LONG bottom);
			WindowDescriptor(const WindowDescriptor&) = default;
			WindowDescriptor& operator=(const WindowDescriptor&) = default;
			~WindowDescriptor() = default;

			HWND GenerateWindowByClassName(HINSTANCE hInstance, std::wstring& windowClassName, std::wstring& windowTitle, bool useSystemDefaultPosition=true)const;
			HWND GenerateWindowByATOM(HINSTANCE hInstance, ATOM atom, std::wstring& windowTitle, bool useSystemDefaultPosition = true)const;

			WindowStyle m_style;
			ExtendedWindowStyle m_extendedStyle;

			// This union describes the position of the window's client area on screen
			// (in pixels) before making adjustments according to its styles.
			union {
				struct {
					LONG m_left;
					LONG m_top;
					LONG m_right;
					LONG m_bottom;
				};
				RECT m_rect;
			};

		};
	}
}