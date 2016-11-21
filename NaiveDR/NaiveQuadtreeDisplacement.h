//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
#pragma once
#include "D3D12App.h"
#include "D3D12HelperMethods.h"
#include "Matrix4x4f.h"
#include "Vector4f.h"
#include "Vector3f.h"
#include "Point3DH.h"
#include "PerspecCamera.h"
#include "Transform3DH.h"
#include "MeshResource.h"

namespace WTF {
	using namespace Luxko;
	using namespace Luxko::Anuthur;
	class NaiveBpqdmGenerator {
	public:

		struct VSI {
			Vector3f posND;
		};

	public:
		NaiveBpqdmGenerator() = default;
		NaiveBpqdmGenerator(const NaiveBpqdmGenerator&) = default;
		NaiveBpqdmGenerator(NaiveBpqdmGenerator&&) = default;
		NaiveBpqdmGenerator& operator=(const NaiveBpqdmGenerator&) = default;
		NaiveBpqdmGenerator& operator=(NaiveBpqdmGenerator&&) = default;
		~NaiveBpqdmGenerator() = default;

	public:
		void initialize(ID3D12Device* pDevice);

	public:
		inline D3D12_SHADER_BYTECODE getVS()const {
			return _vertexShader.Get();
		}
		inline D3D12_SHADER_BYTECODE getPS0()const {
			return _pixelShader0.Get();
		}
		inline D3D12_SHADER_BYTECODE getPS1()const {
			return _pixelShader1.Get();
		}

		inline ID3D12PipelineState* getPSO0()const {
			return _normalState0.Get();
		}

		inline ID3D12PipelineState* getPSO1()const {
			return _normalState1.Get();
		}

		inline ID3D12RootSignature* getRootSignature()const {
			return _rootSignature.Get();
		}

	public:
		static inline void recordRp0TexAttr(ID3D12GraphicsCommandList* cmdlist,
			UINT baseD) {
			cmdlist->SetGraphicsRoot32BitConstant(0u, baseD, 0u);
		}
		static inline void recordRp1BaseTex(
			ID3D12GraphicsCommandList* cmdlist,
			D3D12_GPU_DESCRIPTOR_HANDLE baseTexGpuHandle) {
			cmdlist->SetGraphicsRootDescriptorTable(
				static_cast<UINT>(1), baseTexGpuHandle);
		}

		inline void recordStateSettings0(ID3D12GraphicsCommandList* cmdlist) {
			cmdlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			cmdlist->SetPipelineState(_normalState0.Get());
			cmdlist->SetGraphicsRootSignature(_rootSignature.Get());
		}
		inline void recordStateSettings1(ID3D12GraphicsCommandList* cmdlist) {
			cmdlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			cmdlist->SetPipelineState(_normalState1.Get());
			cmdlist->SetGraphicsRootSignature(_rootSignature.Get());
		}


		static inline void recordResourcesTransitionFrom(
			ID3D12GraphicsCommandList* cmdlist,
			ID3D12Resource* texResource,
			ID3D12Resource* renderTargetResource,
			UINT texSubResource, UINT rtSubResource,
			D3D12_RESOURCE_STATES texFrom = D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATES renderTargetFrom
			= D3D12_RESOURCE_STATE_COMMON) {
			D3D12Helper::ResourceBarrier barriers[] = {
				D3D12Helper::ResourceBarrier::TransitionBarrier(
					texResource, texFrom, 
					D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
					D3D12_RESOURCE_BARRIER_FLAG_NONE, texSubResource),
				D3D12Helper::ResourceBarrier::TransitionBarrier(
					renderTargetResource, renderTargetFrom, 
					D3D12_RESOURCE_STATE_RENDER_TARGET,
					D3D12_RESOURCE_BARRIER_FLAG_NONE, rtSubResource),
			};
			cmdlist->ResourceBarrier(2u, barriers);
		}

