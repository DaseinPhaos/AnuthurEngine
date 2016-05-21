//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
#pragma once

#include "RendererPCH.h"

namespace Luxko {
	namespace Anuthur {

		inline void ThrowIfFailed(HRESULT hr, const wchar_t* wstr = nullptr) {
			if (FAILED(hr)) {
				if (wstr) throw std::wstring(wstr)+L"\nError code: "+std::to_wstring(hr);
				else throw L"Operation failed!\nError code " + std::to_wstring(hr);
			}
		}

		namespace D3D12Helper {

			constexpr auto MinHardwareAllocSize = static_cast<UINT>(256);
			constexpr auto MaxInputSlotCount = static_cast<size_t>(16);

			class ANUTHURRENDERER_API InputElementDescriptor
				: public D3D12_INPUT_ELEMENT_DESC {
			public:
				InputElementDescriptor(LPCSTR semanticName, DXGI_FORMAT format,
					UINT bytedOffsetAligned, UINT inputSlot = 0u, UINT semanticIndex = 0u,
					UINT instanceDataSR = 0u, D3D12_INPUT_CLASSIFICATION classification
					= D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA);
			};

			class ANUTHURRENDERER_API InputLayoutDescriptor {
			public:
				InputLayoutDescriptor() {}

				void PushElementDescription(LPCSTR semanticName,
					DXGI_FORMAT format, UINT inputSlot = 0u, UINT semanticIndex = 0u,
					UINT instanceDataSR = 0u, D3D12_INPUT_CLASSIFICATION classification
					= D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA);

				D3D12_INPUT_LAYOUT_DESC Get()const;
			private:
				std::vector<InputElementDescriptor> _ilds;
				UINT _currentSlotOffset[MaxInputSlotCount] = { 0u };

			};

			class ANUTHURRENDERER_API ResourceDescriptor :public D3D12_RESOURCE_DESC {
			public:
				ResourceDescriptor(D3D12_RESOURCE_DIMENSION dimension,UINT64 width, 
					UINT height = 1, UINT16 depth = 1, 
					D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE,
					UINT64 alignment = 0, UINT16 mipLevels = 1, UINT sampleCount = 1,
					UINT sampleQuality = 0, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN, 
					D3D12_TEXTURE_LAYOUT layout = D3D12_TEXTURE_LAYOUT_UNKNOWN);

				static inline ResourceDescriptor Buffer(UINT64 sizeBytes,
					D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE,
					UINT64 alignment = 0);

				static inline ResourceDescriptor Tex1D(UINT64 width, DXGI_FORMAT format,
					UINT arraySize = 1, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE,
					UINT64 alignment = 0, UINT16 mipLevels = 1,
					D3D12_TEXTURE_LAYOUT layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR);

				static inline ResourceDescriptor Tex2D(UINT64 width, UINT height,
					DXGI_FORMAT format, UINT16 arraySize = 1,
					D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE,
					UINT64 alignment = 0, UINT16 mipLevels = 1,
					D3D12_TEXTURE_LAYOUT layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
					UINT sampleCount = 1, UINT sampleQuality = 0);
				static inline ResourceDescriptor Tex3D(UINT64 width, UINT height,
					UINT16 depth, DXGI_FORMAT format,
					D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE,
					UINT64 alignment = 0, UINT16 mipLevels = 1,
					D3D12_TEXTURE_LAYOUT layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR);

			};

			class ANUTHURRENDERER_API ResourceBarrier : public D3D12_RESOURCE_BARRIER {
			public:
				ResourceBarrier() {}
				static ResourceBarrier TransitionBarrier(ID3D12Resource* resource,
					D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter,
					D3D12_RESOURCE_BARRIER_FLAGS flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
					UINT subResource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

				static ResourceBarrier UAVBarrier(ID3D12Resource* resource, 
					D3D12_RESOURCE_BARRIER_FLAGS flags = D3D12_RESOURCE_BARRIER_FLAG_NONE);

				static ResourceBarrier AliasingBarrier(ID3D12Resource* resourceBefore = nullptr,
					ID3D12Resource* resourceAfter = nullptr,
					D3D12_RESOURCE_BARRIER_FLAGS flags = D3D12_RESOURCE_BARRIER_FLAG_NONE);
			};

