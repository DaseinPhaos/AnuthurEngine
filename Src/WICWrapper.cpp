//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
#include "WICWrapper.h"
#include "D3D12HelperMethods.h"

IWICImagingFactory* Luxko::Anuthur::WIC::Get()
{
	static WIC wic;
	return wic._wicFactory.Get();
}

namespace {
	inline DXGI_FORMAT MakeSRGB(_In_ DXGI_FORMAT format)
	{
		switch (format)
		{
		case DXGI_FORMAT_R8G8B8A8_UNORM:
			return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

		case DXGI_FORMAT_BC1_UNORM:
			return DXGI_FORMAT_BC1_UNORM_SRGB;

		case DXGI_FORMAT_BC2_UNORM:
			return DXGI_FORMAT_BC2_UNORM_SRGB;

		case DXGI_FORMAT_BC3_UNORM:
			return DXGI_FORMAT_BC3_UNORM_SRGB;

		case DXGI_FORMAT_B8G8R8A8_UNORM:
			return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;

		case DXGI_FORMAT_B8G8R8X8_UNORM:
			return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;

		case DXGI_FORMAT_BC7_UNORM:
			return DXGI_FORMAT_BC7_UNORM_SRGB;

		default:
			return format;
		}
	}

	uint32_t CountMips(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0)
			return 0;

