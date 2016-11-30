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
				class NaiveBlinnPhong {
				public:
					struct MaterialAttr {
						Matrix4x4f mOtoW;
						Vector3f sAlbedo;
						float sPower;
						float heightScale;
						float heightOffset;
						float minSampleCount;
						float maxSampleCount;
					};

					struct VSI {
						Vector4f posO;
						Vector3f normal;
						Vector3f tangent;
						float u;
						float v;
					};

				public:
					NaiveBlinnPhong() = default;
					NaiveBlinnPhong(const NaiveBlinnPhong&) = default;
					NaiveBlinnPhong(NaiveBlinnPhong&&) = default;
					NaiveBlinnPhong& operator=(const NaiveBlinnPhong&) = default;
					NaiveBlinnPhong& operator=(NaiveBlinnPhong&&) = default;
					~NaiveBlinnPhong() = default;

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

					inline void recordStateSettingsPOM(ID3D12GraphicsCommandList* cmdlist,
						D3D_PRIMITIVE_TOPOLOGY primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) {
						cmdlist->OMSetStencilRef(0x80);
						cmdlist->IASetPrimitiveTopology(primitiveTopology);
						cmdlist->SetPipelineState(_normalStatePOM.Get());
						cmdlist->SetGraphicsRootSignature(_rootSignature.Get());
					}

					inline void recordStateSettingsISPM(ID3D12GraphicsCommandList* cmdlist,
						D3D_PRIMITIVE_TOPOLOGY primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) {
						cmdlist->OMSetStencilRef(0x80);
						cmdlist->IASetPrimitiveTopology(primitiveTopology);
						cmdlist->SetPipelineState(_normalStateISPM.Get());
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
					static auto constexpr vsPath = LR"(../Src/HLSLShaders/DRP/GBPass/NaiveBlinnPhong.hlsl)";
					static auto constexpr psPath = LR"(../Src/HLSLShaders/DRP/GBPass/NaiveBlinnPhong.hlsl)";
					D3D12Helper::ShaderByteCode _vertexShader;
					D3D12Helper::ShaderByteCode _pixelShader;
					D3D12Helper::ShaderByteCode _pixelShaderISPM;
					D3D12Helper::ShaderByteCode _pixelShaderPOM;
					ComPtr<ID3D12RootSignature> _rootSignature;
					ComPtr<ID3D12PipelineState> _normalState;
					ComPtr<ID3D12PipelineState> _normalStateISPM;
					ComPtr<ID3D12PipelineState> _normalStatePOM;
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
						D3D12_GPU_DESCRIPTOR_HANDLE cameraGpuHandleAddress) {
						cmdlist->SetGraphicsRootDescriptorTable(
							static_cast<UINT>(1), cameraGpuHandleAddress);
					}

					inline void recordRp0Light(
						ID3D12GraphicsCommandList* cmdlist,
						D3D12_GPU_VIRTUAL_ADDRESS lightCBGpuAddress,
						UINT& stencilRef) {
						++stencilRef;
						assert(stencilRef < 0xff);
						cmdlist->OMSetStencilRef(stencilRef);
						cmdlist->SetGraphicsRootConstantBufferView(static_cast<UINT>(0), lightCBGpuAddress);
					}


					ID3D12RootSignature* getRootSignature(
						ID3D12Device* creationDevice = nullptr /* Used first time*/);

					inline void recordSettings(ID3D12GraphicsCommandList* cmdlist,
						ID3D12Device* creationDevice = nullptr /* Used first time*/) {
						cmdlist->SetGraphicsRootSignature(getRootSignature(creationDevice));
						cmdlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
					}


					inline void resetLightingRecords(ID3D12GraphicsCommandList* cmdlist,
						UINT& stencilRef, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle,
						UINT numRects, D3D12_RECT* pRects) {
						stencilRef = 0x80u;
						cmdlist->ClearDepthStencilView(dsvHandle,
							D3D12_CLEAR_FLAG_STENCIL, 0.f, static_cast<UINT8>(stencilRef),
							numRects, pRects);
					}

					inline void recordGBTransitionFrom(ID3D12GraphicsCommandList* cmdlist,
						ID3D12Resource** gBuffers, D3D12_RESOURCE_STATES from
						= D3D12_RESOURCE_STATE_RENDER_TARGET) {
						D3D12Helper::ResourceBarrier barriers[] = {
							D3D12Helper::ResourceBarrier::TransitionBarrier(
								gBuffers[0], from, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),
							D3D12Helper::ResourceBarrier::TransitionBarrier(
								gBuffers[1], from, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),
							D3D12Helper::ResourceBarrier::TransitionBarrier(
								gBuffers[2], from, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),
						};
						cmdlist->ResourceBarrier(3u, barriers);
					}

					inline void recordGBTransitionTo(ID3D12GraphicsCommandList* cmdlist,
						ID3D12Resource** gBuffers, D3D12_RESOURCE_STATES to
						= D3D12_RESOURCE_STATE_RENDER_TARGET) {
						D3D12Helper::ResourceBarrier barriers[] = {
							D3D12Helper::ResourceBarrier::TransitionBarrier(
								gBuffers[0], D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, to),
							D3D12Helper::ResourceBarrier::TransitionBarrier(
								gBuffers[1], D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, to),
							D3D12Helper::ResourceBarrier::TransitionBarrier(
								gBuffers[2], D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, to),
						};
						cmdlist->ResourceBarrier(3u, barriers);
					}

					inline void recordClearAndSetRtvDsv(
						ID3D12GraphicsCommandList* cmdlist,
						D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle,
						D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle,
						UINT numRects, D3D12_RECT* pRects) {
						static FLOAT bbg[] = { 0.f, 0.f, 0.f, 0.f };
						cmdlist->ClearRenderTargetView(rtvHandle, bbg, numRects, pRects);
						cmdlist->OMSetRenderTargets(static_cast<UINT>(1), &rtvHandle, TRUE, &dsvHandle);
					}

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
							DXGI_FORMAT rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM,
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

						static const MeshGeometry& getInputBuffer(
							ID3D12Device* pDevice = nullptr,
							ID3D12GraphicsCommandList* pCmdlist = nullptr);

						inline void recordSettings(
							ID3D12GraphicsCommandList* cmdList) {
							cmdList->SetPipelineState(_normalState.Get());
							auto& mesh = getInputBuffer();
							cmdList->IASetVertexBuffers(0u, 1u, &mesh.VertexBufferView());
							cmdList->IASetIndexBuffer(&mesh.IndexBufferView());
						}

						static inline void recordDraw(
							ID3D12GraphicsCommandList* pCmdlist) {
							pCmdlist->DrawIndexedInstanced(
								getInputBuffer().GetTotoalIndexCount(),
								1u, 0u, 0u, 0u);
						}

						static void generateOtoWMatrix(LightParams& params);

					private:
						static auto constexpr vsPath = LR"(../Src/HLSLShaders/DRP/LightPass/NaiveLights.hlsl)";
						static auto constexpr psPath = LR"(../Src/HLSLShaders/DRP/LightPass/NaiveLights.hlsl)";
						
						
						D3D12Helper::ShaderByteCode _vertexShader;
						D3D12Helper::ShaderByteCode _pixelShader;
						ComPtr<ID3D12PipelineState> _normalState;
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
							DXGI_FORMAT rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM,
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

						static const MeshGeometry& getInputBuffer(
							ID3D12Device* pDevice = nullptr,
							ID3D12GraphicsCommandList* pCmdlist = nullptr);

						inline void recordSettings(
							ID3D12GraphicsCommandList* cmdList) {
							cmdList->SetPipelineState(_normalState.Get());
							auto& mesh = getInputBuffer();
							cmdList->IASetVertexBuffers(0u, 1u, &mesh.VertexBufferView());
							cmdList->IASetIndexBuffer(&mesh.IndexBufferView());
						}

						static inline void recordDraw(
							ID3D12GraphicsCommandList* pCmdlist) {
							pCmdlist->DrawIndexedInstanced(
								getInputBuffer().GetTotoalIndexCount(),
								1u, 0u, 0u, 0u);
						}

						static void generateOtoWMatrix(LightParams& params);

					private:
						static auto constexpr vsPath = LR"(../Src/HLSLShaders/DRP/LightPass/NaiveLights.hlsl)";
						static auto constexpr psPath = LR"(../Src/HLSLShaders/DRP/LightPass/NaiveLights.hlsl)";


						D3D12Helper::ShaderByteCode _vertexShader;
						D3D12Helper::ShaderByteCode _pixelShader;
						ComPtr<ID3D12PipelineState> _normalState;
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
							DXGI_FORMAT rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM,
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

						static const MeshGeometry& getInputBuffer(
							ID3D12Device* pDevice = nullptr,
							ID3D12GraphicsCommandList* pCmdlist = nullptr);

						inline void recordSettings(
							ID3D12GraphicsCommandList* cmdList) {
							cmdList->SetPipelineState(_normalState.Get());
							auto& mesh = getInputBuffer();
							cmdList->IASetVertexBuffers(0u, 1u, &mesh.VertexBufferView());
							cmdList->IASetIndexBuffer(&mesh.IndexBufferView());
						}

						static inline void recordDraw(
							ID3D12GraphicsCommandList* pCmdlist) {
							pCmdlist->DrawIndexedInstanced(
								getInputBuffer().GetTotoalIndexCount(),
								1u, 0u, 0u, 0u);
						}

						static void generateOtoWMatrix(LightParams& params);

					private:
						static auto constexpr vsPath = LR"(../Src/HLSLShaders/DRP/LightPass/NaiveLights.hlsl)";
						static auto constexpr psPath = LR"(../Src/HLSLShaders/DRP/LightPass/NaiveLights.hlsl)";


						D3D12Helper::ShaderByteCode _vertexShader;
						D3D12Helper::ShaderByteCode _pixelShader;
						ComPtr<ID3D12PipelineState> _normalState;
					};
				}
			}
		}
	}
}