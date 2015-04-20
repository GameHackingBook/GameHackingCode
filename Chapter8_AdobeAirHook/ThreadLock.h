#pragma once
#include <Windows.h>

class ThreadLock
{
public:
	ThreadLock()
	{
		InitializeCriticalSection(&cs);
	}

	~ThreadLock(){
		DeleteCriticalSection(&cs);
	}

	void enter()
	{
		EnterCriticalSection(&cs);
	}

	void leave()
	{
		LeaveCriticalSection(&cs);
	}
private:
	CRITICAL_SECTION cs;
};