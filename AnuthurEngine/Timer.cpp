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

void Luxko::Timer::Reset(bool initiallyStarted /*= false*/)
{
	QueryPerformanceCounter(&m_start);
	m_last = m_start;
	m_pausedTime.QuadPart = 0;
	m_pauseStartTime = m_start;
	m_paused = !initiallyStarted;
}

void Luxko::Timer::Start()
{
	if (m_paused) {
		LARGE_INTEGER current;
		QueryPerformanceCounter(&current);
		m_pausedTime.QuadPart += current.QuadPart - m_pauseStartTime.QuadPart;
		//m_last = current;
		m_paused = false;
	}
}

void Luxko::Timer::Pause()
{
	if (!m_paused) {
		m_paused = true;
		QueryPerformanceCounter(&m_pauseStartTime);
	}
}

void Luxko::Timer::Elapse()
{
	if (m_paused) {
		m_last = m_pauseStartTime;
	}
	else {
		QueryPerformanceCounter(&m_last);
	}
}

LONGLONG Luxko::Timer::GetFirstTick() const noexcept
{
	return m_start.QuadPart;
}

LONGLONG Luxko::Timer::PeekCurrentTick() const
{
	LARGE_INTEGER currentTick;
	QueryPerformanceCounter(&currentTick);
	return currentTick.QuadPart;
}

LONGLONG Luxko::Timer::DeltaInTicks() const noexcept
{
	return m_last.QuadPart - m_start.QuadPart - m_pausedTime.QuadPart;
}

float Luxko::Timer::DeltaInMs() const noexcept
{
	auto ticks = DeltaInTicks();
	if (ticks < 0) ticks = 0;
	return TicksToMs(ticks);
}

LONGLONG Luxko::Timer::TotalInTicks() const noexcept
{
	return m_last.QuadPart - m_start.QuadPart;
}

float Luxko::Timer::TotalInMs() const noexcept
{
	auto ticks = TotalInTicks();
	if (ticks < 0) ticks = 0;
	return TicksToMs(ticks);
}

float Luxko::Timer::TicksToSeconds(LONGLONG ticks) const noexcept
{
	return ticks / static_cast<float>(m_TicksPerSecond.QuadPart);
}

float Luxko::Timer::TicksToMs(LONGLONG ticks) const noexcept
{
	float ticksPerMs = m_TicksPerSecond.QuadPart / 1000.f;
	return ticks / ticksPerMs;
}
