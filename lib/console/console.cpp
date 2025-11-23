#include "console.h"

Console::Console()
	: m_row(0)
	, m_column(0)
{
	m_currentColor = MakeColor(White, Blue);
	Clear();
}

void Console::SetColor(const Color text, const Color background)
{
	m_currentColor = MakeColor(text, background);
}

void Console::Clear()
{
	for (size_t y = 0; y < HEIGHT; y++)
	{
		for (size_t x = 0; x < WIDTH; x++)
		{
			const size_t index = y * WIDTH + x;
			m_buffer[index] = MakeVGAEntry(' ', m_currentColor);
		}
	}
	m_row = 0;
	m_column = 0;
}

void Console::PutChar(const char c)
{
	if (c == '\n')
	{
		m_column = 0;
		if (++m_row == HEIGHT)
			m_row = 0;
		return;
	}

	const size_t index = m_row * WIDTH + m_column;
	m_buffer[index] = MakeVGAEntry(c, m_currentColor);

	if (++m_column == WIDTH)
	{
		m_column = 0;
		if (++m_row == HEIGHT)
			m_row = 0;
	}
}

void Console::Write(const char* str)
{
	for (size_t i = 0; str[i] != '\0'; i++)
	{
		PutChar(str[i]);
	}
}

uint8_t Console::MakeColor(const Color& foregroundColor, const Color& backgroundColor)
{
	return static_cast<uint8_t>(foregroundColor) | (static_cast<uint8_t>(backgroundColor) << 4);
}

uint16_t Console::MakeVGAEntry(const char c, const uint8_t color)
{
	return static_cast<uint16_t>(c) | (static_cast<uint16_t>(color) << 8);
}