#include "syscall_handler.h"
#include "../../drivers/keyboard.h"

extern KeyboardDriver* globalKeyboard;

#define SYS_WRITE 1
#define SYS_READ 2
#define SYS_EXIT 3

SyscallHandler::SyscallHandler(Console* console)
	: m_console(console)
{
}

void SyscallHandler::HandleInterrupt(Registers* regs)
{
	if (regs->eax == SYS_WRITE)
	{
		const char* str = reinterpret_cast<const char*>(regs->ebx);
		m_console->Write(str);
	}
	else if (regs->eax == SYS_READ)
	{
		if (globalKeyboard)
		{
			globalKeyboard->ReadLine(reinterpret_cast<char*>(regs->ebx), regs->ecx);
		}
	}
	else if (regs->eax == SYS_EXIT)
	{
		m_console->Write("\nProgram exited with code: ");
		m_console->WriteInt(regs->ebx);
		m_console->Write("\n");
	}
}