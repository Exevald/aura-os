#pragma once

#include <stdint.h>

class Timer
{

public:
	static void Tick() { m_ticks++; }
	static uint32_t GetTime() { return m_ticks; }

private:
	static uint32_t m_ticks;
};