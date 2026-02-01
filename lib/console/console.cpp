#include "console.h"
#include "../../lib/utils/ports.h"

Console::Console()
	: m_row(0)
	, m_column(0)
{
	m_currentColor = MakeColor(White, Black);
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
	UpdateHardwareCursor();
}

void Console::PutChar(const char c)
{
	if (c == '\b')
	{
		if (m_column > 0)
		{
			m_column--;
			const size_t index = m_row * WIDTH + m_column;
			m_buffer[index] = MakeVGAEntry(' ', m_currentColor);
		}
		UpdateHardwareCursor();
		return;
	}
	if (c == '\n')
	{
		m_column = 0;
		if (++m_row == HEIGHT)
		{
			m_row = 0;
		}
		return;
	}

	const size_t index = m_row * WIDTH + m_column;
	m_buffer[index] = MakeVGAEntry(c, m_currentColor);

	if (++m_column == WIDTH)
	{
		m_column = 0;
		if (++m_row == HEIGHT)
		{
			m_row = 0;
		}
	}
	UpdateHardwareCursor();
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

void Console::PutCharAt(const size_t x, const size_t y, const char c, const uint8_t color) const
{
	if (x >= WIDTH || y >= HEIGHT)
	{
		return;
	}
	m_buffer[y * WIDTH + x] = static_cast<uint16_t>(c) | (static_cast<uint16_t>(color) << 8);
}

void Console::WriteAt(const size_t x, const size_t y, const char* str, const uint8_t color) const
{
	for (size_t i = 0; str[i] != '\0'; i++)
	{
		PutCharAt(x + i, y, str[i], color);
	}
}

void Console::WriteInt(uint32_t n)
{
	if (n == 0)
	{
		PutChar('0');
		return;
	}
	char buf[11];
	int i = 10;
	buf[i--] = '\0';
	while (n > 0)
	{
		buf[i--] = (n % 10) + '0';
		n /= 10;
	}
	Write(&buf[i + 1]);
}

void Console::WriteHex(const uint8_t n)
{
	const auto hex = "0123456789ABCDEF";
	PutChar(hex[(n >> 4) & 0xF]);
	PutChar(hex[n & 0xF]);
}

void Console::MoveCursorRelative(int dx, int dy)
{
	// Рассчитываем новую колонку
	int new_col = static_cast<int>(m_column) + dx;
	int new_row = static_cast<int>(m_row) + dy;

	// Проверки границ экрана
	if (new_col < 0) new_col = 0;
	if (new_col >= static_cast<int>(WIDTH)) new_col = WIDTH - 1;

	if (new_row < 0) new_row = 0;
	if (new_row >= static_cast<int>(HEIGHT)) new_row = HEIGHT - 1;

	m_column = static_cast<size_t>(new_col);
	m_row = static_cast<size_t>(new_row);

	UpdateHardwareCursor();
}

void Console::ClearCurrentLine()
{
	for (size_t x = 0; x < WIDTH; x++)
	{
		m_buffer[m_row * WIDTH + x] = MakeVGAEntry(' ', m_currentColor);
	}
	m_column = 0;
	UpdateHardwareCursor();
}

void Console::SetCursorPos(const size_t x, const size_t y)
{
	m_column = x;
	m_row = y;
	UpdateHardwareCursor();
}

size_t Console::GetColumn() const
{
	return m_column;
}

size_t Console::GetRow() const
{
	return m_row;
}

void Console::UpdateHardwareCursor() const
{
	const uint16_t position = m_row * WIDTH + m_column;
	Port::OutByte(0x3D4, 0x0F);
	Port::OutByte(0x3D5, static_cast<uint8_t>(position & 0xFF));
	Port::OutByte(0x3D4, 0x0E);
	Port::OutByte(0x3D5, static_cast<uint8_t>((position >> 8) & 0xFF));
}