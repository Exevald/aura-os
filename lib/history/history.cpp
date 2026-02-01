#include "history.h"

char CommandHistory::m_history[HISTORY_MAX_SIZE][CMD_MAX_LEN];
int CommandHistory::m_count = 0;
int CommandHistory::m_currentIdx = 0;
int CommandHistory::m_browseCommandIndex = 0;

void CommandHistory::AddNewCommand(const char* cmd)
{
	if (cmd[0] == '\0')
	{
		return;
	}
	if (const int lastIndex = (m_currentIdx - 1 + HISTORY_MAX_SIZE) % HISTORY_MAX_SIZE;
		m_count > 0 && StrCmp(m_history[lastIndex], cmd))
	{
		return;
	}

	int i = 0;
	while (cmd[i] && i < CMD_MAX_LEN - 1)
	{
		m_history[m_currentIdx][i] = cmd[i];
		i++;
	}
	m_history[m_currentIdx][i] = '\0';

	m_currentIdx = (m_currentIdx + 1) % HISTORY_MAX_SIZE;
	if (m_count < HISTORY_MAX_SIZE)
	{
		m_count++;
	}

	ResetBrowseCommand();
}

const char* CommandHistory::GetPreviousCommand()
{
	if (m_count <= 0)
		return nullptr;

	m_browseCommandIndex--;
	if (m_browseCommandIndex < 0)
	{
		m_browseCommandIndex = 0;
	}

	int realCommandIndex = (m_currentIdx - m_count + m_browseCommandIndex + HISTORY_MAX_SIZE) % HISTORY_MAX_SIZE;
	return m_history[realCommandIndex];
}

const char* CommandHistory::GetNextCommand()
{
	if (m_count <= 0)
	{
		return nullptr;
	}

	m_browseCommandIndex++;
	if (m_browseCommandIndex >= m_count)
	{
		m_browseCommandIndex = m_count - 1;
		return nullptr;
	}

	int realCommandIndex = (m_currentIdx - m_count + m_browseCommandIndex + HISTORY_MAX_SIZE) % HISTORY_MAX_SIZE;
	return m_history[realCommandIndex];
}

void CommandHistory::ResetBrowseCommand()
{
	m_browseCommandIndex = m_count;
}

bool CommandHistory::StrCmp(const char* a, const char* b)
{
	int i = 0;
	while (a[i] && b[i])
	{
		if (a[i] != b[i])
		{
			return false;
		}
		i++;
	}
	return a[i] == b[i];
}
