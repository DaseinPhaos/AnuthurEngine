//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#pragma once
#include "RendererPCH.h"

inline void ThrowIfFailed(HRESULT hr) {
	if (FAILED(hr)) {
		throw "Operation failed due to code " + hr;
	}
}
