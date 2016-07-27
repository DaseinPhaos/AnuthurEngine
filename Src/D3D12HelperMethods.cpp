//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#include "D3D12HelperMethods.h"

Luxko::Anuthur::D3D12Helper::InputElementDescriptor::InputElementDescriptor(LPCSTR semanticName, DXGI_FORMAT format, UINT bytedOffsetAligned, UINT inputSlot /*= 0u*/, UINT semanticIndex/* = 0u*/, UINT instanceDataSR /*= 0u*/, D3D12_INPUT_CLASSIFICATION classification /*= D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA*/)
	:D3D12_INPUT_ELEMENT_DESC{ semanticName, semanticIndex, format, inputSlot, bytedOffsetAligned,
		classification, instanceDataSR } {}

void Luxko::Anuthur::D3D12Helper::InputLayoutDescriptor::PushElementDescription(LPCSTR semanticName, DXGI_FORMAT format, UINT inputSlot /*= 0u*/, UINT semanticIndex /*= 0u*/, UINT instanceDataSR /*= 0u*/, D3D12_INPUT_CLASSIFICATION classification /*= D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA*/)
{
	assert(inputSlot < MaxInputSlotCount);
	_ilds.emplace_back(semanticName, format, _currentSlotOffset[inputSlot],
		inputSlot, semanticIndex, instanceDataSR, classification);
	auto size = DxgiFormatBitSize(format);
	size /= 8u;
	while (size % 4 != 0u) size++;
	_currentSlotOffset[inputSlot] += size;
}

