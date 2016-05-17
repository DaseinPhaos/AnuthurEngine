//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#include "D3D12App.h"


void Luxko::Anuthur::D3D12App::OnInit()
{
	BasicD3D12ElementsInitialization();
	_mainTimer.Reset(true);
}

void Luxko::Anuthur::D3D12App::BasicD3D12ElementsInitialization()
{
	// Enable debug layer
#if defined (DEBUG) || defined (_DEBUG)
	{
		ComPtr<ID3D12Debug> debugger;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(debugger.GetAddressOf())));
		debugger->EnableDebugLayer();
	}
#endif

	// Obtain the device
	{
		ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(_dxgiFactory.GetAddressOf())));
		HRESULT hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(_d3d12Device.GetAddressOf()));
		if (FAILED(hr)) {
			ComPtr<IDXGIAdapter> warpAdapter;

			ThrowIfFailed(_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)),
				L"Device Creation Failed: No adapters available.");

			ThrowIfFailed(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0,
				IID_PPV_ARGS(_d3d12Device.GetAddressOf())),
				L"Device Creation Failed: Can't obtain device from wARP adapter.");
		}
	}

	// Create the main fence, obtain descriptor sizes.
	{
		ThrowIfFailed(_d3d12Device->CreateFence(_currentMainFenceCount, D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(&_mainFence)), L"Failed to create the main fence.");

		_rtvDescriptorSize = _d3d12Device->
			GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		_dsvDescriptorSize = _d3d12Device->
			GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

		_samplerDescriptorSize = _d3d12Device->
			GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

		_cbvSrvUavDescriptorSize = _d3d12Device->
			GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	}

	// Command Queue and main command list initialization.
	CreateMainCmdObjects();

	// Create the swap chain.
	ResetSwapChain();

	// Initialize descriptor heaps.
	CreateMainDescriptorHeaps();

	// Create the render target view.
	{
		auto rtvHeapHandle = _mainRTVHeap->GetCPUDescriptorHandleForHeapStart();
		for (UINT i = 0; i < SwapChainBufferCount; ++i) {
			ThrowIfFailed(_swapChain->GetBuffer(i, 
				IID_PPV_ARGS(_swapChainBuffer[i].GetAddressOf())));

			_d3d12Device->CreateRenderTargetView(_swapChainBuffer[i].Get(),
				nullptr, rtvHeapHandle);

			rtvHeapHandle.ptr += _rtvDescriptorSize;
		}
	}

	// Create the Depth stencil buffer and its view.
	{
		D3D12_RESOURCE_DESC dsDesc;
		ZeroMemory(&dsDesc, sizeof(dsDesc));
		dsDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		dsDesc.Width = _width;
		dsDesc.Height = _height;
		dsDesc.DepthOrArraySize = 1;
		dsDesc.MipLevels = 1;
		dsDesc.Format = _depthStencilFormat;
		dsDesc.SampleDesc.Count = 1;
		dsDesc.SampleDesc.Quality = 0;
		dsDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		dsDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_HEAP_PROPERTIES heapProperties = {};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_CLEAR_VALUE dsCV;
		dsCV.Format = _depthStencilFormat;
		dsCV.DepthStencil.Depth = 1.f;
		dsCV.DepthStencil.Stencil = 0;

		ThrowIfFailed(_d3d12Device->CreateCommittedResource(&heapProperties,
			D3D12_HEAP_FLAG_NONE, &dsDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&dsCV, IID_PPV_ARGS(&_depthStencilBuffer)));

		_d3d12Device->CreateDepthStencilView(_depthStencilBuffer.Get(),
			nullptr, _mainDSVHeap->GetCPUDescriptorHandleForHeapStart());
	}

	// Set viewport and scissors
	{
		SetMainViewport(0.f, 0.f, static_cast<float>(_width), static_cast<float>(_height),
			0.f, 1.f);
		SetMainScissorRect(0l, _width, 0l, _height);
	}

}

void Luxko::Anuthur::D3D12App::ResetSwapChain(BOOL windowed)
{
	_swapChain.Reset();
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = _width;
	sd.BufferDesc.Height = _height;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = _backBufferFormat;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = SwapChainBufferCount;
	sd.OutputWindow = _hWindow;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	ThrowIfFailed(_dxgiFactory->CreateSwapChain(_cmdQueue.Get(), &sd,
		_swapChain.GetAddressOf()), L"Swap chain creation failed.");
}

