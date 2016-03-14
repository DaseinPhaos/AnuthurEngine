//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************
#pragma once

#ifdef ANUTHURRENDERER_EXPORTS
#define ANUTHURRENDERER_API _declspec(dllexport)
#else
#define ANUTHURRENDERER_API _declspec(dllimport)
#endif

#ifndef WIN32_LEAN_AND_MEAN 
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>


#include <string>
#include <wrl.h>
