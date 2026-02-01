#include "../drivers/keyboard.h"
#include "../drivers/pic.h"
#include "../lib/console/shell.h"
#include "../lib/cpu/idt.h"
#include "../lib/cpu/syscall_handler.h"
#include "../lib/fs/fs.h"
#include "../lib/mm/heap.h"
#include "../lib/mm/paging.h"
#include "../lib/mm/pmm.h"
#include "../lib/utils/timer.h"

uint32_t Timer::m_ticks = 0;
Console* ptrConsole = nullptr;
Shell* globalShell = nullptr;

extern "C" [[noreturn]] void kernel_main()
{
	static Console console;
	ptrConsole = &console;
	console.Clear();

	PMM::Init(0x400000);
	VirtualMemoryManager::Init();

	heap::InitKernelHeap();

	static InterruptManager interruptManager;
	static KeyboardDriver keyboard(&interruptManager, &console);
	static SyscallHandler syscallHandler(&console);
	interruptManager.RegisterHandler(128, &syscallHandler);

	PICRemap(0x20, 0x28);
	LoadIDT();

	InterruptManager::Activate();

	FS::Format();
	FS::CreateFile("hello.elf", 300, 16384);

	static Shell shell(&console);
	globalShell = &shell;

	while (true)
	{
		if (keyboard.HasKeys())
		{
			shell.HandleChar(keyboard.PopKey());
		}
		asm volatile("hlt");
	}
}