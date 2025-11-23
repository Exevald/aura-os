#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdint.h>
#include <stddef.h>

class Console
{
public:
	enum Color : uint8_t
	{
		Black = 0,
		Blue = 1,
		Green = 2,
		Cyan = 3,
		Red = 4,
		Magenta = 5,
		Brown = 6,
		LightGrey = 7,
		DarkGrey = 8,
		LightBlue = 9,
		LightGreen = 10,
		LightCyan = 11,
		LightRed = 12,
		LightMagenta = 13,
		LightBrown = 14,
		White = 15,
	};

	Console();

	void Init();
	void SetColor(Color text, Color background);
	void Clear();
	void PutChar(char c);
	void Write(const char* str);

private:
	static constexpr size_t WIDTH = 80;
	static constexpr size_t HEIGHT = 25;
	uint16_t* const m_buffer = reinterpret_cast<uint16_t*>(0xB8000);

	size_t m_row;
	size_t m_column;
	uint8_t m_currentColor;

	static uint8_t MakeColor(const Color& foregroundColor, const Color& backgroundColor);
	static uint16_t MakeVGAEntry(char c, uint8_t color);
};

#endif // CONSOLE_H
