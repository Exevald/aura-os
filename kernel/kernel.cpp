#include "../lib/console/console.h"
#include "../lib/mm/paging.h"
#include "../lib/utils/timer.h"

uint32_t Timer::m_ticks = 0;

extern "C" void _cxa_pure_virtual()
{
	while (true)
	{
	}
}

extern "C" void kernel_main()
{
	Console console;

	console.Write("Aura OS Kernel (C++ Edition)\n");
	console.SetColor(Console::LightGreen, Console::Blue);
	console.Write("System status: OK\n");
	console.SetColor(Console::Red, Console::Blue);
}
