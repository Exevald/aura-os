#include "almanac.h"
#include "../lib/console/shell.h"

int Almanac::m_selectedIndex = 0;
int Almanac::m_fileCount = 0;
bool Almanac::m_isViewingFile = false;
FileEntry Almanac::m_cachedEntries[8];

extern Shell* globalShell;
extern Console* ptrConsole;

void Almanac::Init()
{
	m_selectedIndex = 0;
	m_isViewingFile = false;
	ATADriver::ReadSectors(FS::ROOT_LBA, 1, reinterpret_cast<uint16_t*>(m_cachedEntries));
	m_fileCount = 0;
	for (const auto& entry : m_cachedEntries)
	{
		if (entry.active)
		{
			m_fileCount++;
		}
	}
}

void Almanac::Draw(Console& console)
{
	if (m_isViewingFile)
	{
		return;
	}
	console.Clear();
	console.WriteAt(0, 0, " [ Aura Almanac - File Manager ] ", 0x3F);

	for (int i = 0; i < 8; i++)
	{
		if (m_cachedEntries[i].active)
		{
			const uint8_t color = (i == m_selectedIndex) ? 0x30 : 0x1F;
			console.WriteAt(2, 2 + i, "                                      ", color);
			console.WriteAt(4, 2 + i, m_cachedEntries[i].name, color);
		}
	}
	console.WriteAt(0, 24, " F3:View  F10:Quit  Arrows:Navigate", 0x07);
}

void Almanac::HandleInput(const char key)
{
	if (m_isViewingFile)
	{
		if (key == static_cast<char>(0x1B) || key == static_cast<char>(0xFA))
		{
			m_isViewingFile = false;
			Draw(*ptrConsole);
		}
		return;
	}

	if (key == static_cast<char>(0xE0))
	{
		if (m_selectedIndex > 0)
		{
			m_selectedIndex--;
		}
	}
	else if (key == static_cast<char>(0xE1))
	{
		if (m_selectedIndex < m_fileCount - 1)
		{
			m_selectedIndex++;
		}
	}
	else if (key == static_cast<char>(0xF3))
	{
		ViewFile(m_cachedEntries[m_selectedIndex]);
		return;
	}
	else if (key == static_cast<char>(0xFA) || key == static_cast<char>(0x1B))
	{
		globalShell->SetAppRunning(false);
		ptrConsole->Clear();
		globalShell->SetPrompt();
		return;
	}
	Draw(*ptrConsole);
}

void Almanac::ViewFile(const FileEntry& file)
{
	m_isViewingFile = true;
	ptrConsole->Clear();
	ptrConsole->SetColor(Console::Black, Console::LightGrey);
	ptrConsole->Write("Viewing File: ");
	ptrConsole->Write(file.name);
	ptrConsole->Write("\n----------------------------------------\n");

	char buffer[512];
	ATADriver::ReadSectors(file.startLBA, 1, reinterpret_cast<uint16_t*>(buffer));
	ptrConsole->Write(buffer);

	ptrConsole->Write("\n\n [ Press ESC to return to list ]");
}