D3D12_INPUT_LAYOUT_DESC Luxko::Anuthur::D3D12Helper::InputLayoutDescriptor::Get() const
{
	D3D12_INPUT_LAYOUT_DESC result;
	result.NumElements = static_cast<UINT>(_ilds.size());
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

Luxko::Anuthur::D3D12Helper::ResourceHeapProperties::ResourceHeapProperties(D3D12_HEAP_TYPE type /*= D3D12_HEAP_TYPE_DEFAULT*/, D3D12_CPU_PAGE_PROPERTY cpuPageProperty /*= D3D12_CPU_PAGE_PROPERTY_UNKOWN*/, D3D12_MEMORY_POOL memoryPoolPref /*= D3D12_MEMORY_POOL_UNKNOWN*/, UINT CreationNodeMask /*= 0*/, UINT visibleNodeMask /*= 0*/)
	:D3D12_HEAP_PROPERTIES{ type, cpuPageProperty, memoryPoolPref, CreationNodeMask, visibleNodeMask } { }

UINT Luxko::Anuthur::D3D12Helper::GetCBSizeAligned(size_t sizeOfObject)
{
	return (sizeOfObject + MinHardwareAllocSize) & (~(MinHardwareAllocSize - 1));
}

HRESULT Luxko::Anuthur::D3D12Helper::CompileShader(LPCWSTR filename, LPCSTR entryPoint, LPCSTR target, ID3DBlob** ppCompiled, ID3DBlob** ppErrorMsg /*= nullptr*/, const D3D_SHADER_MACRO* pDefines /*= nullptr*/, ID3DInclude* include /*= nullptr*/)
{
	UINT flags1 = 0u;
#if defined(DEBUG) || defined(_DEBUG)
	flags1 = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	return D3DCompileFromFile(filename, pDefines, include, entryPoint, target, flags1, 0, ppCompiled, ppErrorMsg);
}

UINT Luxko::Anuthur::D3D12Helper::DxgiFormatBitSize(DXGI_FORMAT format)
{
	switch (format)
	{
	case DXGI_FORMAT_R32G32B32A32_TYPELESS:
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
	case DXGI_FORMAT_R32G32B32A32_UINT:
	case DXGI_FORMAT_R32G32B32A32_SINT:
		return 128u;

	case DXGI_FORMAT_R32G32B32_TYPELESS:
	case DXGI_FORMAT_R32G32B32_FLOAT:
	case DXGI_FORMAT_R32G32B32_UINT:
	case DXGI_FORMAT_R32G32B32_SINT:
		return 96u;

	case DXGI_FORMAT_R16G16B16A16_TYPELESS:
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
	case DXGI_FORMAT_R16G16B16A16_UNORM:
	case DXGI_FORMAT_R16G16B16A16_UINT:
	case DXGI_FORMAT_R16G16B16A16_SNORM:
	case DXGI_FORMAT_R16G16B16A16_SINT:
	case DXGI_FORMAT_R32G32_TYPELESS:
	case DXGI_FORMAT_R32G32_FLOAT:
	case DXGI_FORMAT_R32G32_UINT:
	case DXGI_FORMAT_R32G32_SINT:
	case DXGI_FORMAT_R32G8X24_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
		return 64u;

	case DXGI_FORMAT_R10G10B10A2_TYPELESS:
	case DXGI_FORMAT_R10G10B10A2_UNORM:
	case DXGI_FORMAT_R10G10B10A2_UINT:
	case DXGI_FORMAT_R11G11B10_FLOAT:
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R8G8B8A8_SINT:
	case DXGI_FORMAT_R16G16_TYPELESS:
	case DXGI_FORMAT_R16G16_FLOAT:
	case DXGI_FORMAT_R16G16_UNORM:
	case DXGI_FORMAT_R16G16_UINT:
	case DXGI_FORMAT_R16G16_SNORM:
	case DXGI_FORMAT_R16G16_SINT:
	case DXGI_FORMAT_R32_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT:
	case DXGI_FORMAT_R32_FLOAT:
	case DXGI_FORMAT_R32_UINT:
	case DXGI_FORMAT_R32_SINT:
	case DXGI_FORMAT_R24G8_TYPELESS:
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
	case DXGI_FORMAT_B8G8R8A8_UNORM:
	case DXGI_FORMAT_B8G8R8X8_UNORM:
		return 32u;

	case DXGI_FORMAT_R8G8_TYPELESS:
	case DXGI_FORMAT_R8G8_UNORM:
	case DXGI_FORMAT_R8G8_UINT:
	case DXGI_FORMAT_R8G8_SNORM:
	case DXGI_FORMAT_R8G8_SINT:
	case DXGI_FORMAT_R16_TYPELESS:
	case DXGI_FORMAT_R16_FLOAT:
	case DXGI_FORMAT_D16_UNORM:
	case DXGI_FORMAT_R16_UNORM:
	case DXGI_FORMAT_R16_UINT:
	case DXGI_FORMAT_R16_SNORM:
	case DXGI_FORMAT_R16_SINT:
	case DXGI_FORMAT_B5G6R5_UNORM:
	case DXGI_FORMAT_B5G5R5A1_UNORM:
		return 16u;

	case DXGI_FORMAT_R8_TYPELESS:
	case DXGI_FORMAT_R8_UNORM:
	case DXGI_FORMAT_R8_UINT:
	case DXGI_FORMAT_R8_SNORM:
	case DXGI_FORMAT_R8_SINT:
	case DXGI_FORMAT_A8_UNORM:
		return 8u;

		// Compressed format; http://msdn2.microsoft.com/en-us/library/bb694531(VS.85).aspx
	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
	case DXGI_FORMAT_BC5_TYPELESS:
	case DXGI_FORMAT_BC5_UNORM:
	case DXGI_FORMAT_BC5_SNORM:
		return 128u;

		// Compressed format; http://msdn2.microsoft.com/en-us/library/bb694531(VS.85).aspx
	case DXGI_FORMAT_R1_UNORM:
	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC4_TYPELESS:
	case DXGI_FORMAT_BC4_UNORM:
	case DXGI_FORMAT_BC4_SNORM:
		return 64u;

		// Compressed format; http://msdn2.microsoft.com/en-us/library/bb694531(VS.85).aspx
	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
		return 32u;

		// These are compressed, but bit-size information is unclear.
	case DXGI_FORMAT_R8G8_B8G8_UNORM:
	case DXGI_FORMAT_G8R8_G8B8_UNORM:
		return 32u;

	case DXGI_FORMAT_UNKNOWN:
	default:
		throw "Cannot determine format element size; invalid format specified.";

	}
}

HRESULT Luxko::Anuthur::D3D12Helper::ReadDDSTextureFromMemory(_In_ ID3D12Device* device, _In_ ID3D12GraphicsCommandList* cmdList, _In_reads_bytes_(ddsDataSize) const uint8_t* ddsData, _In_ size_t ddsDataSize, _Out_ Microsoft::WRL::ComPtr<ID3D12Resource>& texture, _Out_ Microsoft::WRL::ComPtr<ID3D12Resource>& textureUploadHeap, _In_ size_t maxsize /*= 0*/, _Out_opt_ DirectX::DDS_ALPHA_MODE* alphaMode /*= nullptr */)
{
	return DirectX::CreateDDSTextureFromMemory12(device, cmdList, ddsData, ddsDataSize, texture, textureUploadHeap, maxsize, alphaMode);
}

HRESULT Luxko::Anuthur::D3D12Helper::ReadDDSTextureFromFile(_In_ ID3D12Device* device, _In_ ID3D12GraphicsCommandList* cmdList, _In_z_ const wchar_t* szFileName, _Out_ Microsoft::WRL::ComPtr<ID3D12Resource>& texture, _Out_ Microsoft::WRL::ComPtr<ID3D12Resource>& textureUploadHeap, _In_ size_t maxsize /*= 0*/, _Out_opt_ DirectX::DDS_ALPHA_MODE* alphaMode /*= nullptr */)
{
	return DirectX::CreateDDSTextureFromFile12(device, cmdList, szFileName, texture, textureUploadHeap, maxsize, alphaMode);
}

Microsoft::WRL::ComPtr<ID3D12Resource> Luxko::Anuthur::D3D12Helper::CreateDefaultBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdlst, const void* data, UINT64 sizeInBytes, Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer)
{
	uploadBuffer.Reset();
	Microsoft::WRL::ComPtr<ID3D12Resource> defaultBuffer;

	ThrowIfFailed(device->CreateCommittedResource(&ResourceHeapProperties(), D3D12_HEAP_FLAG_NONE,
		&ResourceDescriptor::Buffer(sizeInBytes), D3D12_RESOURCE_STATE_COMMON,
		nullptr, IID_PPV_ARGS(defaultBuffer.GetAddressOf())));

	ThrowIfFailed(device->CreateCommittedResource(&ResourceHeapProperties(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE, &ResourceDescriptor::Buffer(sizeInBytes),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(uploadBuffer.GetAddressOf())));

	D3D12_SUBRESOURCE_DATA sbData = { data, static_cast<LONG_PTR>(sizeInBytes), static_cast<LONG_PTR>(sizeInBytes) };

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
	cmdlst->IASetVertexBuffers(startSlot, static_cast<UINT>(_vbvs.size()), _vbvs.data());
}

void Luxko::Anuthur::D3D12Helper::IndexBufferDescriptor::Apply(ID3D12GraphicsCommandList* cmdlst) const
{
	cmdlst->IASetIndexBuffer(this);
}

Luxko::Anuthur::D3D12Helper::DescriptorHeapDescriptor::DescriptorHeapDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT count /*= 1u*/, D3D12_DESCRIPTOR_HEAP_FLAGS flags /*= D3D12_DESCRIPTOR_HEAP_FLAG_NONE*/, UINT nodeMask /*= 0u*/)
	:D3D12_DESCRIPTOR_HEAP_DESC{ type, count, flags, nodeMask } { }

