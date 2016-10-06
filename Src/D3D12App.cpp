//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#include "D3D12App.h"


void Luxko::Anuthur::D3D12App::OnInit()
{
	_keyboard = std::make_unique<DirectX::Keyboard>();
	_mouse = std::make_unique<DirectX::Mouse>();
	_mouse->SetWindow(_hWindow);

	_d3d12Manager.Initialize();
	_d3d12Manager.ConfigureMainWndResource(*(static_cast<Luxko::Application::BaseApp*>(this)), _windowed);
}


Luxko::Anuthur::D3D12App::~D3D12App() {}

void Luxko::Anuthur::D3D12App::OnResize()
{
	
}

bool Luxko::Anuthur::D3D12App::OnEvent(MSG msg)
{
	auto message = static_cast<Application::Message::UsefulWindowMessage>(msg.message);
	switch (message)
	{
	case Luxko::Application::Message::UsefulWindowMessage::ApplicationCommand:
		break;
	case Luxko::Application::Message::UsefulWindowMessage::ApplicationActiveDeactive:
		DirectX::Keyboard::ProcessMessage(msg.message, msg.wParam, msg.lParam);
		DirectX::Mouse::ProcessMessage(msg.message, msg.wParam, msg.lParam);
		if (LOWORD(msg.wParam) == WA_INACTIVE) {
			_appPaused = true;
			_mainTimer.Pause();
		}
		else {
			_appPaused = FALSE;
			_mainTimer.Start();
		}
		//_mouseStateTracker.Update(_mouse->GetState());
		//_keyboardStateTracker.Update(_keyboard->GetState());
		break;
	case Luxko::Application::Message::UsefulWindowMessage::WindowActiveDeactive:
		break;
	case Luxko::Application::Message::UsefulWindowMessage::Close:
		break;
	case Luxko::Application::Message::UsefulWindowMessage::Creating:
		break;
	case Luxko::Application::Message::UsefulWindowMessage::Destroying:
		break;
	case Luxko::Application::Message::UsefulWindowMessage::DPIChanged:
		break;
	case Luxko::Application::Message::UsefulWindowMessage::EnableStateChanged:
		break;
	case Luxko::Application::Message::UsefulWindowMessage::EnterSizingOrMoving:
		_appPaused = TRUE;
		_mainTimer.Pause();
		break;
	case Luxko::Application::Message::UsefulWindowMessage::ExitSizingOrMoving:
		_appPaused = FALSE;
		_mainTimer.Start();
		break;
	case Luxko::Application::Message::UsefulWindowMessage::InputLanguageChanged:
		break;
	case Luxko::Application::Message::UsefulWindowMessage::Moved:
		break;
	case Luxko::Application::Message::UsefulWindowMessage::Moving:
		break;
	case Luxko::Application::Message::UsefulWindowMessage::HiddenStateChanged:
		break;
	case Luxko::Application::Message::UsefulWindowMessage::Sized:
		_width = LOWORD(msg.lParam);
		_height = HIWORD(msg.lParam);
		OnResize();
		break;
	case Luxko::Application::Message::UsefulWindowMessage::Sizing:

		break;
	case Luxko::Application::Message::UsefulWindowMessage::StyleChanged:
		break;
	case Luxko::Application::Message::UsefulWindowMessage::StyleChanging:
		break;
	case Luxko::Application::Message::UsefulWindowMessage::PositionOrSizeChanged:
		break;
	case Luxko::Application::Message::UsefulWindowMessage::PositionOrSizeChanging:
		break;
	case Luxko::Application::Message::UsefulWindowMessage::RawInput:
		DirectX::Mouse::ProcessMessage(msg.message, msg.wParam, msg.lParam);
		//_mouseStateTracker.Update(_mouse->GetState());
		break;
	case Luxko::Application::Message::UsefulWindowMessage::InputDeviceChanged:
		break;
	case Luxko::Application::Message::UsefulWindowMessage::MouseCaptureLost:
		break;

	case Luxko::Application::Message::UsefulWindowMessage::LeftButtonDown:
	case Luxko::Application::Message::UsefulWindowMessage::MidButtonDown:
	case Luxko::Application::Message::UsefulWindowMessage::RightButtonDown:
	case Luxko::Application::Message::UsefulWindowMessage::XButtonDown:
	case Luxko::Application::Message::UsefulWindowMessage::LeftButtonUp:
	case Luxko::Application::Message::UsefulWindowMessage::MidButtonUp:
	case Luxko::Application::Message::UsefulWindowMessage::RightButtonUp:
	case Luxko::Application::Message::UsefulWindowMessage::XButtonUp:
	case Luxko::Application::Message::UsefulWindowMessage::MouseMoving:
	case Luxko::Application::Message::UsefulWindowMessage::MouseHovering:
	case Luxko::Application::Message::UsefulWindowMessage::HorizontalMouseWheel:
		DirectX::Mouse::ProcessMessage(msg.message, msg.wParam, msg.lParam);
		//_mouseStateTracker.Update(_mouse->GetState());
		break;
	//	OnMouseMove(msg.wParam, GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
	//	break;
	//	OnMouseUp(msg.wParam, GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
	//	break;

	case Luxko::Application::Message::UsefulWindowMessage::LeftButtonDoubleClick:
		break;
	case Luxko::Application::Message::UsefulWindowMessage::MidButtonDoubleClick:
		break;
	case Luxko::Application::Message::UsefulWindowMessage::RightButtonDoubleClick:
		break;
	case Luxko::Application::Message::UsefulWindowMessage::XButtonDoubleClick:
		break;

	case Luxko::Application::Message::UsefulWindowMessage::ActivatedByMouse:
		break;


	case Luxko::Application::Message::UsefulWindowMessage::MouseLeft:
		break;
	case Luxko::Application::Message::UsefulWindowMessage::NCHitTest:
		break;

	case Luxko::Application::Message::UsefulWindowMessage::DeviceChanged:
		break;
	case Luxko::Application::Message::UsefulWindowMessage::CopyData:
		break;
	case Luxko::Application::Message::UsefulWindowMessage::Character:
		break;
	case Luxko::Application::Message::UsefulWindowMessage::DeadCharacter:
		break;
	case Luxko::Application::Message::UsefulWindowMessage::HotKeyPressed:
		break;
	case Luxko::Application::Message::UsefulWindowMessage::KeyDown:
	case Luxko::Application::Message::UsefulWindowMessage::KeyUp:
	case Luxko::Application::Message::UsefulWindowMessage::SysKeyDown:
	case Luxko::Application::Message::UsefulWindowMessage::SysKeyUp:
		DirectX::Keyboard::ProcessMessage(msg.message, msg.wParam, msg.lParam);
		//_keyboardStateTracker.Update(_keyboard->GetState());
		break;
	case Luxko::Application::Message::UsefulWindowMessage::KeyboardFocusLost:
		break;
	case Luxko::Application::Message::UsefulWindowMessage::KeyboardFocusGained:
		break;
	default:
		break;
	}
	return true;
}

void Luxko::Anuthur::D3D12App::OnDestroy()
{

}


void Luxko::Anuthur::D3D12App::CreateMainDsvDescriptorHeaps()
{
	
}

void Luxko::Anuthur::D3D12App::LogFPSToTitle()
{
	static LONGLONG lastSecTick = 0;
	static LONGLONG framePerSec = 0ll;
	framePerSec++;
	if (lastSecTick == 0) lastSecTick = _mainTimer.PeekCurrentTick();
	auto current = _mainTimer.PeekCurrentTick();
	auto delta = current - lastSecTick;
	if (_mainTimer.TicksToSeconds(delta) >= 1.f) {
		lastSecTick = current;
		auto fps = L" - FPS: " + std::to_wstring(framePerSec);
		framePerSec = 0ll;
		SetWindowText(_hWindow, (_title + fps).c_str());
	}
}

Luxko::Anuthur::FrameResource::FrameResource(ID3D12Device* pDevice)
{
	ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(_cmdAllocator.GetAddressOf())));
}
