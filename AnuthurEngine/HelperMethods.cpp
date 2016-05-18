//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#include "HelperMethods.h"

Luxko::Anuthur::D3D12Helper::InputElementDescriptor::InputElementDescriptor(LPCSTR semanticName,
	DXGI_FORMAT format, UINT bytedOffsetAligned, UINT inputSlot /*= 0u*/,
	UINT semanticIndex/* = 0u*/, UINT instanceDataSR /*= 0u*/,
	D3D12_INPUT_CLASSIFICATION classification /*= D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA*/)
	:D3D12_INPUT_ELEMENT_DESC{ semanticName, semanticIndex, format, inputSlot, bytedOffsetAligned,
		classification, instanceDataSR } {}

void Luxko::Anuthur::D3D12Helper::InputLayoutDescriptor::PushElementDescription(LPCSTR semanticName, DXGI_FORMAT format, UINT inputSlot /*= 0u*/, UINT semanticIndex /*= 0u*/, UINT instanceDataSR /*= 0u*/, D3D12_INPUT_CLASSIFICATION classification /*= D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA*/)
{
	assert(inputSlot < 16);
	_ilds.emplace_back(semanticName, format, _currentSlotOffset[inputSlot],
		inputSlot, semanticIndex, instanceDataSR, classification);
	auto size = sizeof(format);
	while (size % 4 != 0u) size++;
	_currentSlotOffset[inputSlot] += size;
}

D3D12_INPUT_LAYOUT_DESC Luxko::Anuthur::D3D12Helper::InputLayoutDescriptor::GetDescription() const
{
	D3D12_INPUT_LAYOUT_DESC result;
	result.NumElements = _ilds.size();
	result.pInputElementDescs = _ilds.data();
	return result;
}

Luxko::Anuthur::D3D12Helper::ResourceDescriptor::ResourceDescriptor(D3D12_RESOURCE_DIMENSION dimension, UINT64 width, UINT height /*= 1*/, UINT16 depth /*= 1*/, D3D12_RESOURCE_FLAGS flags /*= D3D12_RESOURCE_FLAG_NONE*/, UINT64 alignment /*= 0*/, UINT16 mipLevels /*= 1*/, UINT sampleCount /*= 1*/, UINT sampleQuality /*= 0*/, DXGI_FORMAT format /*= DXGI_FORMAT_UNKNOWN*/, D3D12_TEXTURE_LAYOUT layout /*= D3D12_TEXTURE_LAYOUT_UNKNOWN*/)
	:D3D12_RESOURCE_DESC{ dimension, alignment, width, height, depth, mipLevels, format, DXGI_SAMPLE_DESC{sampleCount,sampleQuality}, layout, flags } {}

Luxko::Anuthur::D3D12Helper::ResourceDescriptor Luxko::Anuthur::D3D12Helper::ResourceDescriptor::Buffer(UINT64 sizeBytes, D3D12_RESOURCE_FLAGS flags /*= D3D12_RESOURCE_FLAG_NONE*/, UINT64 alignment /*= 0*/)
{
	return ResourceDescriptor(D3D12_RESOURCE_DIMENSION_BUFFER, sizeBytes, 1, 1, flags,
		alignment, 1, 1, 0, DXGI_FORMAT_UNKNOWN, D3D12_TEXTURE_LAYOUT_ROW_MAJOR);
}

Luxko::Anuthur::D3D12Helper::ResourceDescriptor Luxko::Anuthur::D3D12Helper::ResourceDescriptor::Tex1D(UINT64 width, DXGI_FORMAT format, UINT arraySize /*= 1*/, D3D12_RESOURCE_FLAGS flags /*= D3D12_RESOURCE_FLAG_NONE*/, UINT64 alignment /*= 0*/, UINT16 mipLevels /*= 1*/, D3D12_TEXTURE_LAYOUT layout /*= D3D12_TEXTURE_LAYOUT_ROW_MAJOR*/)
{
	return ResourceDescriptor(D3D12_RESOURCE_DIMENSION_TEXTURE1D, width, 1, arraySize,
		flags, alignment, mipLevels, 1, 0, format, layout);
}

Luxko::Anuthur::D3D12Helper::ResourceDescriptor Luxko::Anuthur::D3D12Helper::ResourceDescriptor::Tex2D(UINT64 width, UINT height, DXGI_FORMAT format, UINT16 arraySize /*= 1*/, D3D12_RESOURCE_FLAGS flags /*= D3D12_RESOURCE_FLAG_NONE*/, UINT64 alignment /*= 0*/, UINT16 mipLevels /*= 1*/, D3D12_TEXTURE_LAYOUT layout /*= D3D12_TEXTURE_LAYOUT_ROW_MAJOR*/, UINT sampleCount /*= 1*/, UINT sampleQuality /*= 0*/)
{
	return ResourceDescriptor(D3D12_RESOURCE_DIMENSION_TEXTURE2D, width, height, arraySize, flags,
		alignment, mipLevels, sampleCount, sampleQuality, format, layout);
}

