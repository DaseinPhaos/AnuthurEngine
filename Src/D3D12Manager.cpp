//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
#include "D3D12Manager.h"
#include "D3D12HelperMethods.h"
#include "Threading.h"


D3D12_CPU_DESCRIPTOR_HANDLE Luxko::Anuthur::D3D12WindowResource::GetCurrentCPURTV() const
{
	D3D12_CPU_DESCRIPTOR_HANDLE result;
	result.ptr = _backBufferViews->GetCPUDescriptorHandleForHeapStart().ptr + _currentBackBufferIndex * _manager->GetRTVDescriptorSize();
	return result;
}

void Luxko::Anuthur::D3D12WindowResource::Reset(const Application::BaseApp& targetApp, BOOL windowed, UINT sampleCount /*= 1*/, UINT sampleQuality /*= 0*/)
{
	auto width = targetApp.Width();
	auto height = targetApp.Height();

	// Clear old resources
	for (auto swapChainBuffer : _swapChainBuffers) swapChainBuffer.Reset();
	_swapChain.Reset();

	// reset swapchain
	auto swapChainDesc = D3D12Helper::SwapChainDescriptor(width, height, targetApp.WindowHandle(),
		_backBufferFormat, static_cast<UINT>(_swapChainBuffers.size()), windowed, sampleCount, sampleQuality);
	ThrowIfFailed(_manager->GetDXGIFactory()->CreateSwapChain(_manager->GetCmdQueue(), &swapChainDesc, _swapChain.GetAddressOf()));
	


	// reset back-buffers and back-buffer-views.
	auto rtvHandle = _backBufferViews->GetCPUDescriptorHandleForHeapStart();
	for (size_t i = 0; i < _swapChainBuffers.size(); ++i) {
		ThrowIfFailed(_swapChain->GetBuffer(static_cast<UINT>(i), IID_PPV_ARGS(_swapChainBuffers[i].GetAddressOf())));
		_manager->GetD3D12Device()->CreateRenderTargetView(_swapChainBuffers[i].Get(), nullptr, rtvHandle);
		
		rtvHandle.ptr += _manager->GetRTVDescriptorSize();
	}
	_currentBackBufferIndex = 0u;

	// reset depth-stencil buffer and dsv-heap
	auto rdesc = D3D12Helper::ResourceDescriptor(D3D12_RESOURCE_DIMENSION_TEXTURE2D, width, height,
		1, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, 0, 1, 1, 0,
		_depthStencilFormat, D3D12_TEXTURE_LAYOUT_UNKNOWN);
	auto hdesc = D3D12Helper::ResourceHeapProperties(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_CLEAR_VALUE defaultClearValue;
	defaultClearValue.Format = _depthStencilFormat;
	defaultClearValue.DepthStencil.Depth = 1.f;
	defaultClearValue.DepthStencil.Stencil = 0;
	ThrowIfFailed(_manager->GetD3D12Device()->CreateCommittedResource(&hdesc, D3D12_HEAP_FLAG_NONE, &rdesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE, &defaultClearValue, IID_PPV_ARGS(_depthStencilResource.ReleaseAndGetAddressOf())));
	_manager->GetD3D12Device()->CreateDepthStencilView(_depthStencilResource.Get(),
		nullptr, _dsvHeap->GetCPUDescriptorHandleForHeapStart());

	// reset viewport and scissor-rect
	ResetMainViewport(0.f, 0.f, static_cast<float>(width), static_cast<float>(height), 0.f, 1.f);
	ResetMainScissor(0l, width, 0l, height);

}

void Luxko::Anuthur::D3D12WindowResource::ReconfigureAndReset(size_t backBufferCount, DXGI_FORMAT backBufferFormat, const Application::BaseApp& targetApp, BOOL windowed, UINT sampleCount /*= 1*/, UINT sampleQuality /*= 0*/)
{
	_backBufferFormat = backBufferFormat;
	_swapChainBuffers.clear();
	_swapChainBuffers.insert(_swapChainBuffers.end(), backBufferCount, nullptr);
	ThrowIfFailed(_manager->GetD3D12Device()->CreateDescriptorHeap(
		&D3D12Helper::DescriptorHeapDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
			static_cast<UINT>(_swapChainBuffers.size())), IID_PPV_ARGS(_backBufferViews.ReleaseAndGetAddressOf())));
	ThrowIfFailed(_manager->GetD3D12Device()->CreateDescriptorHeap(
		&D3D12Helper::DescriptorHeapDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_DSV),
		IID_PPV_ARGS(_dsvHeap.ReleaseAndGetAddressOf())));
	Reset(targetApp, windowed, sampleCount, sampleQuality);
}

