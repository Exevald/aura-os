#include "interrupt_manager.h"
#include "../utils/ports.h"

InterruptManager* InterruptManager::m_activeInterruptManager = nullptr;

extern "C" void isr_handler(Registers* regs)
{
	if (InterruptManager::GetInstance() != nullptr)
	{
		InterruptManager::GetInstance()->DoHandleInterrupt(regs);
	}
}

InterruptManager::InterruptManager()
{
	for (auto& handler : m_handlers)
	{
		handler = nullptr;
	}

	m_activeInterruptManager = this;
}

InterruptManager* InterruptManager::GetInstance()
{
	return m_activeInterruptManager;
}

void InterruptManager::RegisterHandler(uint8_t interruptNumber, InterruptHandler* handler)
{
	m_handlers[interruptNumber] = handler;
}

void InterruptManager::Activate()
{
	asm volatile("sti");
}

void InterruptManager::DoHandleInterrupt(Registers* regs) const
{
	auto interruptionNumber = static_cast<uint8_t>(regs->interruptionNumber);
	if (m_handlers[interruptionNumber] != nullptr)
	{
		m_handlers[interruptionNumber]->HandleInterrupt(regs);
	}

	if (interruptionNumber >= 0x20 && interruptionNumber < 0x30)
	{
		Port::OutByte(0x20, 0x20);
		if (interruptionNumber >= 0x28)
		{
			Port::OutByte(0xA0, 0x20);
		}
	}
}