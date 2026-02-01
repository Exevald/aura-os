#pragma once

#include "../console/console.h"
#include "interrupt_handler.h"

class SyscallHandler : public InterruptHandler
{
public:
	explicit SyscallHandler(Console* console);
	void HandleInterrupt(Registers* regs) override;

private:
	Console* m_console;
};