#pragma once

#include "../lib/console/console.h"
#include "../lib/cpu/interrupt_manager.h"

class KeyboardDriver final : public InterruptHandler
{
public:
	explicit KeyboardDriver(InterruptManager* manager, Console* console);
	void HandleInterrupt(Registers* regs) override;
	void ReadLine(char* buffer, uint32_t maxSize);

	[[nodiscard]] bool HasKeys() const;
	char PopKey();

private:
	Console* m_console;

	char m_keyQueue[256]{};
	uint8_t m_writePos = 0;
	uint8_t m_readPos = 0;

	char* m_userBuffer = nullptr;
	uint32_t m_maxSize = 0;
	uint32_t m_currentPos = 0;
	volatile bool m_lineStatus = false;
};