			class ANUTHURRENDERER_API HeapProperties : public D3D12_HEAP_PROPERTIES {
			public:
				HeapProperties(D3D12_HEAP_TYPE type = D3D12_HEAP_TYPE_DEFAULT,
					D3D12_CPU_PAGE_PROPERTY cpuPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
					D3D12_MEMORY_POOL memoryPoolPref = D3D12_MEMORY_POOL_UNKNOWN,
					UINT CreationNodeMask = 0, UINT visibleNodeMask = 0);
			};

			class ANUTHURRENDERER_API DescriptorHeapDescriptor : public D3D12_DESCRIPTOR_HEAP_DESC {
			public:
				DescriptorHeapDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT count = 1u,
					D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
					 UINT nodeMask = 0u);
			};

			class ANUTHURRENDERER_API VertexBuffersDescriptor {
			public:
				VertexBuffersDescriptor() {}
				void Pop();
				void Push(D3D12_GPU_VIRTUAL_ADDRESS bufferLocation, UINT sizeInBytes, UINT strideInBytes);
				void Apply(ID3D12GraphicsCommandList* cmdlst, UINT startSlot)const;
			private:
				std::vector<D3D12_VERTEX_BUFFER_VIEW> _vbvs;
			};

			class ANUTHURRENDERER_API IndexBufferDescriptor : public D3D12_INDEX_BUFFER_VIEW {
			public:
				IndexBufferDescriptor(D3D12_GPU_VIRTUAL_ADDRESS bufferLocation,
					UINT sizeInBytes, DXGI_FORMAT format = DXGI_FORMAT_R16_UINT) :
					D3D12_INDEX_BUFFER_VIEW{ bufferLocation, sizeInBytes, format } {}

				void Apply(ID3D12GraphicsCommandList* cmdlst)const;
			};

			class ANUTHURRENDERER_API StaticSamplerDescriptor : public D3D12_STATIC_SAMPLER_DESC {
			public:
				StaticSamplerDescriptor() {}

				StaticSamplerDescriptor(
					UINT shaderRegister,
					D3D12_FILTER filter = D3D12_FILTER_ANISOTROPIC,
					D3D12_TEXTURE_ADDRESS_MODE addressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
					D3D12_TEXTURE_ADDRESS_MODE addressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
					D3D12_TEXTURE_ADDRESS_MODE addressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
					FLOAT mipLODBias = 0,
					UINT maxAnisotropy = 16,
					D3D12_COMPARISON_FUNC comparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL,
					D3D12_STATIC_BORDER_COLOR borderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
					FLOAT minLOD = 0.f,
					FLOAT maxLOD = D3D12_FLOAT32_MAX,
					D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL,
					UINT registerSpace = 0)
				{
					Init(
						shaderRegister,
						filter,
						addressU,
						addressV,
						addressW,
						mipLODBias,
						maxAnisotropy,
						comparisonFunc,
						borderColor,
						minLOD,
						maxLOD,
						shaderVisibility,
						registerSpace);
				}

				void Init(
					UINT shaderRegister,
					D3D12_FILTER filter = D3D12_FILTER_ANISOTROPIC,
					D3D12_TEXTURE_ADDRESS_MODE addressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
					D3D12_TEXTURE_ADDRESS_MODE addressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
					D3D12_TEXTURE_ADDRESS_MODE addressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
					FLOAT mipLODBias = 0,
					UINT maxAnisotropy = 16,
					D3D12_COMPARISON_FUNC comparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL,
					D3D12_STATIC_BORDER_COLOR borderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
					FLOAT minLOD = 0.f,
					FLOAT maxLOD = D3D12_FLOAT32_MAX,
					D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL,
					UINT registerSpace = 0);



			};
			