void Luxko::Anuthur::D3D12WindowResource::Resize(UINT width, UINT height, DXGI_FORMAT backBufferFormat)
{
	_backBufferFormat = backBufferFormat;

	// resize swapchain
	ThrowIfFailed(_swapChain->ResizeBuffers(static_cast<UINT>(_swapChainBuffers.size()), width, height, _backBufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	// reset back-buffers and back-buffer-views.
	auto rtvHandle = _backBufferViews->GetCPUDescriptorHandleForHeapStart();
	for (size_t i = 0; i < _swapChainBuffers.size(); ++i) {
		ThrowIfFailed(_swapChain->GetBuffer(static_cast<UINT>(i), IID_PPV_ARGS(_swapChainBuffers[i].ReleaseAndGetAddressOf())));
		_manager->GetD3D12Device()->CreateRenderTargetView(_swapChainBuffers[i].Get(), nullptr, rtvHandle);
		rtvHandle.ptr += _manager->GetRTVDescriptorSize();
	}
	_currentBackBufferIndex = 0u;

	// reset depth-stencil buffer and dsv-heap
	auto rdesc = D3D12Helper::ResourceDescriptor(D3D12_RESOURCE_DIMENSION_TEXTURE2D, width, height,
		1, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, 0, 1, 1, 0,
		_depthStencilFormat, D3D12_TEXTURE_LAYOUT_UNKNOWN);
	auto hdesc = D3D12Helper::ResourceHeapProperties(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_CLEAR_VALUE defaultClearValue;
	defaultClearValue.Format = _depthStencilFormat;
	defaultClearValue.DepthStencil.Depth = 1.f;
	defaultClearValue.DepthStencil.Stencil = 0;
	ThrowIfFailed(_manager->GetD3D12Device()->CreateCommittedResource(&hdesc, D3D12_HEAP_FLAG_NONE, &rdesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE, &defaultClearValue, IID_PPV_ARGS(_depthStencilResource.ReleaseAndGetAddressOf())));
	_manager->GetD3D12Device()->CreateDepthStencilView(_depthStencilResource.Get(),
		nullptr, _dsvHeap->GetCPUDescriptorHandleForHeapStart());

	// reset viewport and scissor-rect
	ResetMainViewport(0.f, 0.f, static_cast<float>(width), static_cast<float>(height), 0.f, 1.f);
	ResetMainScissor(0l, width, 0l, height);
}

void Luxko::Anuthur::D3D12WindowResource::ResetMainViewport(float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth) noexcept
{
	_mainViewport.TopLeftX = topLeftX;
	_mainViewport.TopLeftY = topLeftY;
	_mainViewport.Width = width;
	_mainViewport.Height = height;
	_mainViewport.MinDepth = minDepth;
	_mainViewport.MaxDepth = maxDepth;
}

void Luxko::Anuthur::D3D12WindowResource::ResetMainScissor(long left, long right, long top, long bottom) noexcept
{
	_mainScissor.left = left;
	_mainScissor.right = right;
	_mainScissor.top = top;
	_mainScissor.bottom = bottom;
}

Luxko::Anuthur::D3D12Manager::~D3D12Manager()
{
	if (_d3d12Device.Get()) {
		FlushCommandQueue();
	}
}

void Luxko::Anuthur::D3D12Manager::Initialize()
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
	
	// Create the main fence, obtain descriptors' sizes.
	{
		ThrowIfFailed(_d3d12Device->CreateFence(_mainFenceCount, D3D12_FENCE_FLAG_NONE,
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

	// Command queue, main command allocator and main command list initialization.
	{
		D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
		cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		ThrowIfFailed(_d3d12Device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(_cmdQueue.GetAddressOf())));
		ThrowIfFailed(_d3d12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(_mainCmdAllocator.GetAddressOf())));
		ThrowIfFailed(_d3d12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _mainCmdAllocator.Get(), nullptr, IID_PPV_ARGS(_mainCmdList.GetAddressOf())));

		ThrowIfFailed(_mainCmdList->Close());
	}

}

UINT64 Luxko::Anuthur::D3D12Manager::AddBuffer(const D3D12Helper::ResourceHeapProperties& heapProperties, D3D12_HEAP_FLAGS heapFlags, const D3D12Helper::ResourceDescriptor& resourceDescription, D3D12_RESOURCE_STATES initialState /*= D3D12_RESOURCE_STATE_COMMON*/, const D3D12_CLEAR_VALUE* pOptimizedClearValue /*= nullptr*/, const char* name /*= nullptr*/)
{
	auto id = GenerateIDByName(name, _bufferIDTable);

	ThrowIfFailed(_d3d12Device->CreateCommittedResource(&heapProperties, heapFlags, &resourceDescription, initialState, pOptimizedClearValue, IID_PPV_ARGS(_buffers[id].ReleaseAndGetAddressOf())));
	return id;
}

UINT64 Luxko::Anuthur::D3D12Manager::AddTexture1D(const D3D12Helper::ResourceHeapProperties& heapProperties, D3D12_HEAP_FLAGS heapFlags, const D3D12Helper::ResourceDescriptor& resourceDescription, D3D12_RESOURCE_STATES initialState /*= D3D12_RESOURCE_STATE_COMMON*/, const D3D12_CLEAR_VALUE* pOptimizedClearValue /*= nullptr*/, const char* name /*= nullptr*/)
{
	auto id = GenerateIDByName(name, _texture1DIDTable);

	ThrowIfFailed(_d3d12Device->CreateCommittedResource(&heapProperties, heapFlags, &resourceDescription, initialState, pOptimizedClearValue, IID_PPV_ARGS(_texture1Ds[id].ReleaseAndGetAddressOf())));
	return id;
}

UINT64 Luxko::Anuthur::D3D12Manager::AddTexture2D(const D3D12Helper::ResourceHeapProperties& heapProperties, D3D12_HEAP_FLAGS heapFlags, const D3D12Helper::ResourceDescriptor& resourceDescription, D3D12_RESOURCE_STATES initialState /*= D3D12_RESOURCE_STATE_COMMON*/, const D3D12_CLEAR_VALUE* pOptimizedClearValue /*= nullptr*/, const char* name /*= nullptr*/)
{
	auto id = GenerateIDByName(name, _texture2DIDTable);

	ThrowIfFailed(_d3d12Device->CreateCommittedResource(&heapProperties, heapFlags, &resourceDescription, initialState, pOptimizedClearValue, IID_PPV_ARGS(_texture2Ds[id].ReleaseAndGetAddressOf())));
	return id;
}

UINT64 Luxko::Anuthur::D3D12Manager::AddTexture3D(const D3D12Helper::ResourceHeapProperties& heapProperties, D3D12_HEAP_FLAGS heapFlags, const D3D12Helper::ResourceDescriptor& resourceDescription, D3D12_RESOURCE_STATES initialState /*= D3D12_RESOURCE_STATE_COMMON*/, const D3D12_CLEAR_VALUE* pOptimizedClearValue /*= nullptr*/, const char* name /*= nullptr*/)
{
	auto id = GenerateIDByName(name, _texture3DIDTable);

	ThrowIfFailed(_d3d12Device->CreateCommittedResource(&heapProperties, heapFlags, &resourceDescription, initialState, pOptimizedClearValue, IID_PPV_ARGS(_texture3Ds[id].ReleaseAndGetAddressOf())));
	return id;
}

UINT64 Luxko::Anuthur::D3D12Manager::AddDescriptorHeap(const D3D12Helper::DescriptorHeapDescriptor& desc, const char* name /*= nullptr*/)
{
	auto id = GenerateIDByName(name, _descriptorHeapIDTable);

	ThrowIfFailed(_d3d12Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(_descriptorHeaps[id].ReleaseAndGetAddressOf())));
	return id;
}

UINT64 Luxko::Anuthur::D3D12Manager::AddRootSignature(const D3D12Helper::RootSignatureDescriptor& rsd, D3D12_ROOT_SIGNATURE_FLAGS flags, UINT nodeMask /*= 0u*/, const char* name /*= nullptr*/)
{
	auto id = GenerateIDByName(name, _rootSignatureIDTable);

	ComPtr<ID3DBlob> serialized;
	ThrowIfFailed(rsd.SerializeRootSignature(serialized.GetAddressOf(), flags, nullptr));
	ThrowIfFailed(_d3d12Device->CreateRootSignature(nodeMask, serialized->GetBufferPointer(),
		serialized->GetBufferSize(), IID_PPV_ARGS(_rootSignatures[id].ReleaseAndGetAddressOf())));
	return id;
}

UINT64 Luxko::Anuthur::D3D12Manager::AddPSO(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& gpsd, const char* name /*= nullptr*/)
{
	auto id = GenerateIDByName(name, _psoIDTable);

	ThrowIfFailed(_d3d12Device->CreateGraphicsPipelineState(&gpsd, IID_PPV_ARGS(_PSOs[id].ReleaseAndGetAddressOf())));
	return id;
}

UINT64 Luxko::Anuthur::D3D12Manager::AddShader(const wchar_t* shaderFilePathName, const char* entryPoint, const char* target, const char* shaderName /*= nullptr*/, const D3D_SHADER_MACRO* pDefines /*= nullptr*/, ID3DInclude* include /*= D3D_COMPILE_STANDARD_FILE_INCLUDE*/)
{
	auto id = GenerateIDByName(shaderName, _shaderIDTable);

	ThrowIfFailed(_shaders[id].CompileFromFile(shaderFilePathName, entryPoint, target, pDefines, include));
	return id;
}

UINT64 Luxko::Anuthur::D3D12Manager::AddIBD(D3D12_GPU_VIRTUAL_ADDRESS bufferLocation, UINT sizeInBytes, DXGI_FORMAT format /*= DXGI_FORMAT_R16_UINT*/, const char* name /*= nullptr*/)
{
	auto id = GenerateIDByName(name, _ibdIDTable);

	_ibds.emplace(id, D3D12Helper::IndexBufferDescriptor(bufferLocation, sizeInBytes, format));
	return id;
}

UINT64 Luxko::Anuthur::D3D12Manager::AddVBD(const char* name /*= nullptr*/)
{
	auto id = GenerateIDByName(name, _vbdIDTable);

	_vbds[id] = D3D12Helper::VertexBuffersDescriptor();
	return id;
}

UINT64 Luxko::Anuthur::D3D12Manager::AddCommandAllocator(D3D12_COMMAND_LIST_TYPE type /*= D3D12_COMMAND_LIST_TYPE_DIRECT*/, const char* name /*= nullptr*/)
{
	auto id = GenerateIDByName(name, _additionalCmdAllocatorIDTable);

	//_additionalCmdAllocators[id];
	ThrowIfFailed(_d3d12Device->CreateCommandAllocator(type, IID_PPV_ARGS(_additionalCmdAllocators[id].ReleaseAndGetAddressOf())));
	return id;
}

UINT64 Luxko::Anuthur::D3D12Manager::AddCommandList(D3D12_COMMAND_LIST_TYPE type, ID3D12CommandAllocator* pAllocator, UINT nodeMask /*= 0u*/, ID3D12PipelineState* pInitialState /*= nullptr*/, const char* name /*= nullptr*/)
{
	auto id = GenerateIDByName(name, _additionalCmdListIDTable);

	//_additionalCmdLists[id];
	ThrowIfFailed(_d3d12Device->CreateCommandList(nodeMask, type, pAllocator, pInitialState, IID_PPV_ARGS(_additionalCmdLists[id].ReleaseAndGetAddressOf())));
	return id;
}

UINT64 Luxko::Anuthur::D3D12Manager::AddWindowResource(const Application::BaseApp& targetApp, BOOL windowed, UINT sampleCount /*= 1*/, UINT sampleQuality /*= 0*/, size_t backBufferCount /*= 2*/, DXGI_FORMAT backBufferFormat /*= DXGI_FORMAT_R8G8B8A8_UNORM*/, const char* name /*= nullptr */)
{
	auto id = GenerateIDByName(name, _windowResourceIDTable);
	_windowResources[id]._manager = this;
	_windowResources[id].ReconfigureAndReset(backBufferCount, backBufferFormat, targetApp, windowed, sampleCount, sampleQuality);
	return id;
}

void Luxko::Anuthur::D3D12Manager::FlushCommandQueue()
{
	_mainFenceCount++;
	ThrowIfFailed(_cmdQueue->Signal(_mainFence.Get(), _mainFenceCount));

	if (_mainFence->GetCompletedValue() < _mainFenceCount) {
		auto e = Threading::Event::Create();
		ThrowIfFailed(_mainFence->SetEventOnCompletion(_mainFenceCount, e.Get().Get()));
		Threading::WaitForSignal(e.Get());
	}
}

void Luxko::Anuthur::D3D12Manager::CreateCBVOnHeap(const D3D12Helper::CBVDescriptor* desc, ID3D12DescriptorHeap* pHeap, UINT viewsCountFromHeapStart)
{
	auto handle = pHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += viewsCountFromHeapStart * _cbvSrvUavDescriptorSize;
	_d3d12Device->CreateConstantBufferView(desc, handle);
}

void Luxko::Anuthur::D3D12Manager::CreateUAVOnHeap(ID3D12Resource* src, const D3D12Helper::UAVDescriptor* desc, ID3D12DescriptorHeap* pHeap, UINT viewsCountFromHeapStart, ID3D12Resource* pCounterResource /*= nullptr*/)
{
	auto handle = pHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += viewsCountFromHeapStart * _cbvSrvUavDescriptorSize;
	_d3d12Device->CreateUnorderedAccessView(src, pCounterResource, desc, handle);
}

void Luxko::Anuthur::D3D12Manager::CreateSRVOnHeap(ID3D12Resource* src, const D3D12Helper::SRVDescriptor* desc, ID3D12DescriptorHeap* pHeap, UINT viewsCountFromHeapStart)
{
	auto handle = pHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += viewsCountFromHeapStart * _cbvSrvUavDescriptorSize;
	_d3d12Device->CreateShaderResourceView(src, desc, handle);
}

void Luxko::Anuthur::D3D12Manager::CreateRTVOnHeap(ID3D12Resource* src, const D3D12Helper::RTVDescriptor* desc, ID3D12DescriptorHeap* pHeap, UINT viewsCountFromHeapStart)
{
	auto handle = pHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += viewsCountFromHeapStart * _rtvDescriptorSize;
	_d3d12Device->CreateRenderTargetView(src, desc, handle);
}

void Luxko::Anuthur::D3D12Manager::CreateDSVOnHeap(ID3D12Resource* src, const D3D12Helper::DSVDescriptor* desc, ID3D12DescriptorHeap* pHeap, UINT viewsCountFromHeapStart)
{
	auto handle = pHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += viewsCountFromHeapStart * _dsvDescriptorSize;
	_d3d12Device->CreateDepthStencilView(src, desc, handle);
}

void Luxko::Anuthur::D3D12Manager::CreateSamplerOnHeap(const D3D12Helper::SamplerDescriptor* desc, ID3D12DescriptorHeap* pHeap, UINT samplersCountFromHeapStart)
{
	auto handle = pHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += samplersCountFromHeapStart * _samplerDescriptorSize;
	_d3d12Device->CreateSampler(desc, handle);
}

UINT64 Luxko::Anuthur::D3D12Manager::GenerateIDByName(const char* name, HashTable<std::string, UINT64>& container) noexcept
{
	UINT64 id;
	if (name) {
		auto found = container.find(name);
		if (found == container.end()) {
			id = ++_nextID;
			container[name] = id;
		}
		else id = found->second;
	}
	else {
		id = ++_nextID;
	}
	return id;
}