Luxko::Anuthur::D3D12Helper::CBVDescriptor::CBVDescriptor(D3D12_GPU_VIRTUAL_ADDRESS location, UINT sizeInBytes)
	: D3D12_CONSTANT_BUFFER_VIEW_DESC{ location, sizeInBytes } {}

Luxko::Anuthur::D3D12Helper::CBVDescriptor::CBVDescriptor(ID3D12Resource* buffer, UINT64 offsetFromBufferStart, UINT sizeInBytes)
{
	auto start = buffer->GetGPUVirtualAddress();
	BufferLocation = start + offsetFromBufferStart;
	SizeInBytes = sizeInBytes;
}

Luxko::Anuthur::D3D12Helper::RootDescriptorRange::RootDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE type, UINT numDescriptors /*= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND*/, UINT baseShaderRegister /*= 0u*/, UINT registerSpace /*= 0u*/, UINT offsetInDescriptorsFromTableStart /*= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND*/)
	:D3D12_DESCRIPTOR_RANGE{ type, numDescriptors, baseShaderRegister, registerSpace, offsetInDescriptorsFromTableStart } {}

void Luxko::Anuthur::D3D12Helper::RootDescriptorRange::Init(D3D12_DESCRIPTOR_RANGE_TYPE type, UINT numDescriptors /*= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND*/, UINT baseShaderRegister /*= 0u*/, UINT registerSpace /*= 0u*/, UINT offsetInDescriptorsFromTableStart /*= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND*/)
{
	NumDescriptors = numDescriptors;
	BaseShaderRegister = baseShaderRegister;
	RegisterSpace = registerSpace;
	OffsetInDescriptorsFromTableStart = offsetInDescriptorsFromTableStart;
}

void Luxko::Anuthur::D3D12Helper::RootDescriptorTable::Pop()
{
	if (!_dRanges.empty()) _dRanges.pop_back();
}

void Luxko::Anuthur::D3D12Helper::RootDescriptorTable::Push(const RootDescriptorRange& range)
{
	_dRanges.push_back(range);
}

void Luxko::Anuthur::D3D12Helper::RootDescriptorTable::Emplace(D3D12_DESCRIPTOR_RANGE_TYPE type, UINT numDescriptors /*= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND*/, UINT baseShaderRegister /*= 0u*/, UINT registerSpace /*= 0u*/, UINT offsetInDescriptorsFromTableStart /*= D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND*/)
{
	_dRanges.emplace_back(type, numDescriptors, baseShaderRegister, registerSpace, offsetInDescriptorsFromTableStart);
}

D3D12_ROOT_DESCRIPTOR_TABLE Luxko::Anuthur::D3D12Helper::RootDescriptorTable::Get() const
{
	D3D12_ROOT_DESCRIPTOR_TABLE result;
	result.NumDescriptorRanges = static_cast<UINT>(_dRanges.size());
	result.pDescriptorRanges = _dRanges.data();
	return result;
}

