//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
#pragma once
#include "RendererPCH.h"
#include "Matrix4x4f.h"
#include "Vector4f.h"
#include "Vector3f.h"
#include "D3D12HelperMethods.h"
#include "MeshResource.h"

namespace Luxko {
	namespace Anuthur {
		namespace DRP {
			using Microsoft::WRL::ComPtr;
			inline namespace Common {
				struct CameraAttr {
					Matrix4x4f mWtoV;
					Matrix4x4f mVtoW;
					Matrix4x4f mWtoH;
					Vector3f posW;
					float farClipD;
				};
			}
			namespace GBPass {
				class NaiveBinnPhong {
				public:
					struct MaterialAttr {
						Matrix4x4f mOtoW;
						Vector3f sAlbedo;
						float sPower;
					};

					struct VSI {
						Vector4f posO;
						Vector3f normal;
						Vector3f tangent;
						float u;
						float v;
					};

				public:
					NaiveBinnPhong() = default;
					NaiveBinnPhong(const NaiveBinnPhong&) = default;
					NaiveBinnPhong(NaiveBinnPhong&&) = default;
					NaiveBinnPhong& operator=(const NaiveBinnPhong&) = default;
					NaiveBinnPhong& operator=(NaiveBinnPhong&&) = default;
					~NaiveBinnPhong() = default;

				public:
					void initialize(ID3D12Device* pDevice, 
						DXGI_FORMAT dsvFormat = DXGI_FORMAT_D24_UNORM_S8_UINT);
					
				public:
					inline D3D12_SHADER_BYTECODE getVS()const;
					inline D3D12_SHADER_BYTECODE getPS()const;
					inline ID3D12PipelineState* getPSO()const;
					inline ID3D12PipelineState* getPSOWireframe()const;
					inline ID3D12RootSignature* getRootSignature()const;

				public:
					static inline void recordRp0Camera(ID3D12GraphicsCommandList* cmdlist,
						D3D12_GPU_VIRTUAL_ADDRESS cameraGpuAddress);
					static inline void recordRp1Material(ID3D12GraphicsCommandList* cmdlist,
						D3D12_GPU_VIRTUAL_ADDRESS materialGpuAddress);
					static inline void recordRp2Textures(ID3D12GraphicsCommandList* cmdlist,
						D3D12_GPU_DESCRIPTOR_HANDLE texsSrvGpuHandle);

