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
//#define ANUTHURAPPLICATION_API
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

			HWND GenerateWindowByClassName(HINSTANCE hInstance, std::wstring& windowClassName, std::wstring& windowTitle, bool useSystemDefaultPosition = true)const;
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

		namespace Message {
			enum class ANUTHURAPPLICATION_API UsefulWindowMessage : int {
				ApplicationCommand = WM_APPCOMMAND, // Notifies the window that the user generates an application command event. wParam==hWndSource. lParam should be used along with GET_APPCOMMAND_LPARAM (return APPCOMMAND_ prefixed enum indicating the command), GET_DEVICE_LPARAM(return FAPPCOMMAND_ prefixed enum indicating the source device generating the command) and GETKEYSTATE_LPARAM(return MK_ prefixed enum indicating whether various virtual key is down) macros.
													// DefWindowProc generates this msg upon WM_XBUTTONUP or WM_NCXBUTTONUP, or when the user types an application command key. DefWindowProc of the top-level window will call a shell hook with HSHELL_APPCOMMAND.

				ApplicationActiveDeactive = WM_ACTIVATEAPP, // Sent when a window belonging to a different process is about to be activated. Both the current active window (wParam==FALSE) and the about-to active window (lParam==TRUE) will receive this message. The lParam is the threadID of the opponent's window's owner.

				WindowActiveDeactive = WM_ACTIVATE, // Sent to both the window being activated and deactivated. LOWORD(wParam) == WA_ACTIVE(active by methods other than mouse click) || WA_CLICKACTIVE || WA_INACTIVATE (deactivated). HIWORD(wParam)==if the window is minimized. lParam==hWndOpponent.
													// The DefWindowProc sets the keyboard focus if the activated window is not minimized. If activated by mouse click, it also receives a WM_MOUSEACTIVATE msg.
				Close = WM_CLOSE, // Sent as a signal that the window should terminate. The application can process this message and reject the request.

				Creating = WM_CREATE, // Sent when the window is under CreateWindowEx/CreateWindow request. If the WndProc returns -1, the function will return NULL.

				Destroying = WM_DESTROY, // Sent when the window is being destroyed.

				DPIChanged = WM_DPICHANGED, // Sent when the effective DPI has changed. HIWORD(wParam)==newY, LOWORD(wParam)==newX. lParam==pRect, where the Rect provides suggested size and position of the current window scaled for the new DPI. The application must be PROCESS_PER_MONITOR_DPI_AWARE applications in order to receive this msg.

				EnableStateChanged = WM_ENABLE, // Sent after the enabled state style bit has changed, but before returning of the EnableWindow function. wParam==BOOL(changedToEnable).

				EnterSizingOrMoving = WM_ENTERSIZEMOVE, // Sent one time after the window enters the moving or sizing modal loop, e.g. when the user clicks on the title bar or sizing border. The message is sent regardless of whether dragging of full window is enabled.

				ExitSizingOrMoving = WM_EXITSIZEMOVE, // Sent one time after the window exits the moving or sizing modal loop, e.g. when the user clicks on the title bar or sizing border. The message is sent regardless of whether dragging of full window is enabled.

				InputLanguageChanged = WM_INPUTLANGCHANGE, // Sent to the topmost affected window after application's input language has been changed. The DefWindowProc bypass this information to child windows. wParam==CharacterSetOfNewLocale, lParam==InputLocaleID.

				Moved = WM_MOVE, // Sent after the window has been moved. Let Pos = Coordinate of the upper left corner of the window's client area (in its parent's client area frame), HIWORD(lParam)==yPos, LOWORD(lParam)==xPos.

				Moving = WM_MOVING, // Sent when a window is moving. lParam==pRect, where the Rect indicates the current position of the window in screen coordinates. The aaplication can change members in this structure to change he actual position. If the message is processed, the app should return TRUE.

				HiddenStateChanged = WM_SHOWWINDOW, // Sent to a window when it is about to be hidden or shown. wParam==BOOL(toBeShown). If the window is created with WM_VISIBLE style, the msg will be received after creation, befor actually being displayed. When the call to ShowWindow specifies the SW_SHOWNORMAL flag, the message won't be generated.
													// lParam specifies the cause of this msg. 0 indicates that it is the result of a ShowWindow function call. SW_OTHERRUNXOOM indicates that the window is being uncovered because of other window's maximization/minimization. SW_OTHERZOOM indicates the window is being covered by .... SW_PARENTCLOSING, SW_PARENTOPENING ..

				Sized = WM_SIZE, // Sent after the size of the window has changed. wParam specifies the type of resizing. It can be SIZE_MAXHIDE(sent to all popups when other windows is maximized),  SIZE_MAXIMIZED, SIZE_MAXSHOW( sent to popups ...), SIZE_MINIMIZED, SIZE_RESTORED( the window has been resized, but neither maximized or minimized.) Let CA = ClientArea, LOWORD(lParam)==widthCA, HIWORD(lParam)==heightCA.

				Sizing = WM_SIZING, // Sent when the window is being sized. wParam specifies the  edge of the window being sized. WMSZ_BOTTOM, WMSZ_BOTTOMLEFT, WMSZ_BOTTOMRIGHT, WMSZ_LEFT, WMSZ_RIGHT, WMSZ_TOP, WMSZ_TOPLEFT, WMSZ_TOPRIGHT. lParam==pRect, where Rect specifies the screen coordinates of the drag rectangle. If changed, the app should return TRUE.

				StyleChanged = WM_STYLECHANGED, // Sent after the SetWindwoLong function has changed the style. WParam can be a bitwise OR of GWL_EXSTYLE and GWL_STYLE. lParam is a pStyleStruct that contains the new styles for the window. This structure can be examined but can't be changed.

				StyleChanging = WM_STYLECHANGING, // Sent when the SetWindowlong is about to change one or more of the window's style. WParam can be a bitwise OR of GWL_EXSTYLE and GWL_STYLE. lParam is a pStyleStruct that contains the new styles for the window. This structure can be examined and modified if necessary.

				PositionOrSizeChanged = WM_WINDOWPOSCHANGED, // Sent when the window's position/size/Z-order has changed as a result of SetWindowsPos function or other window-managment function. lParam==pWINDOWPOS which contains info. about the new size and position. The DefWindowProc sends WM_SIZE and WM_MOVE messages to the window.

				PositionOrSizeChanging = WM_WINDOWPOSCHANGING, // Sent to a window whose size/pos/Z-order is about to change as a result off relating function calls. lParam==pWINDOWPOS, which can be modified during processing. The DefWindowProc of an overlapped or thickframe window would send a WM_GETMINMAXINFO to the window, which aims at validating if the new pos/size meets the style's requirements. 

				RawInput = WM_INPUT, // Sent to the window that is getting raw input by calling RegisterRawInputDevices with valid device specifications. wParam == RIM_INPUT if the application is in the foreground, or RIM_INPUT_SINK if not. lParam == hRAWINPUT. The raw data can be retrieved by GetRawInputData.

				InputDeviceChanged = WM_INPUT_DEVICE_CHANGE, // Sent to the window registered to receive raw input. wParam == GIDC_ARRIVAL || GIDC_REMOVAL. lParam == hRawInputDevice. Call GetRawInputDeviceInfo on it to..

				MouseCaptureLost = WM_CAPTURECHANGED, // Sent when the window is loosing mouse capture. lParam == hWnd which is to gain the mouse capture. The window receives this msg even by calling ReleaseCapture itself.

				LeftButtonDoubleClick = WM_LBUTTONDBLCLK, // Posted when the user double clicks the left mouse button while the cursor is in the CA. wParam == MK_CONTROL || MK_LBUTTON || MK_MBUTTON || MK_RBUTTON || MK_SHIFT || MK_XBUTTON1 || MK_XBUTTON2 indcating whether these virtual keys are down at the same time. LOWORD(lParam)==xPos, HIWORD(lParam)==yPos, relative to the upperleft corner of the CA. One should use GET_X_LPARAM and GET_Y_LPARAM to get the coordinates rather than use HIWORD and LOWORD directly, because on mulmonitor-systems it will result in incorrect results.
															// Only windows that have the CS_DBLCLKS style can receive WM_LBUTTONDBLCLK messages. A double click actually generates four messages in sequence: WM_LBUTTONDOWN, WM_LBUTTONUP, WM_LBUTTONDBLCLK, WM_LBUTTONUP.

				LeftButtonDown = WM_LBUTTONDOWN, // Posted when the user presses the left mouse button while the cursor is in the CA. wParam == MK_CONTROL || MK_LBUTTON || MK_MBUTTON || MK_RBUTTON || MK_SHIFT || MK_XBUTTON1 || MK_XBUTTON2 indcating whether these virtual keys are down at the same time. LOWORD(lParam)==xPos, HIWORD(lParam)==yPos, relative to the upperleft corner of the CA. One should use GET_X_LPARAM and GET_Y_LPARAM to get the coordinates rather than use HIWORD and LOWORD directly, because on mulmonitor-systems it will result in incorrect results.

				LeftButtonUp = WM_LBUTTONUP, // Posted when the user releases the left mouse button while the cursor is in the CA. wParam == MK_CONTROL || MK_LBUTTON || MK_MBUTTON || MK_RBUTTON || MK_SHIFT || MK_XBUTTON1 || MK_XBUTTON2 indcating whether these virtual keys are down at the same time. LOWORD(lParam)==xPos, HIWORD(lParam)==yPos, relative to the upperleft corner of the CA. One should use GET_X_LPARAM and GET_Y_LPARAM to get the coordinates rather than use HIWORD and LOWORD directly, because on mulmonitor-systems it will result in incorrect results.

				MidButtonDoubleClick = WM_MBUTTONDBLCLK, // Posted when the user double clicks the middle mouse button while the cursor is in the CA. wParam == MK_CONTROL || MK_LBUTTON || MK_MBUTTON || MK_RBUTTON || MK_SHIFT || MK_XBUTTON1 || MK_XBUTTON2 indcating whether these virtual keys are down at the same time. LOWORD(lParam)==xPos, HIWORD(lParam)==yPos, relative to the upperleft corner of the CA. One should use GET_X_LPARAM and GET_Y_LPARAM to get the coordinates rather than use HIWORD and LOWORD directly, because on mulmonitor-systems it will result in incorrect results.
															// Only windows that have the CS_DBLCLKS style can receive WM_LBUTTONDBLCLK messages. A double click actually generates four messages in sequence: WM_LBUTTONDOWN, WM_LBUTTONUP, WM_LBUTTONDBLCLK, WM_LBUTTONUP.

				MidButtonDown = WM_MBUTTONDOWN, // Posted when the user presses the middle mouse button while the cursor is in the CA. wParam == MK_CONTROL || MK_LBUTTON || MK_MBUTTON || MK_RBUTTON || MK_SHIFT || MK_XBUTTON1 || MK_XBUTTON2 indcating whether these virtual keys are down at the same time. LOWORD(lParam)==xPos, HIWORD(lParam)==yPos, relative to the upperleft corner of the CA. One should use GET_X_LPARAM and GET_Y_LPARAM to get the coordinates rather than use HIWORD and LOWORD directly, because on mulmonitor-systems it will result in incorrect results.

				MidButtonUp = WM_MBUTTONUP, // Posted when the user releases the middle mouse button while the cursor is in the CA. wParam == MK_CONTROL || MK_LBUTTON || MK_MBUTTON || MK_RBUTTON || MK_SHIFT || MK_XBUTTON1 || MK_XBUTTON2 indcating whether these virtual keys are down at the same time. LOWORD(lParam)==xPos, HIWORD(lParam)==yPos, relative to the upperleft corner of the CA. One should use GET_X_LPARAM and GET_Y_LPARAM to get the coordinates rather than use HIWORD and LOWORD directly, because on mulmonitor-systems it will result in incorrect results.

				RightButtonDoubleClick = WM_RBUTTONDBLCLK, // Posted when the user double clicks the right mouse button while the cursor is in the CA. wParam == MK_CONTROL || MK_LBUTTON || MK_MBUTTON || MK_RBUTTON || MK_SHIFT || MK_XBUTTON1 || MK_XBUTTON2 indcating whether these virtual keys are down at the same time. LOWORD(lParam)==xPos, HIWORD(lParam)==yPos, relative to the upperleft corner of the CA. One should use GET_X_LPARAM and GET_Y_LPARAM to get the coordinates rather than use HIWORD and LOWORD directly, because on mulmonitor-systems it will result in incorrect results.
														   // Only windows that have the CS_DBLCLKS style can receive WM_LBUTTONDBLCLK messages. A double click actually generates four messages in sequence: WM_LBUTTONDOWN, WM_LBUTTONUP, WM_LBUTTONDBLCLK, WM_LBUTTONUP.

				RightButtonDown = WM_RBUTTONDOWN, // Posted when the user presses the right mouse button while the cursor is in the CA. wParam == MK_CONTROL || MK_LBUTTON || MK_MBUTTON || MK_RBUTTON || MK_SHIFT || MK_XBUTTON1 || MK_XBUTTON2 indcating whether these virtual keys are down at the same time. LOWORD(lParam)==xPos, HIWORD(lParam)==yPos, relative to the upperleft corner of the CA. One should use GET_X_LPARAM and GET_Y_LPARAM to get the coordinates rather than use HIWORD and LOWORD directly, because on mulmonitor-systems it will result in incorrect results.

				RightButtonUp = WM_RBUTTONUP, // Posted when the user releases the right mouse button while the cursor is in the CA. wParam == MK_CONTROL || MK_LBUTTON || MK_MBUTTON || MK_RBUTTON || MK_SHIFT || MK_XBUTTON1 || MK_XBUTTON2 indcating whether these virtual keys are down at the same time. LOWORD(lParam)==xPos, HIWORD(lParam)==yPos, relative to the upperleft corner of the CA. One should use GET_X_LPARAM and GET_Y_LPARAM to get the coordinates rather than use HIWORD and LOWORD directly, because on mulmonitor-systems it will result in incorrect results.

				XButtonDoubleClick = WM_XBUTTONDBLCLK, // Posted when the user double clicks one of the two x-buttons. HIWORD(wParam) == XBUTTON1 || XBUTTON2. LOWORD(wParam) == MK_CONTROL || MK_LBUTTON || MK_MBUTTON || MK_RBUTTON || MK_SHIFT || MK_XBUTTON1 || MK_XBUTTON2 indcating whether these virtual keys are down at the same time.  LOWORD(lParam)==xPos, HIWORD(lParam)==yPos, relative to the upperleft corner of the CA. One should use GET_X_LPARAM and GET_Y_LPARAM to get the coordinates rather than use HIWORD and LOWORD directly, because on mulmonitor-systems it will result in incorrect results.
													   // Only windows that have the CS_DBLCLKS style can receive WM_LBUTTONDBLCLK messages. A double click actually generates four messages in sequence: WM_LBUTTONDOWN, WM_LBUTTONUP, WM_LBUTTONDBLCLK, WM_LBUTTONUP.

				XButtonDown = WM_XBUTTONDOWN, // Posted when the user presses one of the two x-buttons. HIWORD(wParam) == XBUTTON1 || XBUTTON2. LOWORD(wParam) == MK_CONTROL || MK_LBUTTON || MK_MBUTTON || MK_RBUTTON || MK_SHIFT || MK_XBUTTON1 || MK_XBUTTON2 indcating whether these virtual keys are down at the same time.  LOWORD(lParam)==xPos, HIWORD(lParam)==yPos, relative to the upperleft corner of the CA. One should use GET_X_LPARAM and GET_Y_LPARAM to get the coordinates rather than use HIWORD and LOWORD directly, because on mulmonitor-systems it will result in incorrect results.

				XButtonUp = WM_XBUTTONUP, // Posted when the user presses one of the two x-buttons. HIWORD(wParam) == XBUTTON1 || XBUTTON2. LOWORD(wParam) == MK_CONTROL || MK_LBUTTON || MK_MBUTTON || MK_RBUTTON || MK_SHIFT || MK_XBUTTON1 || MK_XBUTTON2 indcating whether these virtual keys are down at the same time.  LOWORD(lParam)==xPos, HIWORD(lParam)==yPos, relative to the upperleft corner of the CA. One should use GET_X_LPARAM and GET_Y_LPARAM to get the coordinates rather than use HIWORD and LOWORD directly, because on mulmonitor-systems it will result in incorrect results.

				ActivatedByMouse = WM_MOUSEACTIVATE, // Sent when the cursor is in an inactive window and the user presses a mouse button. The message would be sent to the parent window if the child window passes it to DefWindowProc. wParam == hWndTop, LOWORD(lParam)==result of hittest invoked by DefWindowProc's WM_NCHITTEST message. HIWORD(lParam)==MSG_TYPE received from the press. Whether the associated message is posted or not depends on the return value of this message's proc, which can be:
													// MA_ACTIVATE, activates the window, doesn't discard the msg.	MA_ACTIVATEANDEAT, activates the window and discards the mouse msg.	MA_NOACTIVATE, doesn't activate the window, doesn't discard the msg.	MA_NOACTIVATEANDEAT, doesn't activate the window, and discards the mouse msg.


				MouseMoving = WM_MOUSEMOVE, // Posted to a window when the cursor moves. If not captured, posted to the window containing the cursor.  wParam == MK_CONTROL || MK_LBUTTON || MK_MBUTTON || MK_RBUTTON || MK_SHIFT || MK_XBUTTON1 || MK_XBUTTON2 indcating whether these virtual keys are down at the same time. LOWORD(lParam)==xPos, HIWORD(lParam)==yPos, relative to the upperleft corner of the CA. One should use GET_X_LPARAM and GET_Y_LPARAM to get the coordinates rather than use HIWORD and LOWORD directly, because on mulmonitor-systems it will result in incorrect results.

				MouseHovering = WM_MOUSEHOVER, // Posted to a window when the cursor hovers over the client area of the window for the period of time specified in a prior call to TrackMouseEvent. wParam == MK_CONTROL || MK_LBUTTON || MK_MBUTTON || MK_RBUTTON || MK_SHIFT || MK_XBUTTON1 || MK_XBUTTON2 indcating whether these virtual keys are down at the same time.  LOWORD(lParam)==xPos, HIWORD(lParam)==yPos, relative to the upperleft corner of the CA. One should use GET_X_LPARAM and GET_Y_LPARAM to get the coordinates rather than use HIWORD and LOWORD directly, because on mulmonitor-systems it will result in incorrect results.

				MouseLeft = WM_MOUSELEAVE, // Posted when the cursor leaves the client area of the window specified in a prior call to TrackMouseEvent. All tracking requested by TrackMouseEvent is canceled when this msg generates. The app must call it again when the mouse reenters the window, and it requires further tracking of mouse hover behaviors.

				NCHitTest = WM_NCHITTEST, // Sent to the window in order to determine what part of the window corresponds to a particular screen coordinate. This can happen when the cursor moves, a mouse button is pressed or released, or in response to a function such as WindowFromPoint. If the mouse is not captured, the msg is sent to the window beneath the cursor. LOWORD(lParam)==xPos, HIWORD(lParam)==yPos, relative to the upperleft corner of the screen. One should use GET_X_LPARAM and GET_Y_LPARAM to get the coordinates rather than use HIWORD and LOWORD directly, because on mulmonitor-systems it will result in incorrect results.
											// The return value of the DefWindowProc function is one of the following, indicating the hit-test result: HTBORDER, in the border of a window that doesn't has a sizing border.	HTBOTTOM, in the bottom sizing border.	HTBOTTOMLEFT, in the lower-left sizing corner.	HTBOTTOMRIGHT	HTCAPTION, in the title bar.	HTCLIENT, in a CA.	HICLOSE, in a Close button. HTERROR, on the screen bg or on a dividing line between windows. HTGROWBOX, in a size box. HTHELP, in a help button. HTHSCROL, in a horizontal scroll bar. HTLEFT,	HTMENU,	HTMAXBUTTON,	HTMINBUTTON,	HTREDUCE, in a Minimize button, HTRIGHT,	HTSYSMENU,	HTTOP,	HTTOPRIGHT,	HTTOPLEFT,	HTTRANSPARENT,	HTVSCROLL,	HTZOOM, in a Maximize button.

				HorizontalMouseWheel = WM_MOUSEHWHEEL, // Sent to the active window when the mouse's horizontal wheel is tiled or rotated. DetWindowProc propagates the message to parent window. HIWORD(wParam)==distance of rotation, expressed in multiples or factors of WHEEL_DELTA. Positive indicates right, negative left. LOWORD(wParam) == MK_CONTROL || MK_LBUTTON || MK_MBUTTON || MK_RBUTTON || MK_SHIFT || MK_XBUTTON1 || MK_XBUTTON2 indcating whether these virtual keys are down at the same time.  LOWORD(lParam)==xPos, HIWORD(lParam)==yPos, relative to the upperleft corner of the CA. One should use GET_X_LPARAM and GET_Y_LPARAM to get the coordinates rather than use HIWORD and LOWORD directly, because on mulmonitor-systems it will result in incorrect results.

				DeviceChanged = WM_DEVICECHANGE, // Notifies an application of a change to hardware configuration of a device or computer. wParam indicates the event that has occurred, which can be
												// DBT_CONFIGCHANGECANCELD, a request to change the current configuration has been canceled.
												// DBT_CONFIGCHANGED, the current configuration has changed, due to a dock or undock.
												// DBT_CUSTOMEVENT,	DBT_USERDEFINED,	DBT_QEURYCHANGECONFIG.
												// DBT_DEVICEARRIVAL, a dive or piece of media has been inserted and is now available.
												// DBT_DEVICEQUERYREMOVE, permission is requested for removal. Can be denied, thus cancel the removal.
												// DBT_DEVICEQUERYREMOVEFAILED,	DBT_DEVICEREMOVECOMPLETE,	DBT_DEVICEREMOVEPENDING,	DBT_DEVICETYPESPECIFIC.
												// DBT_DEVNODES_CHANGED, a device has been added or removed from the system.
												// lParam is a pointer to a structure that contains the event-specific data. Return TRUE to grant the request, BROADCAST_QUERY_DENY to deny the request.

				CopyData = WM_COPYDATA, // The application should send this msg to pass data to another application. wParam==hWndReveiver. lParam==pCOPYDATASTRUCTURE, which contains the data to be passed. Return TRUE to indicate successful receiving, otherwise FALSE. When passing, the data being passed shall not be modified. The pointer used by lParam shall be considered read-only by the receiving window.

				Character = WM_CHAR, // Posted to the window with key focus when a WM_KEYDOWN msg is translated by the TranslateMessage function. wParam==character code of the key. lParam is shouwn in the following table:
									// bits0-15: the repeat count for the current msg, being the result of the number of times the keystroke is autorepeated when the user holds the key. If it is held long enough, multiple msg will be sent. Yet this count doesn't cumulate.
									// bits16-23: the scan code, depending on the OEM.
									// bits24: indicates whether the key is an extended key(1), or not(0).
									// bits25-28 is reserved.
									// bits29: if ALT is down(1) or not(0).
									// bits30: the previous key state. If down(1) or not(0).
									// bits31: the transition state, if the key is being released(1) or not(0).
									// It uses UTF-16.
									// WM_UNICHAR is similar, except that it uses UTF-32.

				DeadCharacter = WM_DEADCHAR, // Posted to the window with the keyboard focus when a WM_KEYUP msg is translated by the TranslateMessage function. It specifies a character code generated by a dead key, which is a key that generates a character that is combined with another to form a composite character. wParam and lParam is the same as that of WM_CHAR's.

				HotKeyPressed = WM_HOTKEY, // Posted when the user presses a hot key registered by RegisterHotKey. The msg is placed at the top of the thread's msg queue. wParam==hotKeyID. If system generated, it is either IDHOT_SNAPDESKTOP or IDHOT_SNAPWINDOW. LOWORD(lParam)==keyToBePressedInCombinationWith HIWORD(lParam) == MOD_ALT || MOD_CONTROL || MOD_SHIFT || MOD_WIN.

				KeyDown = WM_KEYDOWN, // Posted to the window with keyboard focus when a non system key is pressed. i.e. when ALT is not pressed (along with it). wParam indicates the virtual key code, in https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx .  lParam is shown in the following table:
										// bits0-15: the repeat count for the current msg, being the result of the number of times the keystroke is autorepeated when the user holds the key. If it is held long enough, multiple msg will be sent. Yet this count doesn't cumulate.
										// bits16-23: the scan code, depending on the OEM.
										// bits24: indicates whether the key is an extended key(1), or not(0).
										// bits25-28 is reserved.
										// bits29: if ALT is down(1) or not(0).
										// bits30: the previous key state. If down(1) or not(0).
										// bits31: the transition state, if the key is being released(1) or not(0).

				KeyUp = WM_KEYUP,  // Posted to the window with keyboard focus when a non system key is released. i.e. when ALT is not pressed (along with it). wParam indicates the virtual key code, in https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx .  lParam is shown in the following table:
									// bits0-15: the repeat count for the current msg, being the result of the number of times the keystroke is autorepeated when the user holds the key. If it is held long enough, multiple msg will be sent. Yet this count doesn't cumulate.
									// bits16-23: the scan code, depending on the OEM.
									// bits24: indicates whether the key is an extended key(1), or not(0).
									// bits25-28 is reserved.
									// bits29: if ALT is down(1) or not(0).
									// bits30: the previous key state. If down(1) or not(0).
									// bits31: the transition state, if the key is being released(1) or not(0).

				KeyboardFocusLost = WM_KILLFOCUS, // Sent immediately before the window loses the keyboard focus. wParam == hWndNewFocus, which can be NULL.

				KeyboardFocusGained = WM_SETFOCUS, // Sent to a window after it has gained the keyboard focus. wParam == hWndOldFocus, which can be NULL.

			};

			ANUTHURAPPLICATION_API bool operator==(UsefulWindowMessage a, int b);
		}
	}
}