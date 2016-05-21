//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#include "HelperMethods.h"

Luxko::Anuthur::D3D12Helper::InputElementDescriptor::InputElementDescriptor(LPCSTR semanticName, DXGI_FORMAT format, UINT bytedOffsetAligned, UINT inputSlot /*= 0u*/, UINT semanticIndex/* = 0u*/, UINT instanceDataSR /*= 0u*/, D3D12_INPUT_CLASSIFICATION classification /*= D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA*/)
	:D3D12_INPUT_ELEMENT_DESC{ semanticName, semanticIndex, format, inputSlot, bytedOffsetAligned,
		classification, instanceDataSR } {}

void Luxko::Anuthur::D3D12Helper::InputLayoutDescriptor::PushElementDescription(LPCSTR semanticName, DXGI_FORMAT format, UINT inputSlot /*= 0u*/, UINT semanticIndex /*= 0u*/, UINT instanceDataSR /*= 0u*/, D3D12_INPUT_CLASSIFICATION classification /*= D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA*/)
{
	assert(inputSlot < MaxInputSlotCount);
	_ilds.emplace_back(semanticName, format, _currentSlotOffset[inputSlot],
		inputSlot, semanticIndex, instanceDataSR, classification);
	auto size = DxgiFormatSize(format);
	size /= 8u;
	while (size % 4 != 0u) size++;
	_currentSlotOffset[inputSlot] += size;
}

D3D12_INPUT_LAYOUT_DESC Luxko::Anuthur::D3D12Helper::InputLayoutDescriptor::Get() const
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

UINT Luxko::Anuthur::D3D12Helper::DxgiFormatSize(DXGI_FORMAT format)
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

	Microsoft::WRL::ComPtr<ID3D12Resource> Luxko::Anuthur::D3D12Helper::CreateDefaultBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdlst, const void* data, UINT64 sizeInBytes, Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer)
	{
		uploadBuffer.Reset();
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

	Luxko::Anuthur::D3D12Helper::DescriptorHeapDescriptor::DescriptorHeapDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT count /*= 1u*/, D3D12_DESCRIPTOR_HEAP_FLAGS flags /*= D3D12_DESCRIPTOR_HEAP_FLAG_NONE*/, UINT nodeMask /*= 0u*/)
		:D3D12_DESCRIPTOR_HEAP_DESC{ type, count, flags, nodeMask } { }

	Luxko::Anuthur::D3D12Helper::CBVDescriptor::CBVDescriptor(D3D12_GPU_VIRTUAL_ADDRESS location, UINT sizeInBytes)
		: D3D12_CONSTANT_BUFFER_VIEW_DESC{ location, sizeInBytes } {}

	Luxko::Anuthur::D3D12Helper::CBVDescriptor::CBVDescriptor(ID3D12Resource* buffer, UINT64 offsetFromBufferStart, UINT sizeInBytes)
	{
		auto start = buffer->GetGPUVirtualAddress();
		BufferLocation = start + offsetFromBufferStart;
		sizeInBytes = sizeInBytes;
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
		result.NumDescriptorRanges = _dRanges.size();
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
		desc.NumParameters = _rootParamters.size();
		if (desc.NumParameters != 0u) desc.pParameters = _rootParamters.data();
		desc.NumStaticSamplers = _staticSamplerDescs.size();
		if (desc.NumStaticSamplers != 0u) desc.pStaticSamplers = _staticSamplerDescs.data();
		return desc;
	}

	HRESULT Luxko::Anuthur::D3D12Helper::RootSignatureDescriptor::SerializeRootSignature(ID3DBlob** ppSerializedRootSignature, D3D12_ROOT_SIGNATURE_FLAGS flags /*= D3D12_ROOT_SIGNATURE_FLAG_NONE*/, ID3DBlob** ppErrorBlob /*= nullptr*/) const
	{
		return D3D12SerializeRootSignature(&Get(flags), D3D_ROOT_SIGNATURE_VERSION_1,
			ppSerializedRootSignature, ppErrorBlob);
	}


	Luxko::Anuthur::D3D12Helper::RasterizerDescriptor::RasterizerDescriptor(D3D12_FILL_MODE fillMode /*= D3D12_FILL_MODE_SOLID*/, D3D12_CULL_MODE cullMode /*= D3D12_CULL_MODE_BACK*/, BOOL frontCounterClockwise /*= FALSE*/, INT depthBias /*= 0*/, FLOAT depthBiaClamp /*= 0.f*/, FLOAT slopeScaledDepthBias /*= 0.f*/, BOOL depthClipEnable /*= TRUE*/, BOOL scissorEnable /*= FALSE*/, BOOL msEnable /*= FALSE*/, BOOL antiAliasEnable /*= FALSE*/, UINT forcedSampleCount /*= 0u*/, D3D12_CONSERVATIVE_RASTERIZATION_MODE crMode /*= D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF*/)
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
		result.pShaderBytecode = reinterpret_cast<BYTE*>(_shaderBytes->GetBufferPointer());
		return result;
	}
