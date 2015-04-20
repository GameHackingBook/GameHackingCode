#pragma once
#include <windows.h>
#include <stdio.h>

#ifdef _DEBUG
#define LOG(message) printf("%s (%d): %s\n", __FUNCTION__, __LINE__, message)
#else
#define LOG(message)
#endif

class DebugConsole
{
public:
	static DebugConsole* getInstance()
	{
		if (!DebugConsole::instance)
			DebugConsole::instance = new DebugConsole();
		return DebugConsole::instance;
	}
	static void deleteInstance()
	{
		if (DebugConsole::instance)
		{
			delete DebugConsole::instance;
			DebugConsole::instance = NULL;
		}
	}
	void dumpBuffer(const char* buffer, unsigned int size, const char* title);
	void dumpBuffer(char* buffer, unsigned int size, const char* title) { this->dumpBuffer((const char*)buffer, size, title); }
	void dumpBuffer(unsigned char* buffer, unsigned int size, const char* title) { this->dumpBuffer((const char*)buffer, size, title); }
	void dumpBuffer(const unsigned char* buffer, unsigned int size, const char* title) { this->dumpBuffer((const char*)buffer, size, title); }

private:
	static DebugConsole* instance;

	DebugConsole();
	~DebugConsole();

	void show();
	void hide();
};