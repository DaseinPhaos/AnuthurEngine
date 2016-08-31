//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
#include "D3D12Manager.h"
#include "D3D12HelperMethods.h"


void Luxko::Anuthur::D3D12Manager::Initialize(const Luxko::Application::BaseApp& appPtr)
{

}

UINT64 Luxko::Anuthur::D3D12Manager::AddBuffer(const D3D12Helper::ResourceDescriptor& bufferDescription, const D3D12Helper::ResourceHeapProperties& heapProperties, const char* name /*= nullptr*/)
{

}

UINT64 Luxko::Anuthur::D3D12Manager::AddTexture1D(const D3D12Helper::ResourceDescriptor& bufferDescription, const D3D12Helper::ResourceHeapProperties& heapProperties, const char* name /*= nullptr*/)
{

}

UINT64 Luxko::Anuthur::D3D12Manager::AddTexture2D(const D3D12Helper::ResourceDescriptor& bufferDescription, const D3D12Helper::ResourceHeapProperties& heapProperties, const char* name /*= nullptr*/)
{

}

UINT64 Luxko::Anuthur::D3D12Manager::AddTexture3D(const D3D12Helper::ResourceDescriptor& bufferDescription, const D3D12Helper::ResourceHeapProperties& heapProperties, const char* name /*= nullptr*/)
{

}

UINT64 Luxko::Anuthur::D3D12Manager::AddDescriptorHeap(const D3D12Helper::DescriptorHeapDescriptor& desc, const char* name /*= nullptr*/)
{

}

UINT64 Luxko::Anuthur::D3D12Manager::AddRootSignature(const D3D12Helper::RootSignatureDescriptor& rsd, UINT nodeMask /*= 0u*/, const char* name /*= nullptr*/)
{

}

UINT64 Luxko::Anuthur::D3D12Manager::AddPSO(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& gpsd, const char* name /*= nullptr*/)
{

}

UINT64 Luxko::Anuthur::D3D12Manager::AddShader(const wchar_t* shaderFilePathName, const char* entryPoint, const char* target, const char* shaderName /*= nullptr*/, const D3D_SHADER_MACRO* pDefines /*= nullptr*/, ID3DInclude* include /*= D3D_COMPILE_STANDARD_FILE_INCLUDE*/)
{

}

UINT64 Luxko::Anuthur::D3D12Manager::AddIBD(D3D12_GPU_VIRTUAL_ADDRESS bufferLocation, UINT sizeInBytes, DXGI_FORMAT format /*= DXGI_FORMAT_R16_UINT*/, const char* name /*= nullptr*/)
{

}

UINT64 Luxko::Anuthur::D3D12Manager::AddVBD(const char* name /*= nullptr*/)
{

}

UINT64 Luxko::Anuthur::D3D12Manager::AddCommandAllocator(D3D12_COMMAND_LIST_TYPE type /*= D3D12_COMMAND_LIST_TYPE_DIRECT*/, const char* name /*= nullptr*/)
{

}

UINT64 Luxko::Anuthur::D3D12Manager::AddCommandList(D3D12_COMMAND_LIST_TYPE type, ID3D12CommandAllocator* pAllocator, UINT nodeMask /*= 0u*/, ID3D12PipelineState* pInitialState /*= nullptr*/, const char* name /*= nullptr*/)
{
	UINT64 id;
	if (name) {
		auto found = _additionalCmdListIDTable.find(name);
		if (found != _additionalCmdListIDTable.end()) id = (*found).second;
		else {
			id = ++_nextID;
			_additionalCmdListIDTable[name] = id;
		}
	}
	else id = ++_nextID;

	_additionalCmdLists[id] = nullptr;
	ThrowIfFailed(_d3d12Device->CreateCommandList(nodeMask, type, pAllocator, pInitialState, IID_PPV_ARGS(_additionalCmdLists[id].GetAddressOf())));
}

void Luxko::Anuthur::D3D12Manager::ResetSwapChain(const Application::BaseApp& targetApp, BOOL windowed, UINT sampleCount /*= 1*/, UINT sampleQuality /*= 0*/)
{
	for (auto swapChainBuffer : _swapChainResource._swapChainBuffers) swapChainBuffer.Reset();
	_swapChainResource._swapChain.Reset();

	auto swapChainDesc = D3D12Helper::SwapChainDescriptor(targetApp.Width(), targetApp.Height(), targetApp.WindowHandle(), _swapChainResource._backBufferFormat, _swapChainResource._swapChainBuffers.size(), windowed, sampleCount, sampleQuality);
	ThrowIfFailed(_dxgiFactory->CreateSwapChain(_cmdQueue.Get(), &swapChainDesc, _swapChainResource._swapChain.GetAddressOf()));

	auto rtvHandle = _swapChainResource._backBufferViews->GetCPUDescriptorHandleForHeapStart();
	for (size_t i = 0; i < _swapChainResource._swapChainBuffers.size(); ++i) {
		ThrowIfFailed(_swapChainResource._swapChain->GetBuffer(static_cast<UINT>(i), IID_PPV_ARGS(_swapChainResource._swapChainBuffers[i].GetAddressOf())));
		_d3d12Device->CreateRenderTargetView(_swapChainResource._swapChainBuffers[i].Get(), nullptr, rtvHandle);
		rtvHandle.ptr += _rtvDescriptorSize;
	}
	_swapChainResource._currentBackBufferIndex = 0u;
}

void Luxko::Anuthur::D3D12Manager::ResetSwapChainWithConfigurationChange(const Application::BaseApp& targetApp, BOOL windowed, DXGI_FORMAT format, UINT backBufferCount, UINT sampleCount /*= 1*/, UINT sampleQuality /*= 0*/)
{
	_swapChainResource._backBufferFormat = format;
	_swapChainResource._swapChainBuffers.clear();
	_swapChainResource._swapChainBuffers.insert(_swapChainResource._swapChainBuffers.end(), static_cast<size_t>(backBufferCount), nullptr);
	
	ResetSwapChain(targetApp, windowed, sampleCount, sampleQuality);
}

void Luxko::Anuthur::D3D12Manager::ResizeSwapChain(UINT width, UINT height, DXGI_FORMAT backBufferFormat)
{
	_swapChainResource._backBufferFormat = backBufferFormat;
	for (auto swapChainBuffer : _swapChainResource._swapChainBuffers) swapChainBuffer.Reset();
	ThrowIfFailed(_swapChainResource._swapChain->ResizeBuffers(static_cast<UINT>(_swapChainResource._swapChainBuffers.size()), width, height, backBufferFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	auto rtvHandle = _swapChainResource._backBufferViews->GetCPUDescriptorHandleForHeapStart();
	for (size_t i = 0; i < _swapChainResource._swapChainBuffers.size(); ++i) {
		ThrowIfFailed(_swapChainResource._swapChain->GetBuffer(static_cast<UINT>(i), IID_PPV_ARGS(_swapChainResource._swapChainBuffers[i].GetAddressOf())));
		_d3d12Device->CreateRenderTargetView(_swapChainResource._swapChainBuffers[i].Get(), nullptr, rtvHandle);
		rtvHandle.ptr += _rtvDescriptorSize;
	}
	_swapChainResource._currentBackBufferIndex = 0u;
}
