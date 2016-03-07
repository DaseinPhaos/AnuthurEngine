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
