//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
#pragma once

#include "RendererPCH.h"
#include "DDSTextureLoader.h"

namespace Luxko {
	namespace Anuthur {

		inline void ThrowIfFailed(HRESULT hr, const wchar_t* wstr = nullptr) {
			if (FAILED(hr)) {
				if (wstr) throw std::wstring(wstr) + L"\nError code: " + std::to_wstring(hr);
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
				ResourceDescriptor(D3D12_RESOURCE_DIMENSION dimension, UINT64 width,
					UINT height = 1, UINT16 depth = 1,
					D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE,
					UINT64 alignment = 0, UINT16 mipLevels = 1, UINT sampleCount = 1,
					UINT sampleQuality = 0, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN,
					D3D12_TEXTURE_LAYOUT layout = D3D12_TEXTURE_LAYOUT_UNKNOWN);

				static /*inline*/ ResourceDescriptor Buffer(UINT64 sizeBytes,
					D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE,
					UINT64 alignment = 0);

				static /*inline*/ ResourceDescriptor Tex1D(UINT64 width, DXGI_FORMAT format,
					UINT arraySize = 1, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE,
					UINT64 alignment = 0, UINT16 mipLevels = 1,
					D3D12_TEXTURE_LAYOUT layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR);

				static /*inline*/ ResourceDescriptor Tex2D(UINT64 width, UINT height,
					DXGI_FORMAT format, UINT16 arraySize = 1,
					D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE,
					UINT64 alignment = 0, UINT16 mipLevels = 1,
					D3D12_TEXTURE_LAYOUT layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
					UINT sampleCount = 1, UINT sampleQuality = 0);
				static /*inline*/ ResourceDescriptor Tex3D(UINT64 width, UINT height,
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

			class ANUTHURRENDERER_API ResourceHeapProperties : public D3D12_HEAP_PROPERTIES {
			public:
				ResourceHeapProperties(D3D12_HEAP_TYPE type = D3D12_HEAP_TYPE_DEFAULT,
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
				~VertexBuffersDescriptor() = default;
				void Pop();
				void Push(D3D12_GPU_VIRTUAL_ADDRESS bufferLocation, UINT sizeInBytes, UINT strideInBytes);
				void Apply(ID3D12GraphicsCommandList* cmdlst, UINT startSlot)const;
			private:
				std::vector<D3D12_VERTEX_BUFFER_VIEW> _vbvs;
			};

			class ANUTHURRENDERER_API IndexBufferDescriptor : public D3D12_INDEX_BUFFER_VIEW {
			public:
				IndexBufferDescriptor() {}
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

			class ANUTHURRENDERER_API SRVDescriptor : public D3D12_SHADER_RESOURCE_VIEW_DESC {
			public:
				SRVDescriptor() = default;
				static SRVDescriptor BufferDesc(DXGI_FORMAT format, UINT64 firstElement, UINT byteStride,
					UINT numberELements, D3D12_BUFFER_SRV_FLAGS flags = D3D12_BUFFER_SRV_FLAG_NONE,
					UINT shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING);
				
				static SRVDescriptor Texture1DDesc(DXGI_FORMAT format, UINT mostDetailedMip,
					UINT mipLevels = -1, FLOAT minLODClamp = 0.f,
					UINT shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING);

				static SRVDescriptor Texture1DArrayDesc(DXGI_FORMAT format, UINT mostDetailedMip,
					UINT firstArraySlice, UINT arraySize, UINT mipLevels = -1, FLOAT minLODClamp = 0.f,
					UINT shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING);

				static SRVDescriptor Texture2DDesc(DXGI_FORMAT format, UINT mostDetailedMip,
					UINT planeSlice, UINT mipLevels = -1, FLOAT minLODClamp = 0.f,
					UINT shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING);

				static SRVDescriptor Texture2DArrayDesc(DXGI_FORMAT format, UINT mostDetailedMip,
					UINT planeSlice, UINT firstArraySlice, UINT arraySize, UINT mipLevels = -1, 
					FLOAT minLODClamp = 0.f, UINT shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING);

				static SRVDescriptor Texture2DMSDesc(DXGI_FORMAT format, 
					UINT shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING);

				static SRVDescriptor Texture2DMSArrayDesc(DXGI_FORMAT format, UINT firstArraySlice,
					UINT arraySize, UINT shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING);

				static SRVDescriptor Texture3DDesc(DXGI_FORMAT format, UINT mostDetailedMip,
					UINT mipLevels = -1, FLOAT minLODClamp = 0.f,
					UINT shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING);

				static SRVDescriptor TextureCubeDesc(DXGI_FORMAT format, UINT mostDetailedMip,
					UINT mipLevels = -1, FLOAT minLODClamp = 0.f,
					UINT shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING);

				static SRVDescriptor TextureCubeArrayDesc(DXGI_FORMAT format, UINT mostDetailedMip,
					UINT firstFace, UINT numCubes, UINT mipLevels = -1, FLOAT minLODClamp = 0.f,
					UINT shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING);

			};

			class ANUTHURRENDERER_API RasterizerDescriptor : public D3D12_RASTERIZER_DESC {
			public:
				RasterizerDescriptor(D3D12_FILL_MODE fillMode = D3D12_FILL_MODE_SOLID,
					D3D12_CULL_MODE cullMode = D3D12_CULL_MODE_BACK,
					BOOL frontCounterClockwise = FALSE, INT depthBias = 0,
					FLOAT depthBiaClamp = 0.f, FLOAT slopeScaledDepthBias = 0.f,
					BOOL depthClipEnable = TRUE, BOOL msEnable = FALSE,
					BOOL antiAliasEnable = FALSE, UINT forcedSampleCount = 0u,
					D3D12_CONSERVATIVE_RASTERIZATION_MODE crMode = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF);
			};

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

			class ANUTHURRENDERER_API DescriptorHandleCPU : public D3D12_CPU_DESCRIPTOR_HANDLE {
			public:
				DescriptorHandleCPU(D3D12_CPU_DESCRIPTOR_HANDLE dcdh)
					:D3D12_CPU_DESCRIPTOR_HANDLE(dcdh) {}
				DescriptorHandleCPU& Offset(SIZE_T offsetInBytes);
			};

			class ANUTHURRENDERER_API BlendDescriptor : public D3D12_RENDER_TARGET_BLEND_DESC {
			public:
				BlendDescriptor() {}
				
				static BlendDescriptor Default();
				static BlendDescriptor Blend(D3D12_BLEND srcBlend, D3D12_BLEND destBlend,
					D3D12_BLEND_OP blendOp, D3D12_BLEND srcAlphaBlend, D3D12_BLEND destAlphaBlend,
					D3D12_BLEND_OP alphaBlendOp, UINT8 renderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL);
				static BlendDescriptor Logic(D3D12_LOGIC_OP logicOp,
					UINT8 renderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL);

			};

			class ANUTHURRENDERER_API StencilOperationDescriptor : public D3D12_DEPTH_STENCILOP_DESC {
			public:
				StencilOperationDescriptor() {}
				StencilOperationDescriptor(D3D12_STENCIL_OP stencilFailOp,
					D3D12_STENCIL_OP stencilPassDepthFailOp,
					D3D12_STENCIL_OP stencilPassOp,
					D3D12_COMPARISON_FUNC stencilFunction);

			};

			class ANUTHURRENDERER_API DepthStencilDescriptor : public D3D12_DEPTH_STENCIL_DESC {
			public:
				DepthStencilDescriptor() {}
				DepthStencilDescriptor(BOOL depthEnable, D3D12_DEPTH_WRITE_MASK dwm,
					D3D12_COMPARISON_FUNC dcf, BOOL stencilEnable, UINT8 stencilReadMask,
					UINT8 stencilWriteMask, D3D12_DEPTH_STENCILOP_DESC frontFace,
					D3D12_DEPTH_STENCILOP_DESC backFace);
				static DepthStencilDescriptor Default();
			};

			class ANUTHURMATH_API SwapChainDescriptor : public DXGI_SWAP_CHAIN_DESC {
			public:
				SwapChainDescriptor() {}
				SwapChainDescriptor(UINT width, UINT height, HWND outputWindow,
					DXGI_FORMAT format, UINT bufferCount, BOOL windowed = TRUE,
					UINT sampleCount = 1, UINT sampleQuality = 0,
					DXGI_USAGE bufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
					DXGI_SWAP_EFFECT swapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
					UINT flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH,
					DXGI_MODE_SCANLINE_ORDER scanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
					DXGI_MODE_SCALING scaling = DXGI_MODE_SCALING_UNSPECIFIED,
					UINT refreshRateNumerator = 60, UINT refreshRateDenomerator = 1);
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

			ANUTHURRENDERER_API UINT DxgiFormatBitSize(DXGI_FORMAT format);

			ANUTHURRENDERER_API HRESULT ReadDDSTextureFromMemory(_In_ ID3D12Device* device,
				_In_ ID3D12GraphicsCommandList* cmdList,
				_In_reads_bytes_(ddsDataSize) const uint8_t* ddsData,
				_In_ size_t ddsDataSize,
				_Out_ Microsoft::WRL::ComPtr<ID3D12Resource>& texture,
				_Out_ Microsoft::WRL::ComPtr<ID3D12Resource>& textureUploadHeap,
				_In_ size_t maxsize = 0,
				_Out_opt_ DirectX::DDS_ALPHA_MODE* alphaMode = nullptr
			);

			ANUTHURRENDERER_API HRESULT ReadDDSTextureFromFile(_In_ ID3D12Device* device,
				_In_ ID3D12GraphicsCommandList* cmdList,
				_In_z_ const wchar_t* szFileName,
				_Out_ Microsoft::WRL::ComPtr<ID3D12Resource>& texture,
				_Out_ Microsoft::WRL::ComPtr<ID3D12Resource>& textureUploadHeap,
				_In_ size_t maxsize = 0,
				_Out_opt_ DirectX::DDS_ALPHA_MODE* alphaMode = nullptr
			);

			template<typename RT>
			class UpdateBuffer {
			public:
				UpdateBuffer() {}
				UpdateBuffer(const UpdateBuffer<RT>&) = delete;
				UpdateBuffer& operator=(const UpdateBuffer<RT>&) = delete;
				UpdateBuffer(UpdateBuffer<RT>&& ub) {
					_haveIt = ub._haveIt;
					ub._haveIt = false;
					_buffer = ub._buffer;
					_bufferPtr = ub._bufferPtr;
					ub._buffer = nullptr;
					ub._bufferPtr = nullptr;
				}
				UpdateBuffer& operator=(UpdateBuffer<RT>&& ub) {
					if (_haveIt)_buffer->Unmap(0u, nullptr);
					_haveIt = ub._haveIt;
					ub._haveIt = false;
					_buffer = ub._buffer;
					_bufferPtr = ub._bufferPtr;
					ub._buffer = nullptr;
					ub._bufferPtr = nullptr;
					return *this;
				}

				explicit UpdateBuffer(ID3D12Device* device/*, const RT& data*/) {
					ThrowIfFailed(device->CreateCommittedResource(&ResourceHeapProperties(D3D12_HEAP_TYPE_UPLOAD),
						D3D12_HEAP_FLAG_NONE, &ResourceDescriptor::Buffer(GetCBSizeAligned(sizeof(RT))),
						D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(_buffer.GetAddressOf())));
					ThrowIfFailed(_buffer->Map(0u, nullptr, reinterpret_cast<void**>(&_bufferPtr)));
					_haveIt = true;
				}
				~UpdateBuffer() { if (_haveIt)_buffer->Unmap(0u, nullptr); }
				void Update(const RT& data) {
					CopyMemory(_bufferPtr, &data, /*GetCBSizeAligned(sizeof(RT))*/ sizeof(RT));
				}
				ID3D12Resource* Get()const { return _buffer.Get(); }
				UINT Size()const { 
					return GetCBSizeAligned(sizeof(RT)); }
			private:
				Microsoft::WRL::ComPtr<ID3D12Resource>	_buffer;
				BYTE*									_bufferPtr;
				bool									_haveIt = false;
			};

			struct SubmeshGeometry
			{
				UINT IndexCount = 0;
				UINT StartIndexLocation = 0;
				INT BaseVertexLocation = 0;

				// Bounding box of the geometry defined by this submesh. 
				// This is used in later chapters of the book.
				DirectX::BoundingBox Bounds;
			};

			struct MeshGeometry
			{
				// Give it a name so we can look it up by name.
				std::string Name;

				// System memory copies.  Use Blobs because the vertex/index format can be generic.
				// It is up to the client to cast appropriately.  
				Microsoft::WRL::ComPtr<ID3DBlob> VertexBufferCPU = nullptr;
				Microsoft::WRL::ComPtr<ID3DBlob> IndexBufferCPU = nullptr;

				Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
				Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;

				Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;
				Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferUploader = nullptr;

				// Data about the buffers.
				UINT VertexByteStride = 0;
				UINT VertexBufferByteSize = 0;
				DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
				UINT IndexBufferByteSize = 0;

				// A MeshGeometry may store multiple geometries in one vertex/index buffer.
				// Use this container to define the Submesh geometries so we can draw
				// the Submeshes individually.
				std::unordered_map<std::string, SubmeshGeometry> DrawArgs;

				void InitializeCPUResource(DXGI_FORMAT indexFormat, UINT indiceCount,
					const void* indexData, UINT vertexByteStride, UINT verticeCount,
					const void* vertexData) {
					IndexFormat = indexFormat;
					IndexBufferByteSize = D3D12Helper::DxgiFormatBitSize(indexFormat) / 8 * indiceCount;
					ThrowIfFailed(D3DCreateBlob(IndexBufferByteSize, IndexBufferCPU.GetAddressOf()));
					CopyMemory(IndexBufferCPU->GetBufferPointer(), indexData, IndexBufferByteSize);

					VertexByteStride = vertexByteStride;
					VertexBufferByteSize = verticeCount * vertexByteStride;
					ThrowIfFailed(D3DCreateBlob(VertexBufferByteSize, VertexBufferCPU.GetAddressOf()));
					CopyMemory(VertexBufferCPU->GetBufferPointer(), vertexData, VertexBufferByteSize);
				}

				void RecordUpdateFromCPUtoGPU(ID3D12Device* device, ID3D12GraphicsCommandList* commandList) {
					VertexBufferGPU = D3D12Helper::CreateDefaultBuffer(device, commandList,
						VertexBufferCPU->GetBufferPointer(), VertexBufferByteSize,
						VertexBufferUploader);
					IndexBufferGPU = D3D12Helper::CreateDefaultBuffer(device, commandList,
						IndexBufferCPU->GetBufferPointer(), IndexBufferByteSize,
						IndexBufferUploader);
				}

				D3D12_VERTEX_BUFFER_VIEW VertexBufferView()const
				{
					D3D12_VERTEX_BUFFER_VIEW vbv;
					vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
					vbv.StrideInBytes = VertexByteStride;
					vbv.SizeInBytes = VertexBufferByteSize;

					return vbv;
				}

				D3D12_INDEX_BUFFER_VIEW IndexBufferView()const
				{
					D3D12_INDEX_BUFFER_VIEW ibv;
					ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
					ibv.Format = IndexFormat;
					ibv.SizeInBytes = IndexBufferByteSize;

					return ibv;
				}

				// We can free this memory after we finish upload to the GPU.
				void DisposeUploaders()
				{
					VertexBufferUploader = nullptr;
					IndexBufferUploader = nullptr;
				}

				UINT GetTotoalIndexCount()const {
					return static_cast<UINT>(IndexBufferByteSize * 8ll / D3D12Helper::DxgiFormatBitSize(IndexFormat));
				}
				UINT GetTotalVertexCount()const {
					return VertexBufferByteSize / VertexByteStride;
				}
			};

		}
	}
}