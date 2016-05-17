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
		void Reset(bool initiallyStarted = false);
		void Start(); // After started, the timer won't 
					  // return valid time value until another Elapse() is called.

		void Pause();
		void Elapse();
		LONGLONG GetFirstTick()const noexcept;
		LONGLONG PeekCurrentTick()const;
		LONGLONG DeltaInTicks()const noexcept;
		float DeltaInMs()const noexcept;
		LONGLONG TotalInTicks()const noexcept;
		float TotalInMs()const noexcept;
		float TicksToSeconds(LONGLONG ticks)const noexcept;
		float TicksToMs(LONGLONG ticks)const noexcept;
	private:
		LARGE_INTEGER m_TicksPerSecond;
		LARGE_INTEGER m_start;
		LARGE_INTEGER m_last;
		LARGE_INTEGER m_pausedTime;
		LARGE_INTEGER m_pauseStartTime;
		bool m_paused;
	};
	
}