			class ANUTHURRENDERER_API RootDescriptorRange : public D3D12_DESCRIPTOR_RANGE {
			public:
				RootDescriptorRange() {}
				RootDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE type,
					UINT numDescriptors = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND,
					UINT baseShaderRegister = 0u,
					UINT registerSpace = 0u,
					UINT offsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
				void Init(D3D12_DESCRIPTOR_RANGE_TYPE type,
					UINT numDescriptors = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND,
					UINT baseShaderRegister = 0u,
					UINT registerSpace = 0u,
					UINT offsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
			};
			
			class ANUTHURRENDERER_API RootDescriptorTable {
			public:
				RootDescriptorTable() {}
				RootDescriptorTable(const RootDescriptorTable&) = delete;
				RootDescriptorTable& operator=(const RootDescriptorTable&) = delete;
				~RootDescriptorTable() {}

				void Pop();
				void Push(const RootDescriptorRange& range);
				void Emplace(D3D12_DESCRIPTOR_RANGE_TYPE type,
					UINT numDescriptors = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND,
					UINT baseShaderRegister = 0u,
					UINT registerSpace = 0u,
					UINT offsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);

				D3D12_ROOT_DESCRIPTOR_TABLE Get()const;

			private:
				std::vector<RootDescriptorRange> _dRanges;
			};

			class ANUTHURRENDERER_API RootParameter : public D3D12_ROOT_PARAMETER {
			public:
				RootParameter() {}

				static RootParameter Constant(UINT shaderRegister, UINT num32BitValues,
					UINT registerSpace = 0u, D3D12_SHADER_VISIBILITY visb
					= D3D12_SHADER_VISIBILITY_ALL);
				static RootParameter DescriptorTablef(D3D12_ROOT_DESCRIPTOR_TABLE table,
					D3D12_SHADER_VISIBILITY visb = D3D12_SHADER_VISIBILITY_ALL);
				static RootParameter CBV(UINT shaderRegister, UINT registerSpace = 0u,
					D3D12_SHADER_VISIBILITY visb = D3D12_SHADER_VISIBILITY_ALL);
				static RootParameter SRV(UINT shaderRegister, UINT registerSpace = 0u,
					D3D12_SHADER_VISIBILITY visb = D3D12_SHADER_VISIBILITY_ALL);
				static RootParameter UAV(UINT shaderRegister, UINT registerSpace = 0u,
					D3D12_SHADER_VISIBILITY visb = D3D12_SHADER_VISIBILITY_ALL);
			};

			class ANUTHURRENDERER_API RootSignatureDescriptor {
			public:
				RootSignatureDescriptor() {}
				RootSignatureDescriptor(const RootSignatureDescriptor&) = delete;
				RootSignatureDescriptor& operator=(const RootSignatureDescriptor&) = delete;
				~RootSignatureDescriptor() {}

