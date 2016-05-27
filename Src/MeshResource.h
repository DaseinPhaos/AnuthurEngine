//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
#pragma once
#include "RendererPCH.h"
#include "HelperMethods.h"
#include "Transform3DH.h"

namespace Luxko {
	namespace Anuthur {
#pragma region Shit
		////enum class ANUTHURRENDERER_API MeshPrimitiveType {
		////	Triangle = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		////	Line = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,
		////	Point = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT
		////};

		////enum class ANUTHURRENDERER_API MeshPrimitiveIndexFormat {
		////	uint16,
		////	uint32
		////};

		//// Mesh primitives serves as the building block for a mesh.
		//// It provides and manages data in CPU in a consistent format.
		//class ANUTHURRENDERER_API IMeshPrimitive abstract {
		//public:
		//	IMeshPrimitive() {}
		//	IMeshPrimitive(UINT vSizeInBytes, UINT vCount, UINT iCount,
		//		DXGI_FORMAT iFormat = DXGI_FORMAT_R16_UINT,
		//		D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveType
		//			= D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE) 
		//		:_verticeSize(vSizeInBytes), _verticeCount(vCount), 
		//		_indiceCount(iCount), _indiceFormat(iFormat),
		//		_type(primitiveType) { };

		//	IMeshPrimitive(const IMeshPrimitive&) = default;
		//	IMeshPrimitive& operator=(const IMeshPrimitive&) = default;
		//	virtual ~IMeshPrimitive() {}
		//	virtual Frame3DH GetOrientation()const = 0;
		//	virtual const void* GetVerticeData()const = 0;
		//	virtual const void* GetIndiceData()const = 0;
		//	D3D12_PRIMITIVE_TOPOLOGY_TYPE GetTopologyType()const noexcept { return _type; }
		//	UINT GetVerticeCount()const noexcept { return _verticeCount; }
		//	UINT GetVertexSize()const noexcept { return _verticeSize; }
		//	UINT GetIndiceCount()const noexcept { return _indiceCount; }
		//	DXGI_FORMAT GetIndexFormat()const noexcept { return _indiceFormat; }

		//	UINT							_verticeCount;
		//	UINT							_indiceCount;
		//	UINT							_verticeSize;
		//	DXGI_FORMAT						_indiceFormat;
		//	D3D12_PRIMITIVE_TOPOLOGY_TYPE	_type;
		//};

		//// This class describes the mesh data to the GPU.
		//// It should also manage the lifetime of these GPU resources,
		//// and the lifetime of the IMeshPrimitives it use.
		//class ANUTHURRENDERER_API IMesh abstract{
		//public:
		//	IMesh() {}
		//	IMesh(const IMesh&) = default;
		//	IMesh& operator=(const IMesh&) = default;
		//	virtual ~IMesh() {}

		//	virtual D3D12_VERTEX_BUFFER_VIEW* GetTriangleVBV()const { return nullptr; }
		//	virtual D3D12_VERTEX_BUFFER_VIEW* GetLineVBV()const { return nullptr; }
		//	virtual D3D12_VERTEX_BUFFER_VIEW* GetPointVBV()const { return nullptr; }
		//	
		//	virtual D3D12_INDEX_BUFFER_VIEW* GetTriangleIBV()const { return nullptr; }
		//	virtual D3D12_INDEX_BUFFER_VIEW* GetLineIBV()const { return nullptr; }
		//	virtual D3D12_INDEX_BUFFER_VIEW* GetPointIBV()const { return nullptr; }

		//};
#pragma endregion Shit

		// @Luxko:	The following two utility class is pasted directly
		//			from d3dUtil.h by Frank Luna (C)
		//			which is part of the library built for book
		//			*Introduction to 3D Game Programming with DirectX 12*.
		//
		//			I've added some utility method to it.


		// Defines a subrange of geometry in a MeshGeometry.  This is for when multiple
		// geometries are stored in one vertex and index buffer.  It provides the offsets
		// and data needed to draw a subset of geometry stores in the vertex and index 
		// buffers so that we can implement the technique described by Figure 6.3.
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