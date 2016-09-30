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
#include "D3D12Manager.h"
#include "DirectXTKInc/Keyboard.h"
#include "DirectXTKInc/Mouse.h"

namespace Luxko {
	namespace Anuthur {
		constexpr auto SwapChainBufferCount = 2u;
		using Microsoft::WRL::ComPtr;

		// This class serves as a base class.
		// Classes that derives from it should contain all the resources
		// that would be updated in per-frame frequency.
		// The main application would then store multiple instances
		// of such class, apply them to the command list in circular 
		// sequence per frame in order to reduce synchronization needs
		// across CPU and GPU.
		struct ANUTHURRENDERER_API FrameResource {
		public:
			explicit FrameResource(ID3D12Device* pDevice);
			FrameResource(const FrameResource&) = delete;
			FrameResource(FrameResource&& fr) {
				_cmdAllocator = fr._cmdAllocator;
				_fenceCount = fr._fenceCount;
				fr._cmdAllocator.Reset();
			}
			FrameResource& operator=(const FrameResource&) = delete;
			FrameResource& operator=(FrameResource&& fr) {
				_cmdAllocator = nullptr;
				_fenceCount = fr._fenceCount;
				_cmdAllocator = fr._cmdAllocator;
				fr._cmdAllocator.Reset();
				return *this;
			}
			virtual ~FrameResource() {}

			ComPtr<ID3D12CommandAllocator>		_cmdAllocator;
			UINT64								_fenceCount = 0u;
		};


		class ANUTHURRENDERER_API D3D12App abstract : public Luxko::Application::BaseApp {
		public:

			// Ctors
			D3D12App(UINT width, UINT height, const std::wstring& name) :
				BaseApp(width, height, name) {}
			D3D12App(const D3D12App&) = delete;
			D3D12App& operator=(const D3D12App&) = delete;
			// Dtors
			virtual ~D3D12App();

		protected:
			// overridden methods
			virtual void OnInit() override;

			virtual void OnDestroy() override;

			virtual bool OnEvent(MSG msg) override;

			// This method is called when OnEvent receives a WM_SIZE message.
			virtual void OnResize();

			virtual void CreateMainDsvDescriptorHeaps();

			void LogFPSToTitle(); // Called every OnRender() to display fps on windows title.


			// Data members
			Anuthur::D3D12Manager				_d3d12Manager;
			Luxko::Timer						_mainTimer;
			BOOL								_appPaused = FALSE;
			UINT64								_wndResourceID;
			std::unique_ptr<DirectX::Keyboard>	_keyboard;
			std::unique_ptr<DirectX::Mouse>		_mouse;
		};
	}
}
