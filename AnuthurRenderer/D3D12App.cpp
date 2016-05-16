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
}

void Luxko::Anuthur::D3D12App::BasicD3D12ElementsInitialization()
{
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
		ThrowIfFailed(_d3d12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE,
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

	// Create the swap chain.
	{
		ResetSwapChain();
	}

	// Initialize descriptor heaps.
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

}

void Luxko::Anuthur::D3D12App::ResetSwapChain(BOOL windowed)
{
	_swapChain.Reset();
	DXGI_SWAP_CHAIN_DESC swapchainDesc;
	swapchainDesc.BufferCount = SwapChainBufferCount;
	swapchainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.BufferDesc.Width = _width;
	swapchainDesc.BufferDesc.Height = _height;
	swapchainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapchainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapchainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapchainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapchainDesc.OutputWindow = _hWindow;
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.SampleDesc.Quality = 0;
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapchainDesc.Windowed = windowed;
	ThrowIfFailed(_dxgiFactory->CreateSwapChain(_d3d12Device.Get(), &swapchainDesc,
		&_swapChain), L"Swap chain creation failed.");
}

D3D12_CPU_DESCRIPTOR_HANDLE Luxko::Anuthur::D3D12App::GetCurrentBackBufferView() const
{
	D3D12_CPU_DESCRIPTOR_HANDLE result;
	result.ptr = _mainRTVHeap->GetCPUDescriptorHandleForHeapStart().ptr + _currentBackBufferIndex * _rtvDescriptorSize;
	return result;
}