D3D12_CPU_DESCRIPTOR_HANDLE Luxko::Anuthur::D3D12App::GetCurrentBackBufferView() const
{
	D3D12_CPU_DESCRIPTOR_HANDLE result;
	result.ptr = _mainRTVHeap->GetCPUDescriptorHandleForHeapStart().ptr + _currentBackBufferIndex * _rtvDescriptorSize;
	return result;
}

Luxko::Anuthur::D3D12App::~D3D12App() {}

void Luxko::Anuthur::D3D12App::FlushCommandQueue()
{
	_currentMainFenceCount++;
	ThrowIfFailed(_cmdQueue->Signal(_mainFence.Get(), _currentMainFenceCount));

	if (_mainFence->GetCompletedValue() < _currentMainFenceCount) {
		auto hEvent = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
		ThrowIfFailed(_mainFence->SetEventOnCompletion(_currentMainFenceCount, hEvent));
		WaitForSingleObject(hEvent, INFINITY);
		CloseHandle(hEvent);
	}
}


void Luxko::Anuthur::D3D12App::OnResize()
{
	static auto lastResizeTick = 0ll;
	if (lastResizeTick == 0) lastResizeTick = _mainTimer.GetFirstTick();
	auto currentTick = _mainTimer.PeekCurrentTick();
	auto delta = currentTick - lastResizeTick;
	lastResizeTick = currentTick;
	if (_mainTimer.TicksToSeconds(delta) >= 0.2) {
		FlushCommandQueue();
		ThrowIfFailed(_mainCmdList->Reset(_mainCmdAllocator.Get(), nullptr));

		for (auto i = 0u; i < SwapChainBufferCount; ++i) {
			_swapChainBuffer[i].Reset();
		}
		_depthStencilBuffer.Reset();

		ThrowIfFailed(_swapChain->ResizeBuffers(SwapChainBufferCount, _width, _height,
			_depthStencilFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

		_currentBackBufferIndex = 0;

		auto rtvHandle = _mainRTVHeap->GetCPUDescriptorHandleForHeapStart();
		for (auto i = 0u; i < SwapChainBufferCount; ++i) {
			ThrowIfFailed(_swapChain->GetBuffer(i, IID_PPV_ARGS(&_swapChainBuffer[i])));
			_d3d12Device->CreateRenderTargetView(_swapChainBuffer[i].Get(), nullptr, rtvHandle);
			rtvHandle.ptr += _rtvDescriptorSize;
		}

		D3D12_RESOURCE_DESC dsDesc;
		dsDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		dsDesc.Width = _width;
		dsDesc.Height = _height;
		dsDesc.DepthOrArraySize = 1;
		dsDesc.MipLevels = 1;
		dsDesc.Format = _depthStencilFormat;
		dsDesc.SampleDesc.Count = 1;
		dsDesc.SampleDesc.Quality = 0;
		dsDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		dsDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_HEAP_PROPERTIES heapProperties = {};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_CLEAR_VALUE dsCV;
		dsCV.Format = _depthStencilFormat;
		dsCV.DepthStencil.Depth = 1.f;
		dsCV.DepthStencil.Stencil = 0;

		ThrowIfFailed(_d3d12Device->CreateCommittedResource(&heapProperties,
			D3D12_HEAP_FLAG_NONE, &dsDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&dsCV, IID_PPV_ARGS(&_depthStencilBuffer)));

		_d3d12Device->CreateDepthStencilView(_depthStencilBuffer.Get(),
			nullptr, _mainDSVHeap->GetCPUDescriptorHandleForHeapStart());

		ThrowIfFailed(_mainCmdList->Close());
		ID3D12CommandList* cmdlsts[] = { _mainCmdList.Get() };
		_cmdQueue->ExecuteCommandLists(_countof(cmdlsts), cmdlsts);

		FlushCommandQueue();

		SetMainViewport(0.f, 0.f, static_cast<float>(_width), static_cast<float>(_height),
			0.f, 1.f);
		SetMainScissorRect(0l, _width, 0l, _height);
	}
}

bool Luxko::Anuthur::D3D12App::OnEvent(MSG msg)
{
	auto message = static_cast<Application::Message::UsefulWindowMessage>(msg.message);
	switch (message)
	{
	case Luxko::Application::Message::UsefulWindowMessage::ApplicationCommand:
		break;
	case Luxko::Application::Message::UsefulWindowMessage::ApplicationActiveDeactive:
		break;
	case Luxko::Application::Message::UsefulWindowMessage::WindowActiveDeactive:
		if (LOWORD(msg.wParam) == WA_INACTIVE) {
			_appPaused = true;
			_mainTimer.Pause();
		}
		else {
			_appPaused = FALSE;
			_mainTimer.Start();
		}
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
		break;
	case Luxko::Application::Message::UsefulWindowMessage::InputDeviceChanged:
		break;
	case Luxko::Application::Message::UsefulWindowMessage::MouseCaptureLost:
		break;

	case Luxko::Application::Message::UsefulWindowMessage::LeftButtonDown:
	case Luxko::Application::Message::UsefulWindowMessage::MidButtonDown:
	case Luxko::Application::Message::UsefulWindowMessage::RightButtonDown:
	case Luxko::Application::Message::UsefulWindowMessage::XButtonDown:
		OnMouseDown(msg.wParam, GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
		break;
	case Luxko::Application::Message::UsefulWindowMessage::LeftButtonUp:
	case Luxko::Application::Message::UsefulWindowMessage::MidButtonUp:
	case Luxko::Application::Message::UsefulWindowMessage::RightButtonUp:
	case Luxko::Application::Message::UsefulWindowMessage::XButtonUp:
		OnMouseUp(msg.wParam, GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
		break;

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
	case Luxko::Application::Message::UsefulWindowMessage::MouseMoving:
		OnMouseMove(msg.wParam, GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));
		break;
	case Luxko::Application::Message::UsefulWindowMessage::MouseHovering:
		break;
	case Luxko::Application::Message::UsefulWindowMessage::MouseLeft:
		break;
	case Luxko::Application::Message::UsefulWindowMessage::NCHitTest:
		break;
	case Luxko::Application::Message::UsefulWindowMessage::HorizontalMouseWheel:
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
		break;
	case Luxko::Application::Message::UsefulWindowMessage::KeyUp:
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
	if (_d3d12Device) {
		FlushCommandQueue();
	}
}

void Luxko::Anuthur::D3D12App::CreateMainCmdObjects()
{
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	ThrowIfFailed(_d3d12Device->CreateCommandQueue(&cmdQueueDesc,
		IID_PPV_ARGS(&_cmdQueue)),
		L"Main command queue initialization failed.");

	ThrowIfFailed(_d3d12Device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_mainCmdAllocator)),
		L"Main command allocator initialization failed.");

	ThrowIfFailed(_d3d12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		_mainCmdAllocator.Get(), nullptr, IID_PPV_ARGS(&_mainCmdList)),
		L"Main command list initialization failed.");

	ThrowIfFailed(_mainCmdList->Close());
}

void Luxko::Anuthur::D3D12App::CreateMainDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	ThrowIfFailed(_d3d12Device->CreateDescriptorHeap(
		&rtvHeapDesc, IID_PPV_ARGS(&_mainRTVHeap)),
		L"Main render target view heap creation failed.");

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	ThrowIfFailed(_d3d12Device->CreateDescriptorHeap(
		&dsvHeapDesc, IID_PPV_ARGS(&_mainDSVHeap)),
		L"Main depth stencil view heap creation failed.");
}

void Luxko::Anuthur::D3D12App::SetMainViewport(float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth)
{
	_mainViewport.TopLeftX = topLeftX;
	_mainViewport.TopLeftY = topLeftY;
	_mainViewport.Width = width;
	_mainViewport.Height = height;
	_mainViewport.MinDepth = minDepth;
	_mainViewport.MaxDepth = maxDepth;
}

void Luxko::Anuthur::D3D12App::SetMainScissorRect(LONG left, LONG right, LONG top, LONG bottom)
{
	_mainScissorRect.left = left;
	_mainScissorRect.right = right;
	_mainScissorRect.top = top;
	_mainScissorRect.bottom = bottom;
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