					inline void recordStateSettings(ID3D12GraphicsCommandList* cmdlist,
						UINT stencilRef = 1,
						D3D_PRIMITIVE_TOPOLOGY primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
					inline void recordStateSettingsWireframe(
						ID3D12GraphicsCommandList* cmdlist,
						D3D_PRIMITIVE_TOPOLOGY primitiveTopology);

					static inline void recordGBTransitionFrom(ID3D12GraphicsCommandList* cmdlist,
						ID3D12Resource* gBuffers, D3D12_RESOURCE_STATES from
						= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

					static inline void recordGBTransitionTo(ID3D12GraphicsCommandList* cmdlist,
						ID3D12Resource* gBuffers, D3D12_RESOURCE_STATES to
						= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

					static inline void recordClearAndSetRtvDsv(
						ID3D12GraphicsCommandList* cmdlist,
						D3D12_CPU_DESCRIPTOR_HANDLE* rtvHandles,
						D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle,
						UINT numRects, D3D12_RECT* pRects);



				public:
					static D3D12_INPUT_LAYOUT_DESC getInputLayout();

				private:
					static auto constexpr vsPath = LR"(../Src/HLSLShaders/DRP/GBPass/NaiveBlinnPhong.hlsl)";
					static auto constexpr psPath = LR"(../Src/HLSLShaders/DRP/GBPass/NaiveBlinnPhong.hlsl)";
					D3D12Helper::ShaderByteCode _vertexShader;
					D3D12Helper::ShaderByteCode _pixelShader;
					ComPtr<ID3D12RootSignature> _rootSignature;
					ComPtr<ID3D12PipelineState> _normalState;
					ComPtr<ID3D12PipelineState> _wireframeState;

				};
				
			}

			namespace LightPass {
				namespace NaiveLights {
					struct LightParams {
						Matrix4x4f mOtoW;
						Vector4f posW;
						Vector4f color;
						Vector4f direction;
						Vector4f range;
						float angleX; // upper bound of cos(\theta)
						float angleY; // lower bound of cos(\theta)
					};

					struct VSI {
						Vector3f pos;
					};

					inline void recordRp1CameraAndGBuffer(ID3D12GraphicsCommandList* cmdlist,
						D3D12_GPU_DESCRIPTOR_HANDLE cameraGpuHandleAddress);
					inline void recordRp0Light(ID3D12GraphicsCommandList* cmdlist,
						D3D12_GPU_VIRTUAL_ADDRESS lightCBGpuAddress);

					inline ID3D12RootSignature* getRootSignature(
						ID3D12Device* creationDevice = nullptr /* Used first time*/);

					inline void recordSettings(ID3D12GraphicsCommandList* cmdlist,
						ID3D12Device* creationDevice = nullptr /* Used first time*/);

					inline void recordGBTransitionFrom(ID3D12GraphicsCommandList* cmdlist,
						ID3D12Resource* gBuffers, D3D12_RESOURCE_STATES from
						= D3D12_RESOURCE_STATE_RENDER_TARGET);

					inline void recordGBTransitionTo(ID3D12GraphicsCommandList* cmdlist,
						ID3D12Resource* gBuffers, D3D12_RESOURCE_STATES to
						= D3D12_RESOURCE_STATE_RENDER_TARGET);

					inline void recordClearAndSetRtvDsv(
						ID3D12GraphicsCommandList* cmdlist,
						D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle,
						D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle,
						UINT numRects, D3D12_RECT* pRects);

					D3D12_INPUT_LAYOUT_DESC getInputLayout();

					class PointLight {
					public:
						PointLight() = default;
						PointLight(const PointLight&) = default;
						PointLight(PointLight&&) = default;
						PointLight& operator=(const PointLight&) = default;
						PointLight& operator=(PointLight&&) = default;
						~PointLight() = default;

					public:
						void initialize(ID3D12Device* pDevice,
							UINT stencilReadMask = 0x3u, UINT stencilWriteMask = 0x2u,
							DXGI_FORMAT rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM,
							DXGI_FORMAT dsvFormat = DXGI_FORMAT_D24_UNORM_S8_UINT);

					public:
						inline D3D12_SHADER_BYTECODE getVS()const;
						inline D3D12_SHADER_BYTECODE getPS()const;
						inline ID3D12PipelineState* getPSO()const;

						static const MeshGeometry& getInputBuffer(
							ID3D12Device* pDevice = nullptr,
							ID3D12GraphicsCommandList* pCmdlist = nullptr);

						inline void recordSettings(ID3D12GraphicsCommandList* cmdList);

						static void generateOtoWMatrix(LightParams& params);

					private:
						static auto constexpr vsPath = LR"(../Src/HLSLShaders/DRP/LightPass/NaiveLights.hlsl)";
						static auto constexpr psPath = LR"(../Src/HLSLShaders/DRP/LightPass/NaiveLights.hlsl)";
						
						
						D3D12Helper::ShaderByteCode _vertexShader;
						D3D12Helper::ShaderByteCode _pixelShader;
						ComPtr<ID3D12PipelineState> _normalState;
						UINT _stencilRef;
					};

					class SpotLight {
					public:
						SpotLight() = default;
						SpotLight(const SpotLight&) = default;
						SpotLight(SpotLight&&) = default;
						SpotLight& operator=(const SpotLight&) = default;
						SpotLight& operator=(SpotLight&&) = default;
						~SpotLight() = default;

					public:
						void initialize(ID3D12Device* pDevice,
							UINT stencilReadMask = 0x3u, UINT stencilWriteMask = 0x2u,
							DXGI_FORMAT rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM,
							DXGI_FORMAT dsvFormat = DXGI_FORMAT_D24_UNORM_S8_UINT);

					public:
						inline D3D12_SHADER_BYTECODE getVS()const;
						inline D3D12_SHADER_BYTECODE getPS()const;
						inline ID3D12PipelineState* getPSO()const;

						static const MeshGeometry& getInputBuffer(
							ID3D12Device* pDevice = nullptr,
							ID3D12GraphicsCommandList* pCmdlist = nullptr);

						inline void recordSettings(ID3D12GraphicsCommandList* cmdList);

						static void generateOtoWMatrix(LightParams& params);

					private:
						static auto constexpr vsPath = LR"(../Src/HLSLShaders/DRP/LightPass/NaiveLights.hlsl)";
						static auto constexpr psPath = LR"(../Src/HLSLShaders/DRP/LightPass/NaiveLights.hlsl)";


						D3D12Helper::ShaderByteCode _vertexShader;
						D3D12Helper::ShaderByteCode _pixelShader;
						ComPtr<ID3D12PipelineState> _normalState;
						UINT _stencilRef;
					};

					class DirectionalLight {
					public:
						DirectionalLight() = default;
						DirectionalLight(const DirectionalLight&) = default;
						DirectionalLight(DirectionalLight&&) = default;
						DirectionalLight& operator=(const DirectionalLight&) = default;
						DirectionalLight& operator=(DirectionalLight&&) = default;
						~DirectionalLight() = default;

					public:
						void initialize(ID3D12Device* pDevice,
							UINT stencilReadMask = 0x3u, UINT stencilWriteMask = 0x2u,
							DXGI_FORMAT rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM,
							DXGI_FORMAT dsvFormat = DXGI_FORMAT_D24_UNORM_S8_UINT);

					public:
						inline D3D12_SHADER_BYTECODE getVS()const;
						inline D3D12_SHADER_BYTECODE getPS()const;
						inline ID3D12PipelineState* getPSO()const;

						static const MeshGeometry& getInputBuffer(
							ID3D12Device* pDevice = nullptr,
							ID3D12GraphicsCommandList* pCmdlist = nullptr);

						inline void recordSettings(ID3D12GraphicsCommandList* cmdList);

						static void generateOtoWMatrix(LightParams& params);

					private:
						static auto constexpr vsPath = LR"(../Src/HLSLShaders/DRP/LightPass/NaiveLights.hlsl)";
						static auto constexpr psPath = LR"(../Src/HLSLShaders/DRP/LightPass/NaiveLights.hlsl)";


						D3D12Helper::ShaderByteCode _vertexShader;
						D3D12Helper::ShaderByteCode _pixelShader;
						ComPtr<ID3D12PipelineState> _normalState;
						UINT _stencilRef;
					};
				}
			}
		}
	}
}