Luxko::Anuthur::D3D12Helper::RootParameter Luxko::Anuthur::D3D12Helper::RootParameter::Constant(UINT shaderRegister, UINT num32BitValues, UINT registerSpace /*= 0u*/, D3D12_SHADER_VISIBILITY visb /*= D3D12_SHADER_VISIBILITY_ALL*/)
{
	RootParameter rp;
	rp.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	rp.Constants.Num32BitValues = num32BitValues;
	rp.Constants.RegisterSpace = registerSpace;
	rp.Constants.ShaderRegister = shaderRegister;
	rp.ShaderVisibility = visb;
	return rp;
}

Luxko::Anuthur::D3D12Helper::RootParameter Luxko::Anuthur::D3D12Helper::RootParameter::DescriptorTablef(D3D12_ROOT_DESCRIPTOR_TABLE table, D3D12_SHADER_VISIBILITY visb /*= D3D12_SHADER_VISIBILITY_ALL*/)
{
	RootParameter rp;
	rp.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp.DescriptorTable = table;
	rp.ShaderVisibility = visb;
	return rp;
}

Luxko::Anuthur::D3D12Helper::RootParameter Luxko::Anuthur::D3D12Helper::RootParameter::CBV(UINT shaderRegister, UINT registerSpace /*= 0u*/, D3D12_SHADER_VISIBILITY visb /*= D3D12_SHADER_VISIBILITY_ALL*/)
{
	RootParameter rp;
	rp.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rp.Descriptor.RegisterSpace = registerSpace;
	rp.Descriptor.ShaderRegister = shaderRegister;
	rp.ShaderVisibility = visb;
	return rp;
}

Luxko::Anuthur::D3D12Helper::RootParameter Luxko::Anuthur::D3D12Helper::RootParameter::SRV(UINT shaderRegister, UINT registerSpace /*= 0u*/, D3D12_SHADER_VISIBILITY visb /*= D3D12_SHADER_VISIBILITY_ALL*/)
{
	RootParameter rp;
	rp.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	rp.Descriptor.RegisterSpace = registerSpace;
	rp.Descriptor.ShaderRegister = shaderRegister;
	rp.ShaderVisibility = visb;
	return rp;
}

Luxko::Anuthur::D3D12Helper::RootParameter Luxko::Anuthur::D3D12Helper::RootParameter::UAV(UINT shaderRegister, UINT registerSpace /*= 0u*/, D3D12_SHADER_VISIBILITY visb /*= D3D12_SHADER_VISIBILITY_ALL*/)
{
	RootParameter rp;
	rp.ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
	rp.Descriptor.RegisterSpace = registerSpace;
	rp.Descriptor.ShaderRegister = shaderRegister;
	rp.ShaderVisibility = visb;
	return rp;
}

void Luxko::Anuthur::D3D12Helper::StaticSamplerDescriptor::Init(UINT shaderRegister, D3D12_FILTER filter /*= D3D12_FILTER_ANISOTROPIC*/, D3D12_TEXTURE_ADDRESS_MODE addressU /*= D3D12_TEXTURE_ADDRESS_MODE_WRAP*/, D3D12_TEXTURE_ADDRESS_MODE addressV /*= D3D12_TEXTURE_ADDRESS_MODE_WRAP*/, D3D12_TEXTURE_ADDRESS_MODE addressW /*= D3D12_TEXTURE_ADDRESS_MODE_WRAP*/, FLOAT mipLODBias /*= 0*/, UINT maxAnisotropy /*= 16*/, D3D12_COMPARISON_FUNC comparisonFunc /*= D3D12_COMPARISON_FUNC_LESS_EQUAL*/, D3D12_STATIC_BORDER_COLOR borderColor /*= D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE*/, FLOAT minLOD /*= 0.f*/, FLOAT maxLOD /*= D3D12_FLOAT32_MAX*/, D3D12_SHADER_VISIBILITY shaderVisibility /*= D3D12_SHADER_VISIBILITY_ALL*/, UINT registerSpace /*= 0*/)
{
	ShaderRegister = shaderRegister;
	Filter = filter;
	AddressU = addressU;
	AddressV = addressV;
	AddressW = addressW;
	MipLODBias = mipLODBias;
	MaxAnisotropy = maxAnisotropy;
	ComparisonFunc = comparisonFunc;
	BorderColor = borderColor;
	MinLOD = minLOD;
	MaxLOD = maxLOD;
	ShaderVisibility = shaderVisibility;
	RegisterSpace = registerSpace;
}

void Luxko::Anuthur::D3D12Helper::RootSignatureDescriptor::PopRootParameter()
{
	if (!_rootParamters.empty()) _rootParamters.pop_back();
}