		uint32_t count = 1;
		while (width > 1 || height > 1)
		{
			width >>= 1;
			height >>= 1;
			count++;
		}
		return count;
	}

	HRESULT CreateTextureFromWIC(
		ID3D12Device* d3dDevice,
		ID3D12GraphicsCommandList* pCmdList,
		IWICBitmapFrameDecode* frame,
		size_t maxsize,
		D3D12_RESOURCE_FLAGS flags,
		bool forceSRGB,
		Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer,
		Microsoft::WRL::ComPtr<ID3D12Resource>& texture,
		std::unique_ptr<uint8_t[]>& decodedData,
		D3D12_SUBRESOURCE_DATA& subresource)
	{
		assert(d3dDevice&&pCmdList);

		UINT width, height;
		HRESULT hr = frame->GetSize(&width, &height);
		if (FAILED(hr))
			return hr;

		assert(width > 0 && height > 0);

		if (!maxsize)
		{
			maxsize = D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION;
		}

		assert(maxsize > 0);

		UINT twidth, theight;
		if (width > maxsize || height > maxsize)
		{
			float ar = static_cast<float>(height) / static_cast<float>(width);
			if (width > height)
			{
				twidth = static_cast<UINT>(maxsize);
				theight = static_cast<UINT>(static_cast<float>(maxsize) * ar);
			}
			else
			{
				theight = static_cast<UINT>(maxsize);
				twidth = static_cast<UINT>(static_cast<float>(maxsize) / ar);
			}
			assert(twidth <= maxsize && theight <= maxsize);
		}
		else
		{
			twidth = width;
			theight = height;
		}

		// Determine format
		GUID pixelFormat;
		hr = frame->GetPixelFormat(&pixelFormat);
		if (FAILED(hr))
			return hr;

		GUID convertGUID;
		memcpy(&convertGUID, &pixelFormat, sizeof(WICPixelFormatGUID));

		auto format = Luxko::Anuthur::WIC::pixelFormat2DxgiFormat(pixelFormat);


		if (format == DXGI_FORMAT_UNKNOWN)
			return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);



		size_t bpp = Luxko::Anuthur::D3D12Helper::DxgiFormatBitSize(format);


		if (!bpp)
			return E_FAIL;

		// Handle sRGB formats
		if (forceSRGB)
		{
			format = MakeSRGB(format);
		}
		else
		{
			Microsoft::WRL::ComPtr<IWICMetadataQueryReader> metareader;
			if (SUCCEEDED(frame->GetMetadataQueryReader(metareader.GetAddressOf())))
			{
				GUID containerFormat;
				if (SUCCEEDED(metareader->GetContainerFormat(&containerFormat)))
				{
					// Check for sRGB colorspace metadata
					bool sRGB = false;

					PROPVARIANT value;
					PropVariantInit(&value);

					if (memcmp(&containerFormat, &GUID_ContainerFormatPng, sizeof(GUID)) == 0)
					{
						// Check for sRGB chunk
						if (SUCCEEDED(metareader->GetMetadataByName(L"/sRGB/RenderingIntent", &value)) && value.vt == VT_UI1)
						{
							sRGB = true;
						}
					}
					else if (SUCCEEDED(metareader->GetMetadataByName(L"System.Image.ColorSpace", &value)) && value.vt == VT_UI2 && value.uiVal == 1)
					{
						sRGB = true;
					}

					PropVariantClear(&value);

					if (sRGB)
						format = MakeSRGB(format);
				}
			}
		}

		// Allocate memory for decoded image
		size_t rowPitch = (twidth * bpp + 7) / 8;
		size_t imageSize = rowPitch * theight;

		decodedData.reset(new (std::nothrow) uint8_t[imageSize]);
		if (!decodedData)
			return E_OUTOFMEMORY;

		// Load image data
		if (memcmp(&convertGUID, &pixelFormat, sizeof(GUID)) == 0
			&& twidth == width
			&& theight == height)
		{
			// No format conversion or resize needed
			hr = frame->CopyPixels(0,
				static_cast<UINT>(rowPitch),
				static_cast<UINT>(imageSize),
				decodedData.get());
			if (FAILED(hr))
				return hr;
		}
		else if (twidth != width || theight != height)
		{
			// Resize
			auto pWIC = Luxko::Anuthur::WIC::Get();
			if (!pWIC)
				return E_NOINTERFACE;

			Microsoft::WRL::ComPtr<IWICBitmapScaler> scaler;
			hr = pWIC->CreateBitmapScaler(scaler.GetAddressOf());
			if (FAILED(hr))
				return hr;

			hr = scaler->Initialize(
				frame, twidth, theight, WICBitmapInterpolationModeFant);
			if (FAILED(hr))
				return hr;

			WICPixelFormatGUID pfScaler;
			hr = scaler->GetPixelFormat(&pfScaler);
			if (FAILED(hr))
				return hr;

			if (memcmp(&convertGUID, &pfScaler, sizeof(GUID)) == 0)
			{
				// No format conversion needed
				hr = scaler->CopyPixels(0, static_cast<UINT>(rowPitch),
					static_cast<UINT>(imageSize), decodedData.get());
				if (FAILED(hr))
					return hr;
			}
			else
			{
				Microsoft::WRL::ComPtr<IWICFormatConverter> FC;
				hr = pWIC->CreateFormatConverter(FC.GetAddressOf());
				if (FAILED(hr))
					return hr;

				BOOL canConvert = FALSE;
				hr = FC->CanConvert(pfScaler, convertGUID, &canConvert);
				if (FAILED(hr) || !canConvert)
				{
					return E_UNEXPECTED;
				}

				hr = FC->Initialize(scaler.Get(), convertGUID,
					WICBitmapDitherTypeErrorDiffusion, 0, 0,
					WICBitmapPaletteTypeCustom);
				if (FAILED(hr))
					return hr;

				hr = FC->CopyPixels(0, static_cast<UINT>(rowPitch),
					static_cast<UINT>(imageSize), decodedData.get());
				if (FAILED(hr))
					return hr;
			}
		}
		else
		{
			// Format conversion but no resize
			auto pWIC = Luxko::Anuthur::WIC::Get();
			if (!pWIC)
				return E_NOINTERFACE;

			Microsoft::WRL::ComPtr<IWICFormatConverter> FC;
			hr = pWIC->CreateFormatConverter(FC.GetAddressOf());
			if (FAILED(hr))
				return hr;

			BOOL canConvert = FALSE;
			hr = FC->CanConvert(pixelFormat, convertGUID, &canConvert);
			if (FAILED(hr) || !canConvert)
			{
				return E_UNEXPECTED;
			}

			hr = FC->Initialize(frame, convertGUID,
				WICBitmapDitherTypeErrorDiffusion,
				0, 0, WICBitmapPaletteTypeCustom);
			if (FAILED(hr))
				return hr;

			hr = FC->CopyPixels(0,
				static_cast<UINT>(rowPitch),
				static_cast<UINT>(imageSize),
				decodedData.get());
			if (FAILED(hr))
				return hr;
		}

		// Count the number of mips
		//uint32_t mipCount = (reserveFullMipChain) ? CountMips(twidth, theight) : 1;

		auto uploadHP = Luxko::Anuthur::D3D12Helper::ResourceHeapProperties(
			D3D12_HEAP_TYPE_UPLOAD);

		auto uploadRD = Luxko::Anuthur::D3D12Helper::ResourceDescriptor::Buffer(
			static_cast<UINT64>(imageSize),
			D3D12_RESOURCE_FLAG_NONE);

		hr = d3dDevice->CreateCommittedResource(
			&uploadHP,
			D3D12_HEAP_FLAG_NONE,
			&uploadRD,
			D3D12_RESOURCE_STATE_COPY_SOURCE,
			nullptr,
			IID_PPV_ARGS(uploadBuffer.GetAddressOf()));

		if (FAILED(hr))
		{
			return hr;
		}

		D3D12_RESOURCE_DESC desc = {};
		desc.Width = twidth;
		desc.Height = theight;
		desc.MipLevels = 1;
		desc.DepthOrArraySize = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Flags = flags;
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

		hr = d3dDevice->CreateCommittedResource(
			&Luxko::Anuthur::D3D12Helper::ResourceHeapProperties(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(texture.GetAddressOf())
		);

		if (FAILED(hr))
		{
			return hr;
		}

		subresource.pData = decodedData.get();
		subresource.RowPitch = rowPitch;
		subresource.SlicePitch = imageSize;

		D3D12_RANGE readRange;
		readRange.Begin = 0u;
		readRange.End = imageSize;

		uint8_t* ubPtr = nullptr;
		hr = uploadBuffer->Map(0u, &readRange, reinterpret_cast<void**>(&ubPtr));
		if (FAILED(hr))
		{
			return hr;
		}

		memcpy(ubPtr, decodedData.get(), imageSize);

		uploadBuffer->Unmap(0u, &readRange);

		using TextureCopyLocation = Luxko::Anuthur::D3D12Helper::TextureCopyLocation;
		auto destLoc = TextureCopyLocation::Subresource(texture.Get(), 0u);
		auto srcLoc = TextureCopyLocation::PlacedFootPrintF(texture.Get(),
			rowPitch, twidth, theight, 0u, format, 0u);

		pCmdList->CopyTextureRegion(&destLoc, 0u, 0u, 0u, &srcLoc, nullptr);
		
		D3D12_RESOURCE_BARRIER rb;
		rb.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		rb.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		rb.Transition.pResource = texture.Get();
		rb.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		rb.Transition.StateAfter = D3D12_RESOURCE_STATE_COMMON;
		rb.Transition.Subresource = 0u;

		pCmdList->ResourceBarrier(1u, &rb);
	}
}

