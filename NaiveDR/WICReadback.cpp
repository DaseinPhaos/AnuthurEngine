#include "WICReadback.h"

IWICImagingFactory* Luxko::WIC::Get()
{
	static WIC wic;
	return wic._wicFactory.Get();
}

void Luxko::SaveTexture2DAsPNG(ID3D12Resource* readBackHeap, 
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
