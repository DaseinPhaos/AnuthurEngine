//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
#include "WICReadback.h"

IWICImagingFactory* Luxko::Anuthur::WIC::Get()
{
	static WIC wic;
	return wic._wicFactory.Get();
}

GUID Luxko::Anuthur::dxgiFormat2PixelFormat(DXGI_FORMAT format)
{
	switch (format)
	{
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
		return GUID_WICPixelFormat128bppRGBAFloat;
	case DXGI_FORMAT_R32G32B32_FLOAT:
		return GUID_WICPixelFormat96bppRGBFloat;
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
		return GUID_WICPixelFormat64bppRGBAHalf;
	case DXGI_FORMAT_R16G16B16A16_UNORM:
		return GUID_WICPixelFormat64bppRGBA;
	case DXGI_FORMAT_R10G10B10A2_UNORM:
		return GUID_WICPixelFormat32bppRGBA1010102;
	case DXGI_FORMAT_R8G8B8A8_UNORM:
		return GUID_WICPixelFormat32bppRGBA;
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		return GUID_WICPixelFormat32bppRGBA;
	case DXGI_FORMAT_D32_FLOAT:
	case DXGI_FORMAT_R32_FLOAT:
		return GUID_WICPixelFormat32bppGrayFloat;
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
		return GUID_WICPixelFormat32bppRGB;
	case DXGI_FORMAT_R16_FLOAT:
		return GUID_WICPixelFormat16bppGrayHalf;
	case DXGI_FORMAT_D16_UNORM:
	case DXGI_FORMAT_R16_UNORM:
		return GUID_WICPixelFormat16bppGray;
	case DXGI_FORMAT_R8_UNORM:
		return GUID_WICPixelFormat8bppGray;
	case DXGI_FORMAT_A8_UNORM:
		return GUID_WICPixelFormat8bppAlpha;
	case DXGI_FORMAT_R8G8_B8G8_UNORM:
		return GUID_WICPixelFormat32bppRGBA;
	case DXGI_FORMAT_B5G6R5_UNORM:
		return GUID_WICPixelFormat16bppBGR565;
	case DXGI_FORMAT_B5G5R5A1_UNORM:
		return GUID_WICPixelFormat16bppBGRA5551;
	case DXGI_FORMAT_B8G8R8A8_UNORM:
		return GUID_WICPixelFormat32bppBGRA;
	case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
		return GUID_WICPixelFormat32bppRGBA1010102XR;
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
		return GUID_WICPixelFormat32bppRGBA;
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
		return GUID_WICPixelFormat32bppBGRA;
	default:
		return GUID_WICPixelFormatUndefined;
	}
}

void Luxko::Anuthur::SaveTexture2DAsPNG(ID3D12Resource* readBackHeap, 
	unsigned int width, unsigned int height,const wchar_t* filename)
{
	// D3D12_RESOURCE_DESC desc = readBackHeap->GetDesc();
	// assert(desc.Format == DXGI_FORMAT_R32G32B32A32_FLOAT);

	BYTE* dataPtr;
	D3D12_RANGE dr = {};
	dr.Begin = 0u;
	dr.End = static_cast<SIZE_T>(width * height * sizeof(float) * 4);
	ThrowIfFailed(readBackHeap->Map(0u, &dr, reinterpret_cast<void**>(&dataPtr)));

	IWICImagingFactory* wicif = WIC::Get();
	ComPtr<IWICBitmap> srcBitmap;
	ThrowIfFailed(wicif->CreateBitmapFromMemory(
		static_cast<UINT>(width),
		static_cast<UINT>(height),
		GUID_WICPixelFormat128bppRGBAFloat,
		static_cast<UINT>(width * 4 * sizeof(float)),
		static_cast<UINT>(dr.End),
		dataPtr, srcBitmap.GetAddressOf()));

	
	ComPtr<IWICStream> fs;
	ThrowIfFailed(wicif->CreateStream(fs.GetAddressOf()));
	ThrowIfFailed(fs->InitializeFromFilename(filename, GENERIC_WRITE));

	ComPtr<IWICBitmapEncoder> pngEncoder;
	ThrowIfFailed(wicif->CreateEncoder(GUID_ContainerFormatPng, nullptr,
		pngEncoder.GetAddressOf()));
	ThrowIfFailed(pngEncoder->Initialize(fs.Get(), WICBitmapEncoderNoCache));

	ComPtr<IWICBitmapFrameEncode> frameEncode;
	ThrowIfFailed(pngEncoder->CreateNewFrame(frameEncode.GetAddressOf(), nullptr));
	ThrowIfFailed(frameEncode->Initialize(nullptr));

	auto pf = GUID_WICPixelFormat32bppBGRA;
	ThrowIfFailed(frameEncode->SetPixelFormat(&pf));
	ThrowIfFailed(frameEncode->SetSize(static_cast<UINT>(width),
		static_cast<UINT>(height)));
	WICRect rect = {};
	rect.X = 0;
	rect.Y = 0;
	rect.Width = static_cast<INT>(width);
	rect.Height = static_cast<INT>(height);
	ThrowIfFailed(frameEncode->WriteSource(srcBitmap.Get(), nullptr));

	ThrowIfFailed(frameEncode->Commit());
	ThrowIfFailed(pngEncoder->Commit());
}