Microsoft::WRL::ComPtr<ID3D12Resource> Luxko::Anuthur::WIC::LoadWICTextureFromMemory(
	ID3D12Device* d3dDevice,
	ID3D12GraphicsCommandList* pCmdList,
	const uint8_t* wicData,
	size_t wicDataSize,
	ComPtr<ID3D12Resource>& uploadBuffer,
	std::unique_ptr<uint8_t[]>& decodedData,
	D3D12_SUBRESOURCE_DATA& subresource,
	size_t maxsize /*= 0*/)
{
	return LoadWICTextureFromMemoryEx(
		d3dDevice,
		pCmdList,
		wicData,
		wicDataSize,
		maxsize,
		D3D12_RESOURCE_FLAG_NONE,
		false,
		uploadBuffer,
		decodedData,
		subresource);
}

Microsoft::WRL::ComPtr<ID3D12Resource> Luxko::Anuthur::WIC::LoadWICTextureFromFile(
	ID3D12Device* d3dDevice,
	ID3D12GraphicsCommandList* pCmdList,
	const wchar_t* szFileName,
	ComPtr<ID3D12Resource>& uploadBuffer,
	std::unique_ptr<uint8_t[]>& decodedData,
	D3D12_SUBRESOURCE_DATA& subresource,
	size_t maxsize /*= 0*/)
{
	return LoadWICTextureFromFileEx(
		d3dDevice,
		pCmdList,
		szFileName,
		maxsize,
		D3D12_RESOURCE_FLAG_NONE,
		false,
		uploadBuffer,
		decodedData,
		subresource);
}

Microsoft::WRL::ComPtr<ID3D12Resource> Luxko::Anuthur::WIC::LoadWICTextureFromMemoryEx(
	ID3D12Device* d3dDevice,
	ID3D12GraphicsCommandList* pCmdList,
	const uint8_t* wicData,
	size_t wicDataSize,
	size_t maxsize,
	D3D12_RESOURCE_FLAGS flags,
	bool forceSRGB,
	ComPtr<ID3D12Resource>& uploadBuffer,
	std::unique_ptr<uint8_t[]>& decodedData,
	D3D12_SUBRESOURCE_DATA& subresource)
{
	if (!d3dDevice || !wicData || !wicDataSize)
		ThrowIfFailed(E_INVALIDARG);

	if (wicDataSize > UINT32_MAX)
		ThrowIfFailed(HRESULT_FROM_WIN32(ERROR_FILE_TOO_LARGE));

	uploadBuffer.Reset();

	auto pWIC = Get();
	assert(pWIC);

	// Initialize input stream from memory
	ComPtr<IWICStream> memStream;
	ThrowIfFailed(pWIC->CreateStream(memStream.GetAddressOf()));
	ThrowIfFailed(memStream->InitializeFromMemory(const_cast<uint8_t*>(wicData),
		static_cast<DWORD>(wicDataSize)));


	ComPtr<IWICBitmapDecoder> wicDecoder;
	ThrowIfFailed(pWIC->CreateDecoderFromStream(memStream.Get(), 0,
		WICDecodeMetadataCacheOnDemand, wicDecoder.GetAddressOf()));
	ComPtr<IWICBitmapFrameDecode> firstFrame;
	ThrowIfFailed(wicDecoder->GetFrame(0, firstFrame.GetAddressOf()));

	ComPtr<ID3D12Resource> texture;
	ThrowIfFailed(CreateTextureFromWIC(d3dDevice, pCmdList,
		firstFrame.Get(),
		maxsize, flags, forceSRGB,
		uploadBuffer, texture,
		decodedData, subresource));
	return texture;
}

