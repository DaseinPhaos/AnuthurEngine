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

				D3D12_INPUT_LAYOUT_DESC GetDescription()const;
			private:
				std::vector<InputElementDescriptor> _ilds;
				UINT _currentSlotOffset[16] = { 0u };

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
					D3D12_CPU_PAGE_PROPERTY cpuPageProperty = D3D12_CPU_PAGE_PROPERTY_NOT_AVAILABLE,
					D3D12_MEMORY_POOL memoryPoolPref = D3D12_MEMORY_POOL_UNKNOWN,
					UINT CreationNodeMask = 0, UINT visibleNodeMask = 0);
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
					UINT sizeInBytes, DXGI_FORMAT format) :
					D3D12_INDEX_BUFFER_VIEW{ bufferLocation, sizeInBytes, format } {}

				void Apply(ID3D12GraphicsCommandList* cmdlst)const;
			};

			ANUTHURRENDERER_API Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
				ID3D12Device* device, ID3D12GraphicsCommandList* cmdlst,
				const void* data, UINT64 sizeInBytes,
				Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);

		}
	}
}