		static inline void recordClearAndSetRtvDsv(
			ID3D12GraphicsCommandList* cmdlist,
			D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle,
			UINT numRects, const D3D12_RECT* pRects) {
			static FLOAT bbg[] = { 0.f, 0.f, 0.f, 0.f };
			//cmdlist->ClearRenderTargetView(rtvHandle, bbg, numRects, pRects);
			cmdlist->OMSetRenderTargets(static_cast<UINT>(1), &rtvHandle, TRUE, nullptr);
		}



	public:
		static D3D12_INPUT_LAYOUT_DESC getInputLayout();

		static const MeshGeometry& getInputBuffer(
			ID3D12Device* pDevice = nullptr,
			ID3D12GraphicsCommandList* pCmdlist = nullptr);

		std::pair<UINT, ComPtr<ID3D12Resource>> generateQDM(
			ID3D12Resource* srcNormalTexture,
			ID3D12Device* device,
			ID3D12GraphicsCommandList* cmdlist
		);
	private:
		static auto constexpr shaderPath = LR"(./qdgenerator.hlsl)";
		D3D12Helper::ShaderByteCode _vertexShader;
		D3D12Helper::ShaderByteCode _pixelShader0;
		D3D12Helper::ShaderByteCode _pixelShader1;
		ComPtr<ID3D12RootSignature> _rootSignature;
		ComPtr<ID3D12PipelineState> _normalState0;
		ComPtr<ID3D12PipelineState> _normalState1;

		ComPtr<ID3D12DescriptorHeap> _srvHeap;
		ComPtr<ID3D12DescriptorHeap> _rtvHeap;
	};

	class NBPQDM {
	public:
		struct MaterialAttr {
			Matrix4x4f mOtoW;
			Vector3f sAlbedo;
			float sPower;
			float heightScale;
			float heightOffset;
			UINT qdmMaxMipLvl;
		};

		struct VSI {
			Vector4f posO;
			Vector3f normal;
			Vector3f tangent;
			float u;
			float v;
		};

	public:
		NBPQDM() = default;
		NBPQDM(const NBPQDM&) = default;
		NBPQDM(NBPQDM&&) = default;
		NBPQDM& operator=(const NBPQDM&) = default;
		NBPQDM& operator=(NBPQDM&&) = default;
		~NBPQDM() = default;

	public:
		void initialize(ID3D12Device* pDevice,
			DXGI_FORMAT dsvFormat = DXGI_FORMAT_D24_UNORM_S8_UINT);

	public:
		inline D3D12_SHADER_BYTECODE getVS()const {
			return _vertexShader.Get();
		}
		inline D3D12_SHADER_BYTECODE getPS()const {
			return _pixelShader.Get();
		}
		inline ID3D12PipelineState* getPSO()const {
			return _normalState.Get();
		}

		inline ID3D12PipelineState* getPSOWireframe()const {
			return _wireframeState.Get();
		}
		inline ID3D12RootSignature* getRootSignature()const {
			return _rootSignature.Get();
		}

	public:
		static inline void recordRp0Camera(ID3D12GraphicsCommandList* cmdlist,
			D3D12_GPU_VIRTUAL_ADDRESS cameraGpuAddress) {
			cmdlist->SetGraphicsRootConstantBufferView(static_cast<UINT>(0), cameraGpuAddress);
		}
		static inline void recordRp1Material(ID3D12GraphicsCommandList* cmdlist,
			D3D12_GPU_VIRTUAL_ADDRESS materialGpuAddress) {
			cmdlist->SetGraphicsRootConstantBufferView(static_cast<UINT>(1), materialGpuAddress);
		}
		static inline void recordRp2Textures(ID3D12GraphicsCommandList* cmdlist,
			D3D12_GPU_DESCRIPTOR_HANDLE texsSrvGpuHandle) {
			cmdlist->SetGraphicsRootDescriptorTable(static_cast<UINT>(2), texsSrvGpuHandle);
		}

