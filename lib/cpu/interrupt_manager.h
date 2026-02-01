#pragma once

#include "interrupt_handler.h"

#include <stdint.h>

class InterruptManager
{
public:
	InterruptManager();
	static InterruptManager* GetInstance();

	static void Activate();
	void RegisterHandler(uint8_t interruptNumber, InterruptHandler* handler);
	void DoHandleInterrupt(Registers* regs) const;

protected:
	static InterruptManager* m_activeInterruptManager;
	InterruptHandler* m_handlers[256]{};
};