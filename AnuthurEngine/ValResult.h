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
	struct ValResult {
		T results[N];
		size_t count;
		T& operator[](size_t i) { return results[i]; }
		T operator[](size_t i)const { return results[i]; }
	};

	template<typename T, size_t M, size_t N>
	ValResult<T, M + N> ValResultMerge(const ValResult<T, M>& tm, const ValResult<T, N>& tn);

	template<typename T, size_t M, size_t N>
	ValResult<T, M + N> ValResultMerge(const ValResult<T, M>& tm, const ValResult<T, N>& tn) {
		ValResult<T, M + N> result;
		result.count = tm.count + tn.count;
		size_t i;
		for (i = 0U; i < tm.count; ++i) {
			result[i] = tm[i];
		}
		for (auto j = 0U; j < tn.count; ++i, ++j) {
			result[i] = tn[j];
		}
		return result;
	}
}
