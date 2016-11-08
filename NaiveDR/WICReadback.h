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
		private:
			WIC() {
				ThrowIfFailed(CoInitialize(nullptr));
				ThrowIfFailed(CoCreateInstance(CLSID_WICImagingFactory,
					nullptr, CLSCTX_INPROC_SERVER,
					IID_PPV_ARGS(_wicFactory.GetAddressOf())));
			}
			ComPtr<IWICImagingFactory> _wicFactory;

		};

		void SaveTexture2DAsPNG(ID3D12Resource* readBackHeap,
			unsigned int width, unsigned int height, const wchar_t* filename);
	}

}