void Luxko::Anuthur::D3D12Helper::RootSignatureDescriptor::PushRootParameter(const RootParameter& rp)
{
	_rootParamters.push_back(rp);
}

void Luxko::Anuthur::D3D12Helper::RootSignatureDescriptor::PushRPConstant(UINT shaderRegister, UINT num32BitValues, UINT registerSpace /*= 0u*/, D3D12_SHADER_VISIBILITY visb /*= D3D12_SHADER_VISIBILITY_ALL*/)
{
	_rootParamters.push_back(RootParameter::Constant(shaderRegister, num32BitValues, registerSpace, visb));
}

void Luxko::Anuthur::D3D12Helper::RootSignatureDescriptor::PushRPDescriptorTable(D3D12_ROOT_DESCRIPTOR_TABLE table, D3D12_SHADER_VISIBILITY visb /*= D3D12_SHADER_VISIBILITY_ALL*/)
{
	_rootParamters.push_back(RootParameter::DescriptorTablef(table, visb));
}

void Luxko::Anuthur::D3D12Helper::RootSignatureDescriptor::PushRPCBV(UINT shaderRegister, UINT registerSpace /*= 0u*/, D3D12_SHADER_VISIBILITY visb /*= D3D12_SHADER_VISIBILITY_ALL*/)
{
	_rootParamters.push_back(RootParameter::CBV(shaderRegister, registerSpace, visb));
}

void Luxko::Anuthur::D3D12Helper::RootSignatureDescriptor::PushRPSRV(UINT shaderRegister, UINT registerSpace /*= 0u*/, D3D12_SHADER_VISIBILITY visb /*= D3D12_SHADER_VISIBILITY_ALL*/)
{
	_rootParamters.push_back(RootParameter::SRV(shaderRegister, registerSpace, visb));
}

void Luxko::Anuthur::D3D12Helper::RootSignatureDescriptor::PushRPUAV(UINT shaderRegister, UINT registerSpace /*= 0u*/, D3D12_SHADER_VISIBILITY visb /*= D3D12_SHADER_VISIBILITY_ALL*/)
{
	_rootParamters.push_back(RootParameter::UAV(shaderRegister, registerSpace, visb));
}

void Luxko::Anuthur::D3D12Helper::RootSignatureDescriptor::PopStaticSamplerDescriptor()
{
	if (!_staticSamplerDescs.empty()) _staticSamplerDescs.pop_back();
}


void Luxko::Anuthur::D3D12Helper::RootSignatureDescriptor::PushStaticSamplerDescriptor(const StaticSamplerDescriptor& ssd)
{
	_staticSamplerDescs.push_back(ssd);
}

void Luxko::Anuthur::D3D12Helper::RootSignatureDescriptor::PushStaticSamplerDescriptor(UINT shaderRegister, D3D12_FILTER filter /*= D3D12_FILTER_ANISOTROPIC*/, D3D12_TEXTURE_ADDRESS_MODE addressU /*= D3D12_TEXTURE_ADDRESS_MODE_WRAP*/, D3D12_TEXTURE_ADDRESS_MODE addressV /*= D3D12_TEXTURE_ADDRESS_MODE_WRAP*/, D3D12_TEXTURE_ADDRESS_MODE addressW /*= D3D12_TEXTURE_ADDRESS_MODE_WRAP*/, FLOAT mipLODBias /*= 0*/, UINT maxAnisotropy /*= 16*/, D3D12_COMPARISON_FUNC comparisonFunc /*= D3D12_COMPARISON_FUNC_LESS_EQUAL*/, D3D12_STATIC_BORDER_COLOR borderColor /*= D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE*/, FLOAT minLOD /*= 0.f*/, FLOAT maxLOD /*= D3D12_FLOAT32_MAX*/, D3D12_SHADER_VISIBILITY shaderVisibility /*= D3D12_SHADER_VISIBILITY_ALL*/, UINT registerSpace /*= 0*/)
{
	_staticSamplerDescs.emplace_back(shaderRegister, filter, addressU, addressV, addressW, mipLODBias,
		maxAnisotropy, comparisonFunc, borderColor, minLOD, maxLOD, shaderVisibility, registerSpace);
}


D3D12_ROOT_SIGNATURE_DESC Luxko::Anuthur::D3D12Helper::RootSignatureDescriptor::Get(D3D12_ROOT_SIGNATURE_FLAGS flags /*= D3D12_ROOT_SIGNATURE_FLAG_NONE*/) const
{
	D3D12_ROOT_SIGNATURE_DESC desc;
	desc.pStaticSamplers = nullptr;
	desc.pParameters = nullptr;
	desc.Flags = flags;
	desc.NumParameters = static_cast<UINT>(_rootParamters.size());
	if (desc.NumParameters != 0u) desc.pParameters = _rootParamters.data();
	desc.NumStaticSamplers = static_cast<UINT>(_staticSamplerDescs.size());
	if (desc.NumStaticSamplers != 0u) desc.pStaticSamplers = _staticSamplerDescs.data();
	return desc;
}