static GUID pngFormatEncode(const GUID& decodeFrom) {
	if (decodeFrom == GUID_WICPixelFormat128bppRGBAFloat
		|| decodeFrom == GUID_WICPixelFormat96bppRGBFloat
		|| decodeFrom == GUID_WICPixelFormat64bppRGBAHalf
		|| decodeFrom == GUID_WICPixelFormat64bppRGBA
		|| decodeFrom == GUID_WICPixelFormat32bppRGBA1010102
		|| decodeFrom == GUID_WICPixelFormat32bppRGBA
		|| decodeFrom == GUID_WICPixelFormat32bppRGB
		|| decodeFrom == GUID_WICPixelFormat16bppBGR565
		|| decodeFrom == GUID_WICPixelFormat16bppBGRA5551
		|| decodeFrom == GUID_WICPixelFormat32bppBGRA
		|| decodeFrom == GUID_WICPixelFormat32bppRGBA1010102XR) {
		return GUID_WICPixelFormat32bppBGRA;
	}
	else return GUID_WICPixelFormat16bppGray;
}

void Luxko::Anuthur::SaveTexture2DAsPNG(ID3D12Resource* readBackHeap,
	const wchar_t* filename,
	unsigned int subResource, unsigned int width,
	unsigned int height, DXGI_FORMAT format/* = DXGI_FORMAT_UNKNOWN*/)
{
	if (format == DXGI_FORMAT_UNKNOWN) {
		D3D12_RESOURCE_DESC desc = readBackHeap->GetDesc();
		assert(desc.Format != DXGI_FORMAT_UNKNOWN);
		assert(desc.Width == width);
		assert(desc.Height == height);
		assert(desc.MipLevels > subResource);
		format = desc.Format;
	}
	
	auto formatGuid = dxgiFormat2PixelFormat(format);
	assert(formatGuid != GUID_WICPixelFormatUndefined);
	auto formatByteSize = D3D12Helper::DxgiFormatBitSize(format) / 8u;

	BYTE* dataPtr;
	D3D12_RANGE dr = {};
	dr.Begin = 0u;
	dr.End = static_cast<SIZE_T>(width * height * formatByteSize);
	ThrowIfFailed(readBackHeap->Map(subResource, &dr, 
		reinterpret_cast<void**>(&dataPtr)));

	IWICImagingFactory* wicif = WIC::Get();
	ComPtr<IWICBitmap> srcBitmap;
	ThrowIfFailed(wicif->CreateBitmapFromMemory(
		static_cast<UINT>(width),
		static_cast<UINT>(height),
		formatGuid,
		static_cast<UINT>(formatByteSize * width),
		static_cast<UINT>(dr.End),
		dataPtr, srcBitmap.GetAddressOf()));


	ComPtr<IWICStream> fs;
	ThrowIfFailed(wicif->CreateStream(fs.GetAddressOf()));
	ThrowIfFailed(fs->InitializeFromFilename(filename, GENERIC_WRITE));

	ComPtr<IWICBitmapEncoder> pngEncoder;
	ThrowIfFailed(wicif->CreateEncoder(GUID_ContainerFormatPng, nullptr,
		pngEncoder.GetAddressOf()));
	ThrowIfFailed(pngEncoder->Initialize(fs.Get(), WICBitmapEncoderNoCache));

	ComPtr<IWICBitmapFrameEncode> frameEncode;
	ThrowIfFailed(pngEncoder->CreateNewFrame(frameEncode.GetAddressOf(), nullptr));
	ThrowIfFailed(frameEncode->Initialize(nullptr));

	auto pf = pngFormatEncode(formatGuid);
	ThrowIfFailed(frameEncode->SetPixelFormat(&pf));
	ThrowIfFailed(frameEncode->SetSize(static_cast<UINT>(width),
		static_cast<UINT>(height)));
	WICRect rect = {};
	rect.X = 0;
	rect.Y = 0;
	rect.Width = static_cast<INT>(width);
	rect.Height = static_cast<INT>(height);
	ThrowIfFailed(frameEncode->WriteSource(srcBitmap.Get(), nullptr));

	ThrowIfFailed(frameEncode->Commit());
	ThrowIfFailed(pngEncoder->Commit());
}