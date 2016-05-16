//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
#pragma once
#include "RendererPCH.h"
#include "HelperMethods.h"
namespace Luxko {
	namespace Anuthur {
		constexpr int SwapChainBufferCount = 2;
		using Microsoft::WRL::ComPtr;

		class D3D12App : public Luxko::Application::BaseApp {

		protected:
			virtual void OnInit() override;

			virtual void OnUpdate() override
			{
				throw std::logic_error("The method or operation is not implemented.");
			}

			virtual void OnRender() override
			{
				throw std::logic_error("The method or operation is not implemented.");
			}

			virtual void OnDestroy() override
			{
				throw std::logic_error("The method or operation is not implemented.");
			}

			virtual bool OnEvent(MSG msg) override
			{
				throw std::logic_error("The method or operation is not implemented.");
			}

			void BasicD3D12ElementsInitialization();

			void ResetSwapChain(BOOL windowed = TRUE);


			D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView()const;
			//D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentDepthStencilBiew()const;

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
			UINT								_currentBackBufferIndex;
			DXGI_FORMAT							_depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		};
	}
}
