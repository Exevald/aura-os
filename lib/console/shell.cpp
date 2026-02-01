#include "shell.h"
#include "../commands/commands.h"
#include "../history/history.h"

namespace
{
bool CompareStrings(const char* a, const char* b)
{
	int k = 0;
	while (a[k] && b[k])
	{
		if (a[k] != b[k])
		{
			return false;
		}
		k++;
	}
	return a[k] == b[k];
}
} // namespace

Shell::Shell(Console* c)
	: m_console(c)
{
	for (char& i : m_buffer)
	{
		i = 0;
	}
	SetPrompt();
}

void Shell::SetPrompt() const
{
	m_console->SetColor(Console::Cyan, Console::Black);
	m_console->Write("\naura@os:/$ ");
	m_console->SetColor(Console::White, Console::Black);
}

void Shell::SetAppRunning(const bool state)
{
	m_isAppRunning = state;
}

bool Shell::IsAppRunning() const
{
	return m_isAppRunning;
}

void Shell::HandleChar(const char c)
{
	if (c == static_cast<char>(0xE0))
	{
		if (const char* previousCommand = CommandHistory::GetPreviousCommand())
		{
			ClearCurrentLine();
			int i = 0;
			while (previousCommand[i])
			{
				m_buffer[i] = previousCommand[i];
				m_console->PutChar(m_buffer[i]);
				i++;
			}
			m_bufferLen = i;
			m_cursorPos = i;
		}
	}
	else if (c == static_cast<char>(0xE1))
	{
		const char* nextCommand = CommandHistory::GetNextCommand();
		if (nextCommand)
		{
			ClearCurrentLine();
			int i = 0;
			while (nextCommand[i])
			{
				m_buffer[i] = nextCommand[i];
				m_console->PutChar(m_buffer[i]);
				i++;
			}
			m_bufferLen = i;
			m_cursorPos = i;
		}
	}
	else if (c == static_cast<char>(0xE2))
	{
		if (m_cursorPos > 0)
		{
			m_cursorPos--;
			m_console->SetCursorPos(m_console->GetColumn() - 1, m_console->GetRow());
		}
	}
	else if (c == static_cast<char>(0xE3))
	{
		if (m_cursorPos < m_bufferLen)
		{
			m_cursorPos++;
			m_console->SetCursorPos(m_console->GetColumn() + 1, m_console->GetRow());
		}
	}
	else if (c == '\b')
	{
		HandleBackspaceKey();
	}
	else if (c == '\n')
	{
		m_buffer[m_bufferLen] = '\0';
		CommandHistory::AddNewCommand(m_buffer);
		Execute();

		for (char & k : m_buffer)
		{
			k = 0;
		}
		m_bufferLen = 0;
		m_cursorPos = 0;
		CommandHistory::ResetBrowseCommand();
		SetPrompt();
	}
	else if (m_bufferLen < 63 && c >= 32 && static_cast<unsigned char>(c) < 128)
	{
		m_buffer[m_cursorPos++] = c;
		if (m_cursorPos > m_bufferLen)
		{
			m_bufferLen = m_cursorPos;
		}
		m_console->PutChar(c);
	}
}

void Shell::Execute()
{
	if (m_bufferLen == 0)
		return;

	char* argv[16];
	int argc = 0;

	bool inputToken = false;

	for (int i = 0; i < m_bufferLen; i++)
	{
		if (m_buffer[i] == ' ')
		{
			m_buffer[i] = '\0';
			inputToken = false;
		}
		else
		{
			if (!inputToken)
			{
				if (argc < 16)
				{
					argv[argc++] = &m_buffer[i];
				}
				inputToken = true;
			}
		}
	}

	if (argc > 0)
	{
		HandleCommands(argc, argv);
	}
}

void Shell::ClearCurrentLine()
{
	m_console->SetCursorPos(m_promptLen, m_console->GetRow());

	for (int i = 0; i < m_bufferLen; i++)
	{
		m_console->PutChar(' ');
	}
	m_console->SetCursorPos(m_promptLen, m_console->GetRow());

	for (char& i : m_buffer)
	{
		i = 0;
	}
	m_bufferLen = 0;
	m_cursorPos = 0;
}

void Shell::HandleBackspaceKey()
{
	if (m_cursorPos > 0)
	{
		for (int i = m_cursorPos - 1; i < m_bufferLen - 1; i++)
		{
			m_buffer[i] = m_buffer[i + 1];
		}
		m_bufferLen--;
		m_cursorPos--;

		m_console->SetCursorPos(m_console->GetColumn() - 1, m_console->GetRow());
		m_console->PutChar(' ');
		m_console->SetCursorPos(m_console->GetColumn() - 1, m_console->GetRow());
	}
}

void Shell::HandleCommands(int argc, char** argv)
{
	char* cmd = argv[0];

	if (CompareStrings(cmd, "ls"))
	{
		Command::Ls(m_console);
	}
	else if (CompareStrings(cmd, "touch"))
	{
		if (argc > 1)
		{
			Command::Touch(argv[1], m_console);
		}
		else
		{
			m_console->Write("\nUsage: touch <filename>");
		}
	}
	else if (CompareStrings(cmd, "cat"))
	{
		if (argc > 1)
		{
			Command::Cat(argv[1], m_console);
		}
		else
		{
			m_console->Write("\nUsage: cat <filename>");
		}
	}
	else if (CompareStrings(cmd, "grep"))
	{
		if (argc > 2)
		{
			Command::Grep(argv[1], argv[2], m_console);
		}
		else
		{
			m_console->Write("\nUsage: grep <pattern> <filename>");
		}
	}
	else if (CompareStrings(cmd, "echo"))
	{
		if (argc > 2)
		{
			Command::Echo(argv[1], argv[2], m_console);
		}
		else if (argc == 2)
		{
			m_console->Write("\n");
			m_console->Write(argv[1]);
		}
		else
		{
			m_console->Write("\nUsage: echo <text> [filename]");
		}
	}
	else if (CompareStrings(cmd, "run"))
	{
		if (argc > 1)
		{
			Command::Run(argc - 1, &argv[1], m_console);
		}
		else
		{
			m_console->Write("\nUsage: run <filename> [args...]");
		}
	}
	else if (CompareStrings(cmd, "clear"))
	{
		Command::Clear(m_console);
	}
	else if (cmd[0] != '\0')
	{
		m_console->Write("\nUnknown command: ");
		m_console->Write(cmd);
	}
}