Microsoft::WRL::ComPtr<ID3D12Resource> Luxko::Anuthur::WIC::LoadWICTextureFromFileEx(
	ID3D12Device* d3dDevice,
	ID3D12GraphicsCommandList* pCmdList,
	const wchar_t* szFileName,
	size_t maxsize,
	D3D12_RESOURCE_FLAGS flags,
	bool forceSRGB,
	ComPtr<ID3D12Resource>& uploadBuffer,
	std::unique_ptr<uint8_t[]>& decodedData,
	D3D12_SUBRESOURCE_DATA& subresource)
{
	if (!d3dDevice || !szFileName)
		ThrowIfFailed(E_INVALIDARG);
	uploadBuffer.Reset();

	auto pWIC = Get();
	ComPtr<IWICBitmapDecoder> decoder;
	ThrowIfFailed(pWIC->CreateDecoderFromFilename(
		szFileName, 0, GENERIC_READ, WICDecodeMetadataCacheOnDemand,
		decoder.GetAddressOf()));
	ComPtr<IWICBitmapFrameDecode> frame;
	ThrowIfFailed(decoder->GetFrame(0, frame.GetAddressOf()));


	ComPtr<ID3D12Resource> texture;
	ThrowIfFailed(CreateTextureFromWIC(d3dDevice, pCmdList,
		frame.Get(),
		maxsize, flags, forceSRGB,
		uploadBuffer, texture,
		decodedData, subresource));
	return texture;
}

GUID Luxko::Anuthur::WIC::dxgiFormat2PixelFormat(DXGI_FORMAT format)
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
		return GUID_WICPixelFormat32bppBGRA;
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
		return GUID_WICPixelFormat32bppBGRA;
	default:
		return GUID_WICPixelFormatUndefined;
	}
}

DXGI_FORMAT Luxko::Anuthur::WIC::pixelFormat2DxgiFormat(GUID pixelFormat)
{
	if (pixelFormat == GUID_WICPixelFormat128bppRGBAFloat)
		return DXGI_FORMAT_R32G32B32A32_FLOAT;
	else if (pixelFormat == GUID_WICPixelFormat96bppRGBFloat)
		return DXGI_FORMAT_R32G32B32_FLOAT;
	else if (pixelFormat == GUID_WICPixelFormat64bppRGBAHalf)
		return DXGI_FORMAT_R16G16B16A16_FLOAT;
	else if (pixelFormat == GUID_WICPixelFormat64bppRGBA)
		return DXGI_FORMAT_R16G16B16A16_UNORM;
	else if (pixelFormat == GUID_WICPixelFormat32bppRGBA1010102)
		return DXGI_FORMAT_R10G10B10A2_UNORM;
	else if (pixelFormat == GUID_WICPixelFormat32bppRGBA)
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	else if (pixelFormat == GUID_WICPixelFormat32bppGrayFloat)
		return DXGI_FORMAT_R32_FLOAT;
	else if (pixelFormat == GUID_WICPixelFormat16bppGrayHalf)
		return DXGI_FORMAT_R16_FLOAT;
	else if (pixelFormat == GUID_WICPixelFormat16bppGray)
		return DXGI_FORMAT_R16_UNORM;
	else if (pixelFormat == GUID_WICPixelFormat8bppGray)
		return DXGI_FORMAT_R8_UNORM;
	else if (pixelFormat == GUID_WICPixelFormat8bppAlpha)
		return DXGI_FORMAT_A8_UNORM;
	else if (pixelFormat == GUID_WICPixelFormat16bppBGR565)
		return DXGI_FORMAT_B5G6R5_UNORM;
	else if (pixelFormat == GUID_WICPixelFormat16bppBGRA5551)
		return DXGI_FORMAT_B5G5R5A1_UNORM;
	else if (pixelFormat == GUID_WICPixelFormat32bppBGRA)
		return DXGI_FORMAT_B8G8R8A8_UNORM;
	else if (pixelFormat == GUID_WICPixelFormat32bppRGBA1010102XR)
		return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;
	else
		return DXGI_FORMAT_UNKNOWN;

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

void Luxko::Anuthur::WIC::SaveTexture2DAsPNG(ID3D12Resource* readBackHeap,
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