		inline void recordStateSettings(ID3D12GraphicsCommandList* cmdlist,
			D3D_PRIMITIVE_TOPOLOGY primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) {
			cmdlist->OMSetStencilRef(0x80);
			cmdlist->IASetPrimitiveTopology(primitiveTopology);
			cmdlist->SetPipelineState(_normalState.Get());
			cmdlist->SetGraphicsRootSignature(_rootSignature.Get());
		}
		inline void recordStateSettingsWireframe(
			ID3D12GraphicsCommandList* cmdlist,
			D3D_PRIMITIVE_TOPOLOGY primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) {
			cmdlist->OMSetStencilRef(0x80);
			cmdlist->IASetPrimitiveTopology(primitiveTopology);
			cmdlist->SetPipelineState(_normalState.Get());
			cmdlist->SetGraphicsRootSignature(_rootSignature.Get());
		}


		static inline void recordGBTransitionFrom(ID3D12GraphicsCommandList* cmdlist,
			ID3D12Resource** gBuffers, D3D12_RESOURCE_STATES from
			= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE) {
			D3D12Helper::ResourceBarrier barriers[] = {
				D3D12Helper::ResourceBarrier::TransitionBarrier(
					gBuffers[0], from, D3D12_RESOURCE_STATE_RENDER_TARGET),
				D3D12Helper::ResourceBarrier::TransitionBarrier(
					gBuffers[1], from, D3D12_RESOURCE_STATE_RENDER_TARGET),
				D3D12Helper::ResourceBarrier::TransitionBarrier(
					gBuffers[2], from, D3D12_RESOURCE_STATE_RENDER_TARGET),
			};
			cmdlist->ResourceBarrier(3u, barriers);
		}

		static inline void recordGBTransitionTo(ID3D12GraphicsCommandList* cmdlist,
			ID3D12Resource** gBuffers, D3D12_RESOURCE_STATES to
			= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE) {
			D3D12Helper::ResourceBarrier barriers[] = {
				D3D12Helper::ResourceBarrier::TransitionBarrier(
					gBuffers[0], D3D12_RESOURCE_STATE_RENDER_TARGET, to),
				D3D12Helper::ResourceBarrier::TransitionBarrier(
					gBuffers[1], D3D12_RESOURCE_STATE_RENDER_TARGET, to),
				D3D12Helper::ResourceBarrier::TransitionBarrier(
					gBuffers[2], D3D12_RESOURCE_STATE_RENDER_TARGET, to),
			};
			cmdlist->ResourceBarrier(3u, barriers);
		}

		static inline void recordClearAndSetRtvDsv(
			ID3D12GraphicsCommandList* cmdlist,
			D3D12_CPU_DESCRIPTOR_HANDLE* rtvHandles,
			D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle,
			UINT numRects, const D3D12_RECT* pRects) {
			cmdlist->ClearDepthStencilView(dsvHandle,
				D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f,
				static_cast<UINT8>(0), numRects, pRects);
			static FLOAT bbg[] = { 0.f, 0.f, 0.f, 0.f };
			cmdlist->ClearRenderTargetView(rtvHandles[0], bbg, numRects, pRects);
			cmdlist->ClearRenderTargetView(rtvHandles[1], bbg, numRects, pRects);
			cmdlist->ClearRenderTargetView(rtvHandles[2], bbg, numRects, pRects);
			cmdlist->OMSetRenderTargets(static_cast<UINT>(3), rtvHandles, TRUE, &dsvHandle);

		}



	public:
		static D3D12_INPUT_LAYOUT_DESC getInputLayout();

	private:
		static auto constexpr shaderPath = LR"(./dsquadtree.hlsl)";
		D3D12Helper::ShaderByteCode _vertexShader;
		D3D12Helper::ShaderByteCode _pixelShader;
		ComPtr<ID3D12RootSignature> _rootSignature;
		ComPtr<ID3D12PipelineState> _normalState;
		ComPtr<ID3D12PipelineState> _wireframeState;


	};
}