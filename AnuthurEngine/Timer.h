//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#pragma once

#ifdef LUXKOUTILITY_EXPORTS
#define LUXKOUTILITY_API _declspec(dllexport)
#else
#define LUXKOUTILITY_API _declspec(dllimport)
#endif


#include "CommonHeader.h"

namespace Luxko {
	class LUXKOUTILITY_API Timer {
	public:
		Timer();
		Timer(const Timer&) = default;
		Timer& operator = (const Timer&) = default;

		~Timer() {};
		void Reset() { QueryPerformanceCounter(&m_start); m_last = m_start; }
		void Elapse() { QueryPerformanceCounter(&m_last); }
		LONGLONG GetLastTicks()const noexcept;
		float GetLastMs()const noexcept;
	private:
		LARGE_INTEGER m_TicksPerSecond;
		LARGE_INTEGER m_start;
		LARGE_INTEGER m_last;
	};
	
}