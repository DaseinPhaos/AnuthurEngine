//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
#pragma once
#include "RendererPCH.h"
#include "HelperMethods.h"
#include "Timer.h"

namespace Luxko {
	namespace Anuthur {
		constexpr auto SwapChainBufferCount = 2u;
		using Microsoft::WRL::ComPtr;

		class ANUTHURRENDERER_API D3D12App abstract : public Luxko::Application::BaseApp {
		public:
			D3D12App(UINT width, UINT height, const std::wstring& name) :
				BaseApp(width, height, name) {}
			D3D12App(const D3D12App&) = delete;
			D3D12App& operator=(const D3D12App&) = delete;
			virtual ~D3D12App();

		protected:
			virtual void OnInit() override;

			virtual void OnDestroy() override;

			virtual bool OnEvent(MSG msg) override;

			// This method is called when OnEvent receives a WM_SIZE message.
			virtual void OnResize();

			virtual void OnMouseDown(WPARAM wParam, int x, int y) {}

			virtual void OnMouseUp(WPARAM wParam, int x, int y) {}

			virtual void OnMouseMove(WPARAM wParam, int x, int y) {}

			void BasicD3D12ElementsInitialization();

			void ResetSwapChain(BOOL windowed = TRUE);

			void CreateMainCmdObjects();

			void CreateMainDescriptorHeaps();

			void FlushCommandQueue();

			void LogFPSToTitle(); // Called every OnRender() to display fps on windows title.

			void SetMainViewport(float topLeftX, float topLeftY, float width, float height,
				float minDepth, float maxDepth);

			void SetMainScissorRect(LONG left, LONG right, LONG top, LONG bottom);

			D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView()const;
			//D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentDepthStencilBiew()const;

			Luxko::Timer						_mainTimer;
			BOOL								_appPaused=false;
			
			ComPtr<IDXGIFactory4>				_dxgiFactory;
			//ComPtr<IDXGIAdapter3>				_hardwareAdapter;
			ComPtr<ID3D12Device>				_d3d12Device;
			ComPtr<ID3D12Fence>					_mainFence;
			ComPtr<ID3D12CommandQueue>			_cmdQueue;
			ComPtr<ID3D12CommandAllocator>		_mainCmdAllocator;
			ComPtr<ID3D12GraphicsCommandList>	_mainCmdList;
			ComPtr<IDXGISwapChain>				_swapChain;
			ComPtr<ID3D12Resource>				_swapChainBuffer[SwapChainBufferCount];
			ComPtr<ID3D12Resource>				_depthStencilBuffer;
			ComPtr<ID3D12DescriptorHeap>		_mainRTVHeap;
			ComPtr<ID3D12DescriptorHeap>		_mainDSVHeap;
			UINT								_rtvDescriptorSize;
			UINT								_samplerDescriptorSize;
			UINT								_dsvDescriptorSize;
			UINT								_cbvSrvUavDescriptorSize;
			UINT								_currentBackBufferIndex = 0;
			UINT64								_currentMainFenceCount = 0;
			DXGI_FORMAT							_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			DXGI_FORMAT							_depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
			D3D12_VIEWPORT						_mainViewport;
			D3D12_RECT							_mainScissorRect;
		};
	}
}