Luxko::Anuthur::D3D12Helper::ResourceDescriptor Luxko::Anuthur::D3D12Helper::ResourceDescriptor::Tex3D(UINT64 width, UINT height, UINT16 depth, DXGI_FORMAT format, D3D12_RESOURCE_FLAGS flags /*= D3D12_RESOURCE_FLAG_NONE*/, UINT64 alignment /*= 0*/, UINT16 mipLevels /*= 1*/, D3D12_TEXTURE_LAYOUT layout /*= D3D12_TEXTURE_LAYOUT_ROW_MAJOR*/)
{
	return ResourceDescriptor(D3D12_RESOURCE_DIMENSION_TEXTURE3D, width, height, depth, flags,
		alignment, mipLevels, 1, 0, format, layout);
}

Luxko::Anuthur::D3D12Helper::ResourceBarrier Luxko::Anuthur::D3D12Helper::ResourceBarrier::TransitionBarrier(ID3D12Resource* resource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter, D3D12_RESOURCE_BARRIER_FLAGS flags, UINT subResource /*= D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES*/)
{
	ResourceBarrier result;
	result.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	result.Flags = flags;
	result.Transition.StateBefore = stateBefore;
	result.Transition.StateAfter = stateAfter;
	result.Transition.pResource = resource;
	result.Transition.Subresource = subResource;
	return result;
}

Luxko::Anuthur::D3D12Helper::ResourceBarrier Luxko::Anuthur::D3D12Helper::ResourceBarrier::UAVBarrier(ID3D12Resource* resource, D3D12_RESOURCE_BARRIER_FLAGS flags)
{
	ResourceBarrier result;
	result.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	result.Flags = flags;
	result.UAV.pResource = resource;
	return result;
}

Luxko::Anuthur::D3D12Helper::ResourceBarrier Luxko::Anuthur::D3D12Helper::ResourceBarrier::AliasingBarrier(ID3D12Resource* resourceBefore /*= nullptr*/, ID3D12Resource* resourceAfter /*= nullptr*/, D3D12_RESOURCE_BARRIER_FLAGS flags /*= D3D12_RESOURCE_BARRIER_FLAG_NONE*/)
{
	ResourceBarrier result;
	result.Type = D3D12_RESOURCE_BARRIER_TYPE_ALIASING;
	result.Flags = flags;
	result.Aliasing.pResourceBefore = resourceBefore;
	result.Aliasing.pResourceAfter = resourceAfter;
	return result;
}

Luxko::Anuthur::D3D12Helper::HeapProperties::HeapProperties(D3D12_HEAP_TYPE type /*= D3D12_HEAP_TYPE_DEFAULT*/, D3D12_CPU_PAGE_PROPERTY cpuPageProperty /*= D3D12_CPU_PAGE_PROPERTY_NOT_AVAILABLE*/, D3D12_MEMORY_POOL memoryPoolPref /*= D3D12_MEMORY_POOL_UNKNOWN*/, UINT CreationNodeMask /*= 0*/, UINT visibleNodeMask /*= 0*/)
	:D3D12_HEAP_PROPERTIES{ type, cpuPageProperty, memoryPoolPref, CreationNodeMask, visibleNodeMask } { }

ANUTHURRENDERER_API Microsoft::WRL::ComPtr<ID3D12Resource> Luxko::Anuthur::D3D12Helper::CreateDefaultBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdlst, const void* data, UINT64 sizeInBytes, Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer)
{
	Microsoft::WRL::ComPtr<ID3D12Resource> defaultBuffer;

	ThrowIfFailed(device->CreateCommittedResource(&HeapProperties(), D3D12_HEAP_FLAG_NONE,
		&ResourceDescriptor::Buffer(sizeInBytes), D3D12_RESOURCE_STATE_COMMON,
		nullptr, IID_PPV_ARGS(defaultBuffer.GetAddressOf())));

	ThrowIfFailed(device->CreateCommittedResource(&HeapProperties(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE, &ResourceDescriptor::Buffer(sizeInBytes),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(uploadBuffer.GetAddressOf())));

	D3D12_SUBRESOURCE_DATA sbData = { data, sizeInBytes, sizeInBytes };

	cmdlst->ResourceBarrier(1, &ResourceBarrier::TransitionBarrier(defaultBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
	UpdateSubresources<1>(cmdlst, defaultBuffer.Get(), uploadBuffer.Get(),
		0, 0, 1, &sbData);
	cmdlst->ResourceBarrier(1, &ResourceBarrier::TransitionBarrier(defaultBuffer.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));

	return defaultBuffer;
	
}

void Luxko::Anuthur::D3D12Helper::VertexBuffersDescriptor::Pop()
{
	if (!_vbvs.empty()) _vbvs.pop_back();
}

void Luxko::Anuthur::D3D12Helper::VertexBuffersDescriptor::Push(D3D12_GPU_VIRTUAL_ADDRESS bufferLocation, UINT sizeInBytes, UINT strideInBytes)
{
	_vbvs.push_back({ bufferLocation, sizeInBytes, strideInBytes });
}

void Luxko::Anuthur::D3D12Helper::VertexBuffersDescriptor::Apply(ID3D12GraphicsCommandList* cmdlst, UINT startSlot) const
{
	cmdlst->IASetVertexBuffers(startSlot, _vbvs.size(), _vbvs.data());
}

void Luxko::Anuthur::D3D12Helper::IndexBufferDescriptor::Apply(ID3D12GraphicsCommandList* cmdlst) const
{
	cmdlst->IASetIndexBuffer(this);
}
