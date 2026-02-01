#include "keyboard.h"
#include "../apps/almanac.h"
#include "../lib/console/shell.h"

extern Shell* globalShell;
KeyboardDriver* globalKeyboard = nullptr;

constexpr char ScanCodeLookupTable[128] = {
	0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', /* 0x00 - 0x0E */
	'\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* 0x0F - 0x1C */
	0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', /* 0x1D - 0x29 */
	0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, /* 0x2A - 0x35 */
	'*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x36 - 0x42 */
	0, 0, 0, 0, 0, 0, 0, 7, 8, 9, '-', 4, 5, /* 0x43 - 0x4F */
	6, '+', 1, 2, 3, 0, '.', 0, 0, 0, 0, 0, 0, /* 0x50 - 0x5C */
};

KeyboardDriver::KeyboardDriver(InterruptManager* manager, Console* console)
{
	manager->RegisterHandler(33, this);
	m_console = console;
	globalKeyboard = this;
}

void KeyboardDriver::HandleInterrupt(Registers* regs)
{
	const uint8_t scancode = Port::InByte(0x60);
	if (scancode & 0x80)
	{
		return;
	}
	char ch = ScanCodeLookupTable[scancode];

	if (m_userBuffer != nullptr)
	{
		if (scancode == 0x1C)
		{
			m_userBuffer[m_currentPos] = '\0';
			m_lineStatus = true;
			m_console->Write("\n");
			return;
		}

		if (scancode == 0x0E)
		{
			if (m_currentPos > 0)
			{
				m_currentPos--;
				m_console->PutChar('\b');
			}
			return;
		}

		if (scancode == 0x4B)
		{
			if (m_currentPos > 0)
			{
				m_currentPos--;
				m_console->MoveCursorRelative(-1, 0);
			}
			return;
		}

		if (scancode == 0x4D)
		{
			if (m_currentPos < m_maxSize - 1)
			{
				m_currentPos++;
				m_console->MoveCursorRelative(1, 0);
			}
			return;
		}

		if (ch != 0 && m_currentPos < m_maxSize - 1)
		{
			m_userBuffer[m_currentPos++] = ch;
			m_console->PutChar(ch);
		}
		return;
	}

	if (ch != 0)
	{
		if ((uint8_t)(m_writePos + 1) != m_readPos)
		{
			m_keyQueue[m_writePos++] = ch;
		}
	}
}

bool KeyboardDriver::HasKeys() const
{
	return m_readPos != m_writePos;
}

char KeyboardDriver::PopKey()
{
	if (!HasKeys())
	{
		return 0;
	}
	return m_keyQueue[m_readPos++];
}

void KeyboardDriver::ReadLine(char* buffer, uint32_t maxSize)
{
	m_userBuffer = buffer;
	m_maxSize = maxSize;
	m_currentPos = 0;
	m_lineStatus = false;

	asm volatile("sti");

	while (!m_lineStatus)
	{
		asm volatile("hlt");
	}

	asm volatile("cli");

	m_userBuffer = nullptr;
}