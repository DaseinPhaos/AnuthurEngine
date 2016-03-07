//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#pragma once
#include "CommonHeader.h"

namespace Luxko {
	template<typename T, size_t N>
	struct ValResult{
		T results[N];
		size_t count;
		T& operator[](size_t i) { return results[i]; }
		T operator[](size_t i)const { return results[i]; }
	};
}
