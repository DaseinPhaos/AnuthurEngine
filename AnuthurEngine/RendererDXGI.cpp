//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#include "RendererDXGI.h"
#include "HelperMethods.h"

Luxko::DXGI::FactoryPtr Luxko::DXGI::CreateFactory()
{
	FactoryPtr p;
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&p)));
	
	return p;
}

Luxko::DXGI::AdapterPtr Luxko::DXGI::GetWarpAdapter(IDXGIFactory4* pFactory)
{
	AdapterPtr warp;
	ThrowIfFailed(pFactory->EnumWarpAdapter(IID_PPV_ARGS(&warp)));
	return warp;
}

Luxko::DXGI::AdapterPtr Luxko::DXGI::GetFirstHardwareAdapter(IDXGIFactory4* pFactory)
{
	IDXGIAdapter1* pAdapter;
	for (auto i = 0U; pFactory->EnumAdapters1(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
		DXGI_ADAPTER_DESC1 desc;
		pAdapter->GetDesc1(&desc);
		if ((desc.Flags&DXGI_ADAPTER_FLAG_SOFTWARE)!=0
			&& (FAILED(D3D12CreateDevice(pAdapter,D3D_FEATURE_LEVEL_11_0,_uuidof(ID3D12Device),nullptr)))) {
			pAdapter->Release();
			continue;
		}
		return pAdapter;
	}
	throw "Now device available, creation Failed.";
}
