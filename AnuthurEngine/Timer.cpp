//**********************************************************************
// This file is part of the Anuthur Engine. It is distributed under
// the MIT license(https://opensource.org/licenses/MIT).
//
// Copyright (c) Dasein Phaos aka. Luxko.
//**********************************************************************

#include "Timer.h"

Luxko::Timer::Timer()
{
	QueryPerformanceFrequency(&m_TicksPerSecond);
	Reset();
}

LONGLONG Luxko::Timer::GetLastTicks() const noexcept
{
	return m_last.QuadPart - m_start.QuadPart;
}

float Luxko::Timer::GetLastMs() const noexcept
{
	float ticksPerMs = m_TicksPerSecond.QuadPart / 1000.f;
	auto ticks = GetLastTicks();
	return ticks / ticksPerMs;
}
