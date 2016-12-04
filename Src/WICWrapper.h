//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
#pragma once
#include "RendererPCH.h"
#include "D3D12HelperMethods.h"
#include <wincodec.h>

namespace Luxko {
	using Microsoft::WRL::ComPtr;
	namespace Anuthur {
		class WIC {
		public:
			static IWICImagingFactory* Get();

			static void ReadTextureFromMemory(
				const uint8_t* wicData,
				size_t wicDataSize,
				std::unique_ptr<uint8_t[]>& decodedData,
				bool forceSRGB = false,
				size_t maxsize = 0);

			static void ReadTextureFromFile(
				const wchar_t* szFileName,
				std::unique_ptr<uint8_t[]>& decodedData,
				bool forceSRGB = false,
				size_t maxsize = 0);


			// Standard version
			static ComPtr<ID3D12Resource> LoadTextureFromMemory(
				ID3D12Device* d3dDevice,
				ID3D12GraphicsCommandList* pCmdList,
				const uint8_t* wicData,
				size_t wicDataSize,
				ComPtr<ID3D12Resource>& uploadBuffer,
				std::unique_ptr<uint8_t[]>& decodedData,
				D3D12_SUBRESOURCE_DATA& subresource,
				size_t maxsize = 0);

			static ComPtr<ID3D12Resource> LoadTextureFromFile(
				ID3D12Device* d3dDevice,
				ID3D12GraphicsCommandList* pCmdList,
				const wchar_t* szFileName,
				ComPtr<ID3D12Resource>& uploadBuffer,
				std::unique_ptr<uint8_t[]>& decodedData,
				D3D12_SUBRESOURCE_DATA& subresource,
				size_t maxsize = 0);

			// Extended version
			static ComPtr<ID3D12Resource> LoadTextureFromMemoryEx(
				ID3D12Device* d3dDevice,
				ID3D12GraphicsCommandList* pCmdList,
				const uint8_t* wicData,
				size_t wicDataSize,
				size_t maxsize,
				D3D12_RESOURCE_FLAGS flags,
				bool forceSRGB,
				ComPtr<ID3D12Resource>& uploadBuffer,
				std::unique_ptr<uint8_t[]>& decodedData,
				D3D12_SUBRESOURCE_DATA& subresource);

			static ComPtr<ID3D12Resource> LoadTextureFromFileEx(
				ID3D12Device* d3dDevice,
				ID3D12GraphicsCommandList* pCmdList,
				const wchar_t* szFileName,
				size_t maxsize,
				D3D12_RESOURCE_FLAGS flags,
				bool forceSRGB,
				ComPtr<ID3D12Resource>& uploadBuffer,
				std::unique_ptr<uint8_t[]>& decodedData,
				D3D12_SUBRESOURCE_DATA& subresource);

			static GUID dxgiFormat2PixelFormat(DXGI_FORMAT format);

			static DXGI_FORMAT pixelFormat2DxgiFormat(GUID pixelFormat);

			static void SaveTexture2DAsPNG(ID3D12Resource* readBackHeap,
				const wchar_t* filename,
				unsigned int subResource, unsigned int width,
				unsigned int height, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN);
		private:
			WIC() {
				ThrowIfFailed(CoInitialize(nullptr));
				ThrowIfFailed(CoCreateInstance(CLSID_WICImagingFactory,
					nullptr, CLSCTX_INPROC_SERVER,
					IID_PPV_ARGS(_wicFactory.GetAddressOf())));
			}
			ComPtr<IWICImagingFactory> _wicFactory;
		};


	}

}