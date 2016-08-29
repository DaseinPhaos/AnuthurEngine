//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
#pragma once
#include "RendererPCH.h"
#include "D3D12HelperMethods.h"
#include "Timer.h"
#include "ApplicationBase.h"
#include "D3D12HelperMethods.h"
#include <unordered_map>

namespace Luxko {
	namespace Anuthur {
		using Microsoft::WRL::ComPtr;

		template<class KT, class VT>
		using HashTable = std::unordered_map<KT, VT>;

		class D3D12SwapChain {
			friend class D3D12Renderer;
		private:
			// Ctors
			D3D12SwapChain() = default;
			D3D12SwapChain(const D3D12SwapChain&) = delete;
			D3D12SwapChain& operator=(const D3D12SwapChain&) = delete;

		public:
			// Initializers
			void ResetSwapChain(IDXGIFactory4* pFactory, const Luxko::Application::BaseApp& targetApp,
				BOOL windowed = TRUE, UINT SampleCount = 1, UINT SampleQuality = 0);
			void ResetSwapChain(IDXGIFactory4* pFactory, const Luxko::Application::BaseApp& targetApp,
				DXGI_FORMAT backBufferFormat, BOOL windowed = TRUE, UINT SampleCount = 1, UINT SampleQuality = 0);

			// Dtor
			~D3D12SwapChain() = default;

			// Modifier
			void Resize(UINT width, UINT height);

			// Queries
			inline auto SwapChainBufferCount()const noexcept { return _swapChainBuffers.size(); }
			inline auto BackBufferFormat()const noexcept { return _backBufferFormat; }
			inline auto operator->() const throw() { return _swapChain.operator ->(); }
			inline const auto& SwapChainBuffers()const noexcept { return _swapChainBuffers; }

		private:
			// Data members
			ComPtr<IDXGISwapChain> _swapChain;
			std::vector<ComPtr<ID3D12Resource>> _swapChainBuffers;
			DXGI_FORMAT _backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		};


		class D3D12Renderer {
		public:
			// Ctors
			D3D12Renderer()=default;
			D3D12Renderer(const D3D12Renderer&) = delete;
			D3D12Renderer& operator=(const D3D12Renderer&) = delete;
			
			// Dtors
			~D3D12Renderer() = default;

			// Initializer
			void Initialize(const Luxko::Application::BaseApp& appPtr);

			// Modifiers, return a UUID for the created resource.
			UINT64 AddBuffer(const D3D12_RESOURCE_DESC& bufferDescription, const D3D12_HEAP_PROPERTIES& heapProperties, const char* name = nullptr);
			UINT64 AddTexture1D(const D3D12_RESOURCE_DESC& bufferDescription, const D3D12_HEAP_PROPERTIES& heapProperties, const char* name = nullptr);
			UINT64 AddTexture2D(const D3D12_RESOURCE_DESC& bufferDescription, const D3D12_HEAP_PROPERTIES& heapProperties, const char* name = nullptr);
			UINT64 AddTexture3D(const D3D12_RESOURCE_DESC& bufferDescription, const D3D12_HEAP_PROPERTIES& heapProperties, const char* name = nullptr);
			UINT64 AddDescriptorHeap(const D3D12_DESCRIPTOR_HEAP_DESC& desc, const char* name = nullptr);
			UINT64 AddRootSignature(const D3D12Helper::RootSignatureDescriptor& rsd, UINT nodeMask = 0u, const char* name = nullptr);
			UINT64 AddPSO(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& gpsd, const char* name = nullptr);
			UINT64 AddShader(const wchar_t* shaderFilePathName, const char* entryPoint, const char* target, const char* shaderName = nullptr,
				const D3D_SHADER_MACRO* pDefines = nullptr, ID3DInclude* include = D3D_COMPILE_STANDARD_FILE_INCLUDE);
			UINT64 AddIBD(D3D12_GPU_VIRTUAL_ADDRESS bufferLocation, UINT sizeInBytes,
				DXGI_FORMAT format = DXGI_FORMAT_R16_UINT, const char* name = nullptr);
			UINT64 AddVBD(const char* name = nullptr);
			UINT64 AddCommandAllocator(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT, const char* name = nullptr);
			UINT64 AddCommandList(D3D12_COMMAND_LIST_TYPE type, ID3D12CommandAllocator* pAllocator,
				UINT nodeMask = 0u, ID3D12PipelineState* pInitialState = nullptr, const char* name = nullptr);

			inline auto RemoveBuffer(UINT64 id) { return _buffers.erase(id); }
			inline auto RemoveBuffer(const std::string& name) { return RemoveBuffer(_idTable[name]); }
			
			inline auto RemoveTexture1D(UINT64 id) { return _texture1Ds.erase(id); }
			inline auto RemoveTexture1D(const std::string& name) { return RemoveTexture1D(_idTable[name]); }
			
			inline auto RemoveTexture2D(UINT64 id) { return _texture2Ds.erase(id); }
			inline auto RemoveTexture2D(const std::string& name) { return RemoveTexture2D(_idTable[name]); }

			inline auto RemoveTexture3D(UINT64 id) { return _texture3Ds.erase(id); }
			inline auto RemoveTexture3D(const std::string& name) { return RemoveTexture3D(_idTable[name]); }

			inline auto RemoveDescriptorHeap(UINT64 id) { return _descriptorHeaps.erase(id); }
			inline auto RemoveDescriptorHeap(const std::string& name) { return RemoveDescriptorHeap(_idTable[name]); }
			
			inline auto RemoveRootSignature(UINT64 id) { return _rootSignatures.erase(id); }
			inline auto RemoveRootSignature(const std::string& name) { return RemoveRootSignature(_idTable[name]); }
			