HRESULT Luxko::Anuthur::D3D12Helper::RootSignatureDescriptor::SerializeRootSignature(ID3DBlob** ppSerializedRootSignature, D3D12_ROOT_SIGNATURE_FLAGS flags /*= D3D12_ROOT_SIGNATURE_FLAG_NONE*/, ID3DBlob** ppErrorBlob /*= nullptr*/) const
{
	return D3D12SerializeRootSignature(&Get(flags), D3D_ROOT_SIGNATURE_VERSION_1,
		ppSerializedRootSignature, ppErrorBlob);
}


Luxko::Anuthur::D3D12Helper::RasterizerDescriptor::RasterizerDescriptor(D3D12_FILL_MODE fillMode /*= D3D12_FILL_MODE_SOLID*/, D3D12_CULL_MODE cullMode /*= D3D12_CULL_MODE_BACK*/, BOOL frontCounterClockwise /*= FALSE*/, INT depthBias /*= 0*/, FLOAT depthBiaClamp /*= 0.f*/, FLOAT slopeScaledDepthBias /*= 0.f*/, BOOL depthClipEnable /*= TRUE*/, BOOL msEnable /*= FALSE*/, BOOL antiAliasEnable /*= FALSE*/, UINT forcedSampleCount /*= 0u*/, D3D12_CONSERVATIVE_RASTERIZATION_MODE crMode /*= D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF*/)
	:D3D12_RASTERIZER_DESC{ fillMode, cullMode, frontCounterClockwise, depthBias, depthBiaClamp, slopeScaledDepthBias, depthClipEnable, msEnable, antiAliasEnable, forcedSampleCount, crMode } {}

HRESULT Luxko::Anuthur::D3D12Helper::ShaderByteCode::CompileFromFile(LPCWSTR filename, LPCSTR entryPoint, LPCSTR target, const D3D_SHADER_MACRO* pDefines /*= nullptr*/, ID3DInclude* include /*= D3D_COMPILE_STANDARD_FILE_INCLUDE*/)
{
	_shaderBytes.Reset();
	return CompileShader(filename, entryPoint, target, _shaderBytes.GetAddressOf(), nullptr, pDefines, include);
}

D3D12_SHADER_BYTECODE Luxko::Anuthur::D3D12Helper::ShaderByteCode::Get() const
{
	D3D12_SHADER_BYTECODE result;
	result.BytecodeLength = _shaderBytes->GetBufferSize();
	result.pShaderBytecode = _shaderBytes->GetBufferPointer();
	return result;
}

Luxko::Anuthur::D3D12Helper::DescriptorHandleCPU& Luxko::Anuthur::D3D12Helper::DescriptorHandleCPU::Offset(SIZE_T offsetInBytes)
{
	ptr += offsetInBytes;
	return *this;
}

Luxko::Anuthur::D3D12Helper::SRVDescriptor Luxko::Anuthur::D3D12Helper::SRVDescriptor::BufferDesc(DXGI_FORMAT format, UINT64 firstElement, UINT byteStride, UINT numberELements, D3D12_BUFFER_SRV_FLAGS flags, UINT shader4ComponentMapping /*= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING*/)
{
	SRVDescriptor srv;
	srv.Format = format;
	srv.Shader4ComponentMapping = shader4ComponentMapping;
	srv.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srv.Buffer.FirstElement = firstElement;
	srv.Buffer.Flags = flags;
	srv.Buffer.NumElements = numberELements;
	srv.Buffer.StructureByteStride = byteStride;
	return srv;
}

Luxko::Anuthur::D3D12Helper::SRVDescriptor Luxko::Anuthur::D3D12Helper::SRVDescriptor::Texture1DDesc(DXGI_FORMAT format, UINT mostDetailedMip, UINT mipLevels /*= -1*/, FLOAT minLODClamp /*= 0.f*/, UINT shader4ComponentMapping /*= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING*/)
{
	SRVDescriptor srv;
	srv.Format = format;
	srv.Shader4ComponentMapping = shader4ComponentMapping;
	srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
	srv.Texture1D.MipLevels = mipLevels;
	srv.Texture1D.MostDetailedMip = mostDetailedMip;
	srv.Texture1D.ResourceMinLODClamp = minLODClamp;
	return srv;
}

