//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#pragma once
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
#include <wrl.h>
#include <vector>
#include <string>
#include <random>
#include <cmath>
#include <utility>
#include <algorithm>
#include <emmintrin.h>

#pragma warning ( disable: 4229)

// constexpr auto Pi_f = static_cast<float>(M_PI);

//#ifdef LUXKOUTILITY_EXPORTS
//#define LUXKOUTILITY_API _declspec(dllexport)
//#else
//#define LUXKOUTILITY_API _declspec(dllimport)
//#endif
//#ifdef ANUTHURMATH_EXPORTS
//#define ANUTHURMATH_API _declspec(dllexport)
//#else
//#define ANUTHURMATH_API _declspec(dllimport)
//#endif
#define LUXKOUTILITY_API
#define ANUTHURMATH_API

using namespace std::rel_ops;