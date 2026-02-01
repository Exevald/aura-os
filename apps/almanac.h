#pragma once

#include "../lib/console/console.h"
#include "../lib/fs/fs.h"

class Shell;

class Almanac
{
public:
	static void Init();
	static void Draw(Console& console);
	static void HandleInput(char key);
	static void ViewFile(const FileEntry& file);

private:
	static int m_selectedIndex;
	static int m_fileCount;
	static FileEntry m_cachedEntries[8];
	static bool m_isViewingFile;
};