Luxko::Anuthur::D3D12Helper::SRVDescriptor Luxko::Anuthur::D3D12Helper::SRVDescriptor::Texture1DArrayDesc(DXGI_FORMAT format, UINT mostDetailedMip, UINT firstArraySlice, UINT arraySize, UINT mipLevels /*= -1*/, FLOAT minLODClamp /*= 0.f*/, UINT shader4ComponentMapping /*= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING*/)
{
	SRVDescriptor srv;
	srv.Format = format;
	srv.Shader4ComponentMapping = shader4ComponentMapping;
	srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
	srv.Texture1DArray.ArraySize = arraySize;
	srv.Texture1DArray.FirstArraySlice = firstArraySlice;
	srv.Texture1DArray.MipLevels = mipLevels;
	srv.Texture1DArray.MostDetailedMip = mostDetailedMip;
	srv.Texture1DArray.ResourceMinLODClamp = minLODClamp;
	return srv;
}

Luxko::Anuthur::D3D12Helper::SRVDescriptor Luxko::Anuthur::D3D12Helper::SRVDescriptor::Texture2DDesc(DXGI_FORMAT format, UINT mostDetailedMip, UINT planeSlice, UINT mipLevels /*= -1*/, FLOAT minLODClamp /*= 0.f*/, UINT shader4ComponentMapping /*= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING*/)
{
	SRVDescriptor srv;
	srv.Format = format;
	srv.Shader4ComponentMapping = shader4ComponentMapping;
	srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srv.Texture2D.MipLevels = mipLevels;
	srv.Texture2D.MostDetailedMip = mostDetailedMip;
	srv.Texture2D.PlaneSlice = planeSlice;
	srv.Texture2D.ResourceMinLODClamp = minLODClamp;
	return srv;
}

Luxko::Anuthur::D3D12Helper::SRVDescriptor Luxko::Anuthur::D3D12Helper::SRVDescriptor::Texture2DArrayDesc(DXGI_FORMAT format, UINT mostDetailedMip, UINT planeSlice, UINT firstArraySlice, UINT arraySize, UINT mipLevels /*= -1*/, FLOAT minLODClamp /*= 0.f*/, UINT shader4ComponentMapping /*= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING*/)
{
	SRVDescriptor srv;
	srv.Format = format;
	srv.Shader4ComponentMapping = shader4ComponentMapping;
	srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
	srv.Texture2DArray.ArraySize = arraySize;
	srv.Texture2DArray.FirstArraySlice = firstArraySlice;
	srv.Texture2DArray.MipLevels = mipLevels;
	srv.Texture2DArray.MostDetailedMip = mostDetailedMip;
	srv.Texture2DArray.PlaneSlice = planeSlice;
	srv.Texture2DArray.ResourceMinLODClamp = minLODClamp;
	return srv;
}

Luxko::Anuthur::D3D12Helper::SRVDescriptor Luxko::Anuthur::D3D12Helper::SRVDescriptor::Texture2DMSDesc(DXGI_FORMAT format, UINT shader4ComponentMapping /*= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING*/)
{
	SRVDescriptor srv;
	srv.Format = format;
	srv.Shader4ComponentMapping = shader4ComponentMapping;
	srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
	return srv;
}

Luxko::Anuthur::D3D12Helper::SRVDescriptor Luxko::Anuthur::D3D12Helper::SRVDescriptor::Texture2DMSArrayDesc(DXGI_FORMAT format, UINT firstArraySlice, UINT arraySize, UINT shader4ComponentMapping /*= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING*/)
{
	SRVDescriptor srv;
	srv.Format = format;
	srv.Shader4ComponentMapping = shader4ComponentMapping;
	srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;
	srv.Texture2DMSArray.ArraySize = arraySize;
	srv.Texture2DMSArray.FirstArraySlice = firstArraySlice;
	return srv;
}

Luxko::Anuthur::D3D12Helper::SRVDescriptor Luxko::Anuthur::D3D12Helper::SRVDescriptor::Texture3DDesc(DXGI_FORMAT format, UINT mostDetailedMip, UINT mipLevels /*= -1*/, FLOAT minLODClamp /*= 0.f*/, UINT shader4ComponentMapping /*= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING*/)
{
	SRVDescriptor srv;
	srv.Format = format;
	srv.Shader4ComponentMapping = shader4ComponentMapping;
	srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
	srv.Texture3D.MipLevels = mipLevels;
	srv.Texture3D.MostDetailedMip = mostDetailedMip;
	srv.Texture3D.ResourceMinLODClamp = minLODClamp;
	return srv;
}