			inline auto RemovePSO(UINT64 id) { return _PSOs.erase(id); }
			inline auto RemovePSO(const std::string& name) { return RemovePSO(_idTable[name]); }

			inline auto RemoveShader(UINT64 id) { return _shaders.erase(id); }
			inline auto RemoveShader(const std::string& name) { return RemoveShader(_idTable[name]); }
			
			inline auto RemoveIBD(UINT64 id) { return _ibds.erase(id); }
			inline auto RemoveIBD(const std::string& name) { return RemoveIBD(_idTable[name]); }
			
			inline auto RemoveVBD(UINT64 id) { return _vbds.erase(id); }
			inline auto RemoveVBD(const std::string& name) { return RemoveVBD(_idTable[name]); }
			
			inline auto RemoveCommandAllocator(UINT64 id) { return _additionalCmdAllocators.erase(id); }
			inline auto RemoveCommandAllocator(const std::string& name) { return RemoveCommandAllocator(_idTable[name]); }
			
			inline auto RemoveCommandList(UINT64 id) { return _additionalCmdLists.erase(id); }
			inline auto RemoveCommandList(const std::string& name) { return RemoveCommandList(_idTable[name]); }
			

			// Queries
			inline auto& FindBuffer(UINT64 id) { return _buffers[id]; }
			inline auto& FindBuffer(const std::string& name) { return FindBuffer(_idTable[name]); }

			inline auto& FindTexture1D(UINT64 id) { return _texture1Ds[id]; }
			inline auto& FindTexture1D(const std::string& name) { return FindTexture1D(_idTable[name]); }

			inline auto& FindTexture2D(UINT64 id) { return _texture2Ds[id]; }
			inline auto& FindTexture2D(const std::string& name) { return FindTexture2D(_idTable[name]); }

			inline auto& FindTexture3D(UINT64 id) { return _texture3Ds[id]; }
			inline auto& FindTexture3D(const std::string& name) { return FindTexture3D(_idTable[name]); }

			inline auto& FindDescriptorHeap(UINT64 id) { return _descriptorHeaps[id]; }
			inline auto& FindDescriptorHeap(const std::string& name) { return FindDescriptorHeap(_idTable[name]); }

			inline auto& FindRootSignature(UINT64 id) { return _rootSignatures[id]; }
			inline auto& FindRootSignature(const std::string& name) { return FindRootSignature(_idTable[name]); }

			inline auto& FindPSO(UINT64 id) { return _PSOs[id]; }
			inline auto& FindPSO(const std::string& name) { return FindPSO(_idTable[name]); }

			inline auto& FindShader(UINT64 id) { return _shaders[id]; }
			inline auto& FindShader(const std::string& name) { return FindShader(_idTable[name]); }

			inline auto& FindIBD(UINT64 id) { return _ibds[id]; }
			inline auto& FindIBD(const std::string& name) { return FindIBD(_idTable[name]); }

			inline auto& FindVBD(UINT64 id) { return _vbds[id]; }
			inline auto& FindVBD(const std::string& name) { return FindVBD(_idTable[name]); }

			inline auto& FindCommandAllocator(UINT64 id) { return _additionalCmdAllocators[id]; }
			inline auto& FindCommandAllocator(const std::string& name) { return FindCommandAllocator(_idTable[name]); }

			inline auto& FindCommandList(UINT64 id) { return _additionalCmdLists[id]; }
			inline auto& FindCommandList(const std::string& name) { return FindCommandList(_idTable[name]); }

		private:

			// Data members
			ComPtr<IDXGIFactory4>				_dxgiFactory;
			ComPtr<ID3D12Device>				_d3d12Device;
			ComPtr<ID3D12Fence>					_mainFence;
			ComPtr<ID3D12CommandQueue>			_cmdQueue;
			ComPtr<ID3D12CommandAllocator>		_mainCmdAllocator;
			ComPtr<ID3D12GraphicsCommandList>	_mainCmdList;
			D3D12SwapChain						_swapChain;
			D3D12_VIEWPORT						_mainViewport;
			D3D12_RECT							_mainScissor;

			UINT								_rtvDescriptorSize;
			UINT								_samplerDescriptorSize;
			UINT								_dsvDescriptorSize;
			UINT								_cbvSrvUavDescriptorSize;

			HashTable<std::string, UINT64>		_idTable;

			HashTable<UINT64, ComPtr<ID3D12Resource>> _buffers;
			HashTable<UINT64, ComPtr<ID3D12Resource>> _texture1Ds;
			HashTable<UINT64, ComPtr<ID3D12Resource>> _texture2Ds;
			HashTable<UINT64, ComPtr<ID3D12Resource>> _texture3Ds;
			HashTable<UINT64, ComPtr<ID3D12DescriptorHeap>> _descriptorHeaps;

			HashTable<UINT64, ComPtr<ID3D12RootSignature>> _rootSignatures;
			HashTable<UINT64, ComPtr<ID3D12PipelineState>> _PSOs;
			HashTable<UINT64, D3D12Helper::ShaderByteCode> _shaders;

			HashTable<UINT64, D3D12Helper::IndexBufferDescriptor> _ibds;
			HashTable<UINT64, D3D12Helper::VertexBuffersDescriptor> _vbds;

			HashTable<UINT64, ComPtr<ID3D12CommandAllocator>> _additionalCmdAllocators;
			HashTable<UINT64, ComPtr<ID3D12GraphicsCommandList>> _additionalCmdLists;
			
			UINT64								_nextID = 0ull;
			UINT64								_mainFenceCount = 0ull;
			

		};
	}
}