				void PopRootParameter();
				void PushRootParameter(const RootParameter& rp);
				void PushRPConstant(UINT shaderRegister, UINT num32BitValues,
					UINT registerSpace = 0u, D3D12_SHADER_VISIBILITY visb
					= D3D12_SHADER_VISIBILITY_ALL);
				void PushRPDescriptorTable(D3D12_ROOT_DESCRIPTOR_TABLE table,
					D3D12_SHADER_VISIBILITY visb = D3D12_SHADER_VISIBILITY_ALL);
				void PushRPCBV(UINT shaderRegister, UINT registerSpace = 0u,
					D3D12_SHADER_VISIBILITY visb = D3D12_SHADER_VISIBILITY_ALL);
				void PushRPSRV(UINT shaderRegister, UINT registerSpace = 0u,
					D3D12_SHADER_VISIBILITY visb = D3D12_SHADER_VISIBILITY_ALL);
				void PushRPUAV(UINT shaderRegister, UINT registerSpace = 0u,
					D3D12_SHADER_VISIBILITY visb = D3D12_SHADER_VISIBILITY_ALL);
				void PopStaticSamplerDescriptor();
				void PushStaticSamplerDescriptor(const StaticSamplerDescriptor& ssd);
				void PushStaticSamplerDescriptor(UINT shaderRegister,
					D3D12_FILTER filter = D3D12_FILTER_ANISOTROPIC,
					D3D12_TEXTURE_ADDRESS_MODE addressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
					D3D12_TEXTURE_ADDRESS_MODE addressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
					D3D12_TEXTURE_ADDRESS_MODE addressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
					FLOAT mipLODBias = 0,
					UINT maxAnisotropy = 16,
					D3D12_COMPARISON_FUNC comparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL,
					D3D12_STATIC_BORDER_COLOR borderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
					FLOAT minLOD = 0.f,
					FLOAT maxLOD = D3D12_FLOAT32_MAX,
					D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL,
					UINT registerSpace = 0);
				D3D12_ROOT_SIGNATURE_DESC Get(D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE)const;
				
				HRESULT SerializeRootSignature(ID3DBlob** ppSerializedRootSignature, 
					 D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE,
					ID3DBlob** ppErrorBlob = nullptr)const;
			private:
				std::vector<RootParameter> _rootParamters;
				std::vector<StaticSamplerDescriptor> _staticSamplerDescs;
			};

			class ANUTHURRENDERER_API CBVDescriptor : public D3D12_CONSTANT_BUFFER_VIEW_DESC {
			public:
				CBVDescriptor(D3D12_GPU_VIRTUAL_ADDRESS location, UINT sizeInBytes);
				CBVDescriptor(ID3D12Resource* buffer, UINT64 offsetFromBufferStart, UINT sizeInBytes);

			};

			class ANUTHURRENDERER_API RasterizerDescriptor : public D3D12_RASTERIZER_DESC {
			public:
				RasterizerDescriptor(D3D12_FILL_MODE fillMode = D3D12_FILL_MODE_SOLID,
					D3D12_CULL_MODE cullMode = D3D12_CULL_MODE_BACK,
					BOOL frontCounterClockwise = FALSE, INT depthBias = 0,
					FLOAT depthBiaClamp = 0.f, FLOAT slopeScaledDepthBias = 0.f,
					BOOL depthClipEnable = TRUE,BOOL msEnable = FALSE, 
					BOOL antiAliasEnable = FALSE, UINT forcedSampleCount = 0u,
					D3D12_CONSERVATIVE_RASTERIZATION_MODE crMode = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF);
			};

			ANUTHURRENDERER_API Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
				ID3D12Device* device, ID3D12GraphicsCommandList* cmdlst,
				const void* data, UINT64 sizeInBytes,
				Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);

			ANUTHURRENDERER_API UINT GetCBSizeAligned(size_t sizeOfObject);

			ANUTHURRENDERER_API HRESULT CompileShader(LPCWSTR filename,
				LPCSTR entryPoint, LPCSTR target,
				ID3DBlob** ppCompiled, ID3DBlob** ppErrorMsg = nullptr,
				const D3D_SHADER_MACRO* pDefines = nullptr,
				ID3DInclude* include = D3D_COMPILE_STANDARD_FILE_INCLUDE);

			class ANUTHURRENDERER_API ShaderByteCode {
			public:
				ShaderByteCode() {}
				HRESULT CompileFromFile(LPCWSTR filename,
					LPCSTR entryPoint, LPCSTR target,
					const D3D_SHADER_MACRO* pDefines = nullptr,
					ID3DInclude* include = D3D_COMPILE_STANDARD_FILE_INCLUDE);
				D3D12_SHADER_BYTECODE Get()const;

			private:
				Microsoft::WRL::ComPtr<ID3DBlob> _shaderBytes;
			};

			ANUTHURRENDERER_API UINT DxgiFormatSize(DXGI_FORMAT format);
		}
	}
}