Luxko::Anuthur::D3D12Helper::SRVDescriptor Luxko::Anuthur::D3D12Helper::SRVDescriptor::TextureCubeDesc(DXGI_FORMAT format, UINT mostDetailedMip, UINT mipLevels /*= -1*/, FLOAT minLODClamp /*= 0.f*/, UINT shader4ComponentMapping /*= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING*/)
{
	SRVDescriptor srv;
	srv.Format = format;
	srv.Shader4ComponentMapping = shader4ComponentMapping;
	srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	srv.TextureCube.MipLevels = mipLevels;
	srv.TextureCube.MostDetailedMip = mostDetailedMip;
	srv.TextureCube.ResourceMinLODClamp = minLODClamp;
	return srv;
}

Luxko::Anuthur::D3D12Helper::SRVDescriptor Luxko::Anuthur::D3D12Helper::SRVDescriptor::TextureCubeArrayDesc(DXGI_FORMAT format, UINT mostDetailedMip, UINT firstFace, UINT numCubes, UINT mipLevels /*= -1*/, FLOAT minLODClamp /*= 0.f*/, UINT shader4ComponentMapping /*= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING*/)
{
	SRVDescriptor srv;
	srv.Format = format;
	srv.Shader4ComponentMapping = shader4ComponentMapping;
	srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
	srv.TextureCubeArray.First2DArrayFace = firstFace;
	srv.TextureCubeArray.MipLevels = mipLevels;
	srv.TextureCubeArray.MostDetailedMip = mostDetailedMip;
	srv.TextureCubeArray.NumCubes = numCubes;
	srv.TextureCubeArray.ResourceMinLODClamp = minLODClamp;
	return srv;
}

Luxko::Anuthur::D3D12Helper::BlendDescriptor Luxko::Anuthur::D3D12Helper::BlendDescriptor::Default()
{
	BlendDescriptor bd;
	bd.BlendEnable = FALSE;
	bd.LogicOpEnable = FALSE;
	return bd;
}

Luxko::Anuthur::D3D12Helper::BlendDescriptor Luxko::Anuthur::D3D12Helper::BlendDescriptor::Blend(D3D12_BLEND srcBlend, D3D12_BLEND destBlend, D3D12_BLEND_OP blendOp, D3D12_BLEND srcAlphaBlend, D3D12_BLEND destAlphaBlend, D3D12_BLEND_OP alphaBlendOp, UINT8 renderTargetWriteMask /*= D3D12_COLOR_WRITE_ENABLE_ALL*/)
{
	BlendDescriptor bd;
	bd.BlendEnable = TRUE;
	bd.LogicOpEnable = FALSE;
	bd.SrcBlend = srcBlend;
	bd.DestBlend = destBlend;
	bd.BlendOp = blendOp;
	bd.SrcBlendAlpha = srcAlphaBlend;
	bd.DestBlendAlpha = destAlphaBlend;
	bd.BlendOpAlpha = alphaBlendOp;
	bd.RenderTargetWriteMask = renderTargetWriteMask;
	return bd;
}

Luxko::Anuthur::D3D12Helper::BlendDescriptor Luxko::Anuthur::D3D12Helper::BlendDescriptor::Logic(D3D12_LOGIC_OP logicOp, UINT8 renderTargetWriteMask /*= D3D12_COLOR_WRITE_ENABLE_ALL*/)
{
	BlendDescriptor bd;
	bd.BlendEnable = FALSE;
	bd.LogicOpEnable = TRUE;
	bd.LogicOp = logicOp;
	return bd;
}

Luxko::Anuthur::D3D12Helper::DepthStencilDescriptor::DepthStencilDescriptor(BOOL depthEnable, D3D12_DEPTH_WRITE_MASK dwm, D3D12_COMPARISON_FUNC dcf, BOOL stencilEnable, UINT8 stencilReadMask, UINT8 stencilWriteMask, D3D12_DEPTH_STENCILOP_DESC frontFace, D3D12_DEPTH_STENCILOP_DESC backFace)
	:D3D12_DEPTH_STENCIL_DESC{depthEnable, dwm, dcf, stencilEnable, stencilReadMask, stencilWriteMask, frontFace, backFace} {}

Luxko::Anuthur::D3D12Helper::DepthStencilDescriptor Luxko::Anuthur::D3D12Helper::DepthStencilDescriptor::Default()
{
	DepthStencilDescriptor dsd;
	dsd.DepthEnable = TRUE;
	dsd.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	dsd.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

	dsd.StencilEnable = FALSE;
	dsd.StencilReadMask = 0xff;
	dsd.StencilWriteMask = 0xff;
	return dsd;
}

Luxko::Anuthur::D3D12Helper::StencilOperationDescriptor::StencilOperationDescriptor(D3D12_STENCIL_OP stencilFailOp, D3D12_STENCIL_OP stencilPassDepthFailOp, D3D12_STENCIL_OP stencilPassOp, D3D12_COMPARISON_FUNC stencilFunction)
	:D3D12_DEPTH_STENCILOP_DESC{stencilFailOp, stencilPassDepthFailOp, stencilPassOp, stencilFunction} {}
