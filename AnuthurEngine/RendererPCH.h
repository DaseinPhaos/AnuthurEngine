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
//#define ANUTHURRENDERER_API

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif
#ifndef STRICT
#define STRICT
#endif
#ifndef WIN32_LEAN_AND_MEAN 
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include <windows.h>
#include <windowsx.h>

#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>


#include <string>
#include <wrl.h>
#include "ApplicationBase.h"
#include "ApplicationHelper.h"

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")
