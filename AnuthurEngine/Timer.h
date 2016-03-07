#pragma once
#include "CommonHeader.h"

namespace Luxko {
	class Timer {
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