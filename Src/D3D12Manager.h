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

		class D3D12SwapChainResource {
			friend class D3D12Manager;
		public:
			// Static utility methods

		private:
			// Ctors
			D3D12SwapChainResource() = default;
			D3D12SwapChainResource(const D3D12SwapChainResource&) = delete;
			D3D12SwapChainResource& operator=(const D3D12SwapChainResource&) = delete;

		public:
			// Initializers
			//void ResetSwapChain(IDXGIFactory4* pFactory, const Luxko::Application::BaseApp& targetApp,
			//	BOOL windowed = TRUE, UINT SampleCount = 1, UINT SampleQuality = 0);
			//void ResetSwapChain(IDXGIFactory4* pFactory, const Luxko::Application::BaseApp& targetApp,
			//	DXGI_FORMAT backBufferFormat, BOOL windowed = TRUE, UINT SampleCount = 1, UINT SampleQuality = 0);

			// Dtor
			~D3D12SwapChainResource() = default;

			// Modifier
			void Resize(UINT width, UINT height);

			// Queries
			inline auto SwapChainBufferCount()const noexcept { return _swapChainBuffers.size(); }
			inline auto BackBufferFormat()const noexcept { return _backBufferFormat; }
			inline auto operator->() const throw() { return _swapChain.operator ->(); }
			inline const auto& SwapChainBuffers()const noexcept { return _swapChainBuffers; }
			inline auto GetCPURTVHandleFromHeapStart()const noexcept { return _backBufferViews->GetCPUDescriptorHandleForHeapStart(); }
			inline auto GetGPURTVHandleFromHeapStart()const noexcept { return _backBufferViews->GetGPUDescriptorHandleForHeapStart(); }

		private:
			// Data members
			ComPtr<IDXGISwapChain> _swapChain;
			std::vector<ComPtr<ID3D12Resource>> _swapChainBuffers;
			ComPtr<ID3D12DescriptorHeap> _backBufferViews;
			DXGI_FORMAT _backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			UINT _currentBackBufferIndex;
		};


		class D3D12Manager {
		public:
			// Ctors
			D3D12Manager() = default;
			D3D12Manager(const D3D12Manager&) = delete;
			D3D12Manager& operator=(const D3D12Manager&) = delete;

			// Dtors
			~D3D12Manager() = default;

			// Initializer
			void Initialize(const Application::BaseApp& targetApp, BOOL windowed, DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM, UINT backBufferCount = 2, UINT sampleCount = 1, UINT sampleQuality = 0);

			// Modifiers, return a UUID for the created resource. If the name already exist,
			// the former resource would be replaced by the new one, the ID would be reused.
			// Names can be identical across different types of resources, while the ID would
			// be universally unique.
			UINT64 AddBuffer(const D3D12Helper::ResourceHeapProperties& heapProperties,
				D3D12_HEAP_FLAGS heapFlags, const D3D12Helper::ResourceDescriptor& resourceDescription,
				D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COMMON,
				const D3D12_CLEAR_VALUE* pOptimizedClearValue = nullptr, const char* name = nullptr);

			UINT64 AddTexture1D(const D3D12Helper::ResourceHeapProperties& heapProperties,
				D3D12_HEAP_FLAGS heapFlags, const D3D12Helper::ResourceDescriptor& resourceDescription,
				D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COMMON,
				const D3D12_CLEAR_VALUE* pOptimizedClearValue = nullptr, const char* name = nullptr);

			UINT64 AddTexture2D(const D3D12Helper::ResourceHeapProperties& heapProperties,
				D3D12_HEAP_FLAGS heapFlags, const D3D12Helper::ResourceDescriptor& resourceDescription,
				D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COMMON,
				const D3D12_CLEAR_VALUE* pOptimizedClearValue = nullptr, const char* name = nullptr);

			UINT64 AddTexture3D(const D3D12Helper::ResourceHeapProperties& heapProperties,
				D3D12_HEAP_FLAGS heapFlags, const D3D12Helper::ResourceDescriptor& resourceDescription,
				D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COMMON,
				const D3D12_CLEAR_VALUE* pOptimizedClearValue = nullptr, const char* name = nullptr);

			UINT64 AddDescriptorHeap(const D3D12Helper::DescriptorHeapDescriptor& desc, const char* name = nullptr);

			UINT64 AddRootSignature(const D3D12Helper::RootSignatureDescriptor& rsd,
				D3D12_ROOT_SIGNATURE_FLAGS flags, UINT nodeMask = 0u, const char* name = nullptr);

			UINT64 AddPSO(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& gpsd, const char* name = nullptr);

			UINT64 AddShader(const wchar_t* shaderFilePathName, const char* entryPoint,
				const char* target, const char* shaderName = nullptr,
				const D3D_SHADER_MACRO* pDefines = nullptr,
				ID3DInclude* include = D3D_COMPILE_STANDARD_FILE_INCLUDE);

			UINT64 AddIBD(D3D12_GPU_VIRTUAL_ADDRESS bufferLocation, UINT sizeInBytes,
				DXGI_FORMAT format = DXGI_FORMAT_R16_UINT, const char* name = nullptr);

			UINT64 AddVBD(const char* name = nullptr);
			UINT64 AddCommandAllocator(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT,
				const char* name = nullptr);

			UINT64 AddCommandList(D3D12_COMMAND_LIST_TYPE type, ID3D12CommandAllocator* pAllocator,
				UINT nodeMask = 0u, ID3D12PipelineState* pInitialState = nullptr, const char* name = nullptr);

			inline auto RemoveBuffer(UINT64 id) { return _buffers.erase(id); }
			inline auto RemoveBuffer(const std::string& name) { return RemoveBuffer(_bufferIDTable[name]); }

			inline auto RemoveTexture1D(UINT64 id) { return _texture1Ds.erase(id); }
			inline auto RemoveTexture1D(const std::string& name) { return RemoveTexture1D(_texture1DIDTable[name]); }

			inline auto RemoveTexture2D(UINT64 id) { return _texture2Ds.erase(id); }
			inline auto RemoveTexture2D(const std::string& name) { return RemoveTexture2D(_texture2DIDTable[name]); }

			inline auto RemoveTexture3D(UINT64 id) { return _texture3Ds.erase(id); }
			inline auto RemoveTexture3D(const std::string& name) { return RemoveTexture3D(_texture3DIDTable[name]); }

			inline auto RemoveDescriptorHeap(UINT64 id) { return _descriptorHeaps.erase(id); }
			inline auto RemoveDescriptorHeap(const std::string& name) { return RemoveDescriptorHeap(_descriptorHeapIDTable[name]); }

			inline auto RemoveRootSignature(UINT64 id) { return _rootSignatures.erase(id); }
			inline auto RemoveRootSignature(const std::string& name) { return RemoveRootSignature(_rootSignatureIDTable[name]); }

			inline auto RemovePSO(UINT64 id) { return _PSOs.erase(id); }
			inline auto RemovePSO(const std::string& name) { return RemovePSO(_psoIDTable[name]); }

			inline auto RemoveShader(UINT64 id) { return _shaders.erase(id); }
			inline auto RemoveShader(const std::string& name) { return RemoveShader(_shaderIDTable[name]); }

			inline auto RemoveIBD(UINT64 id) { return _ibds.erase(id); }
			inline auto RemoveIBD(const std::string& name) { return RemoveIBD(_ibdIDTable[name]); }

			inline auto RemoveVBD(UINT64 id) { return _vbds.erase(id); }
			inline auto RemoveVBD(const std::string& name) { return RemoveVBD(_vbdIDTable[name]); }

			inline auto RemoveCommandAllocator(UINT64 id) { return _additionalCmdAllocators.erase(id); }
			inline auto RemoveCommandAllocator(const std::string& name) { return RemoveCommandAllocator(_additionalCmdAllocatorIDTable[name]); }

			inline auto RemoveCommandList(UINT64 id) { return _additionalCmdLists.erase(id); }
			inline auto RemoveCommandList(const std::string& name) { return RemoveCommandList(_additionalCmdListIDTable[name]); }

			inline void IncrementMainFenceCount()noexcept { _mainFenceCount++; }


			// Queries
			inline auto& FindBuffer(UINT64 id) { return _buffers[id]; }
			inline auto& FindBuffer(const std::string& name) { return FindBuffer(_bufferIDTable[name]); }
			inline bool HaveBuffer(const std::string& name)const noexcept { return _bufferIDTable.find(name) != _bufferIDTable.end(); }

			inline auto& FindTexture1D(UINT64 id) { return _texture1Ds[id]; }
			inline auto& FindTexture1D(const std::string& name) { return FindTexture1D(_texture1DIDTable[name]); }
			inline bool HaveTexture1D(const std::string& name)const { return _texture1DIDTable.find(name) != _texture1DIDTable.end(); }

			inline auto& FindTexture2D(UINT64 id) { return _texture2Ds[id]; }
			inline auto& FindTexture2D(const std::string& name) { return FindTexture2D(_texture2DIDTable[name]); }
			inline bool HaveTexture2D(const std::string& name)const noexcept { return _texture2DIDTable.find(name) != _texture2DIDTable.end(); }

			inline auto& FindTexture3D(UINT64 id) { return _texture3Ds[id]; }
			inline auto& FindTexture3D(const std::string& name) { return FindTexture3D(_texture3DIDTable[name]); }
			inline bool HaveTexture3D(const std::string& name)const noexcept { return _texture3DIDTable.find(name) != _texture3DIDTable.end(); }

			inline auto& FindDescriptorHeap(UINT64 id) { return _descriptorHeaps[id]; }
			inline auto& FindDescriptorHeap(const std::string& name) { return FindDescriptorHeap(_descriptorHeapIDTable[name]); }
			inline bool HaveDescriptorHeap(const std::string& name)const noexcept { return _descriptorHeapIDTable.find(name) != _descriptorHeapIDTable.end(); }

			inline auto& FindRootSignature(UINT64 id) { return _rootSignatures[id]; }
			inline auto& FindRootSignature(const std::string& name) { return FindRootSignature(_rootSignatureIDTable[name]); }
			inline bool HaveRootSignature(const std::string& name)const noexcept { return _rootSignatureIDTable.find(name) != _rootSignatureIDTable.end(); }

			inline auto& FindPSO(UINT64 id) { return _PSOs[id]; }
			inline auto& FindPSO(const std::string& name) { return FindPSO(_psoIDTable[name]); }
			inline bool HavePSO(const std::string& name)const noexcept { return _psoIDTable.find(name) != _psoIDTable.end(); }

			inline auto& FindShader(UINT64 id) { return _shaders[id]; }
			inline auto& FindShader(const std::string& name) { return FindShader(_shaderIDTable[name]); }
			inline bool HaveShader(const std::string& name)const noexcept { return _shaderIDTable.find(name) != _shaderIDTable.end(); }

			inline auto& FindIBD(UINT64 id) { return _ibds[id]; }
			inline auto& FindIBD(const std::string& name) { return FindIBD(_ibdIDTable[name]); }
			inline bool HaveIBD(const std::string& name)const noexcept { return _ibdIDTable.find(name) != _ibdIDTable.end(); }

			inline auto& FindVBD(UINT64 id) { return _vbds[id]; }
			inline auto& FindVBD(const std::string& name) { return FindVBD(_vbdIDTable[name]); }
			inline bool HaveVBD(const std::string& name)const noexcept { return _vbdIDTable.find(name) != _vbdIDTable.end(); }

			inline auto& FindCommandAllocator(UINT64 id) { return _additionalCmdAllocators[id]; }
			inline auto& FindCommandAllocator(const std::string& name) { return FindCommandAllocator(_additionalCmdAllocatorIDTable[name]); }
			inline bool HaveCommandAllocator(const std::string& name)const noexcept { return _additionalCmdAllocatorIDTable.find(name) != _additionalCmdAllocatorIDTable.end(); }

			inline auto& FindCommandList(UINT64 id) { return _additionalCmdLists[id]; }
			inline auto& FindCommandList(const std::string& name) { return FindCommandList(_additionalCmdListIDTable[name]); }
			inline bool HaveCommandList(const std::string& name)const noexcept { return _additionalCmdListIDTable.find(name) != _additionalCmdListIDTable.end(); }

			inline auto GetDXGIFactory()const noexcept { return _dxgiFactory.Get(); }
			inline auto GetD3D12Device()const noexcept { return _d3d12Device.Get(); }
			inline auto GetMainFence()const noexcept { return _mainFence.Get(); }
			inline auto GetCmdQueue()const noexcept { return _cmdQueue.Get(); }
			inline auto GetMainCmdAllocator()const noexcept { return _mainCmdAllocator.Get(); }
			inline auto GetMainCmdList()const noexcept { return _mainCmdList.Get(); }
			inline auto& GetSwapChainResource()const noexcept { return _swapChainResource; }
			inline auto GetRTVDescriptorSize()const noexcept { return _rtvDescriptorSize; }
			inline auto GetSamplerDescriptorSize()const noexcept { return _samplerDescriptorSize; }
			inline auto GetDSVDescriptorSize()const noexcept { return _dsvDescriptorSize; }
			inline auto GetCbvSrvUavDescriptorSize()const noexcept { return _cbvSrvUavDescriptorSize; }
			inline auto GetMainFenceCount()const noexcept { return _mainFenceCount; }

			D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentSwapChainBackBufferView()const;

			// Properties
			inline auto& GetMainViewport()noexcept { return _mainViewport; }
			inline auto& GetMainScissor()noexcept { return _mainScissor; }


			// Utilities
			void ResetSwapChain(const Application::BaseApp& targetApp, BOOL windowed, UINT sampleCount = 1, UINT sampleQuality = 0);
			void ResetSwapChainWithConfigurationChange(const Application::BaseApp& targetApp, BOOL windowed, DXGI_FORMAT format, UINT backBufferCount, UINT sampleCount = 1, UINT sampleQuality = 0);
			void ResizeSwapChain(UINT width, UINT height, DXGI_FORMAT backBufferFormat);
			void ResetMainViewport(float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth);
			void ResetMainScissor(long left, long right, long top, long bottom);
			
			void FlushCommandQueue();

		private:
			// Helper functions
			UINT64 GenerateIDByName(const char* name, HashTable<std::string, UINT64>& container) noexcept;

		private:

			// Data members
			ComPtr<IDXGIFactory4>				_dxgiFactory;
			ComPtr<ID3D12Device>				_d3d12Device;
			ComPtr<ID3D12Fence>					_mainFence;
			ComPtr<ID3D12CommandQueue>			_cmdQueue;
			ComPtr<ID3D12CommandAllocator>		_mainCmdAllocator;
			ComPtr<ID3D12GraphicsCommandList>	_mainCmdList;
			D3D12SwapChainResource				_swapChainResource;
			D3D12_VIEWPORT						_mainViewport;
			D3D12_RECT							_mainScissor;

			UINT								_rtvDescriptorSize;
			UINT								_samplerDescriptorSize;
			UINT								_dsvDescriptorSize;
			UINT								_cbvSrvUavDescriptorSize;

			HashTable<std::string, UINT64>		_bufferIDTable;
			HashTable<std::string, UINT64>		_texture1DIDTable;
			HashTable<std::string, UINT64>		_texture2DIDTable;
			HashTable<std::string, UINT64>		_texture3DIDTable;
			HashTable<std::string, UINT64>		_descriptorHeapIDTable;
			HashTable<std::string, UINT64>		_rootSignatureIDTable;
			HashTable<std::string, UINT64>		_psoIDTable;
			HashTable<std::string, UINT64>		_shaderIDTable;
			HashTable<std::string, UINT64>		_ibdIDTable;
			HashTable<std::string, UINT64>		_vbdIDTable;
			HashTable<std::string, UINT64>		_additionalCmdAllocatorIDTable;
			HashTable<std::string, UINT64>		_additionalCmdListIDTable;

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