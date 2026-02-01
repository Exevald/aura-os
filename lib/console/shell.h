#pragma once

#include "console.h"

class Shell
{
public:
	explicit Shell(Console* c);

	void SetPrompt() const;
	void SetAppRunning(bool state);
	[[nodiscard]] bool IsAppRunning() const;
	void HandleChar(char c);
	void Execute();
	void ClearCurrentLine();

private:
	void HandleBackspaceKey();
	void HandleCommands(int argc, char** argv);

	char m_buffer[64]{};
	int m_bufferLen = 0;
	int m_cursorPos = 0;
	int m_promptLen = 11;
	Console* m_console;
	bool m_isAppRunning = false;
};