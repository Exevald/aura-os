#pragma once

#define HISTORY_MAX_SIZE 10
#define CMD_MAX_LEN 64

class CommandHistory
{
public:
	static void AddNewCommand(const char* cmd);
	static const char* GetPreviousCommand();
	static const char* GetNextCommand();
	static void ResetBrowseCommand();

private:
	static bool StrCmp(const char* a, const char* b);

	static char m_history[HISTORY_MAX_SIZE][CMD_MAX_LEN];
	static int m_count;
	static int m_currentIdx;
	static int m_browseCommandIndex;
};