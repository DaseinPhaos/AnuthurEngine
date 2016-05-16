//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
#pragma once

#include "RendererPCH.h"

namespace Luxko {
	namespace Anuthur {
		inline void ThrowIfFailed(HRESULT hr, const wchar_t* wstr = nullptr) {
			if (FAILED(hr)) {
				if (wstr) throw std::wstring(wstr)+L"\nError code: "+std::to_wstring(hr);
				else throw L"Operation failed!\nError code " + std::to_wstring(hr);
			}
		}
	}
}