#include <iostream>
#include <windows.h>
#include <tlhelp32.h>


DWORD printStringManyTimes(int times, const char* string)
{
	for (int i = 0; i < times; i++)
		printf(string);
	return 0;
}


void injectCodeUsingThreadInjection(HANDLE process, LPVOID func, int times, const char* string)
{
	BYTE codeCave[20] = {
		0xFF, 0x74, 0x24, 0x04, // PUSH DWORD PTR[ESP+0x4]
		0x68, 0x00, 0x00, 0x00, 0x00, // PUSH 0
		0xB8, 0x00, 0x00, 0x00, 0x00, // MOV EAX, 0x0
		0xFF, 0xD0, // CALL EAX
		0x83, 0xC4, 0x08, // ADD ESP, 0x08
		0xC3 // RETN
	};

	// copy values to the shellcode
	memcpy(&codeCave[5], &times, 4);
	memcpy(&codeCave[10], &func, 4);


	// allocate memory for the coe cave
	int stringlen = strlen(string) + 1;
	int fulllen = stringlen + sizeof(codeCave);
	LPVOID remoteString = VirtualAllocEx(process, NULL, fulllen, MEM_COMMIT, PAGE_EXECUTE);
	LPVOID remoteCave = (LPVOID)((DWORD)remoteString + stringlen);

	// write the code cave
	WriteProcessMemory(process, remoteString, string, stringlen, NULL);
	WriteProcessMemory(process, remoteCave, codeCave, sizeof(codeCave), NULL);

	// run the thread
	HANDLE thread = CreateRemoteThread(process, NULL, NULL,
												(LPTHREAD_START_ROUTINE)remoteCave,
												remoteString, NULL, NULL);
	WaitForSingleObject(thread, INFINITE);
	CloseHandle(thread);
}


DWORD GetProcessThreadID(HANDLE Process)
{
	THREADENTRY32 entry;
    entry.dwSize = sizeof(THREADENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

	if (Thread32First(snapshot, &entry) == TRUE)
	{
		DWORD PID = GetProcessId(Process);
		while (Thread32Next(snapshot, &entry) == TRUE)
		{
			if (entry.th32OwnerProcessID == PID)
			{
				CloseHandle(snapshot);
				return entry.th32ThreadID;
			}
		}
	}
	CloseHandle(snapshot);
	return NULL;
}



void injectCodeUsingThreadHijacking(HANDLE process, LPVOID func, int times, const char* string)
{
	BYTE codeCave[31] = {
		0x60, //PUSHAD
		0x9C, //PUSHFD
		0x68, 0x00, 0x00, 0x00, 0x00, // PUSH 0
		0x68, 0x00, 0x00, 0x00, 0x00, // PUSH 0
		0xB8, 0x00, 0x00, 0x00, 0x00, // MOV EAX, 0x0
		0xFF, 0xD0, // CALL EAX
		0x83, 0xC4, 0x08, // ADD ESP, 0x08
		0x9D, //POPFD
		0x61, //POPAD
		0x68, 0x00, 0x00, 0x00, 0x00, // PUSH 0
		0xC3 // RETN
	};

	// allocate memory for the coe cave
	int stringlen = strlen(string) + 1;
	int fulllen = stringlen + sizeof(codeCave);
	LPVOID remoteString = VirtualAllocEx(process, NULL, fulllen, MEM_COMMIT, PAGE_EXECUTE);
	LPVOID remoteCave = (LPVOID)((DWORD)remoteString + stringlen);

	// suspend the thread and query its control context
	DWORD threadID = GetProcessThreadID(process);
	HANDLE thread = OpenThread((THREAD_GET_CONTEXT | THREAD_SUSPEND_RESUME | THREAD_SET_CONTEXT), false, threadID);
	SuspendThread(thread);

	CONTEXT threadContext;
	threadContext.ContextFlags = CONTEXT_CONTROL;
	GetThreadContext(thread, &threadContext);

	// copy values to the shellcode (happens late because we need values from allocation)
	memcpy(&codeCave[3], &remoteString, 4);
	memcpy(&codeCave[8], &times, 4);
	memcpy(&codeCave[13], &func, 4);
	memcpy(&codeCave[25], &threadContext.Eip, 4);


	// write the code cave
	WriteProcessMemory(process, remoteString, string, stringlen, NULL);
	WriteProcessMemory(process, remoteCave, codeCave, sizeof(codeCave), NULL);


	//hijack the thread
	threadContext.Eip = (DWORD)remoteCave;
	threadContext.ContextFlags = CONTEXT_CONTROL;
	SetThreadContext(thread, &threadContext);
	ResumeThread(thread);

	//clean
	CloseHandle(thread);
}

DWORD WINAPI hijackThread(LPVOID lpParam)
{
	injectCodeUsingThreadHijacking((HANDLE)lpParam, &printStringManyTimes, 2, "hijacked\n");
	return 1;
}


void LoadDll(HANDLE process, const wchar_t* dllPath)
{
	// write the dll name to memory
	int namelen = wcslen(dllPath) + 1;
	LPVOID remoteString = VirtualAllocEx(process, NULL, namelen * 2, MEM_COMMIT, PAGE_EXECUTE);
	WriteProcessMemory(process, remoteString, dllPath, namelen * 2, NULL);

	// get the address of GetModuleHandle()
	HMODULE k32 = GetModuleHandleA("kernel32.dll");
	LPVOID funcAdr = GetProcAddress(k32, "LoadLibraryW");

	// create the thread
	HANDLE thread =
		CreateRemoteThread(process, NULL, NULL, (LPTHREAD_START_ROUTINE)funcAdr, remoteString, NULL, NULL);
	
	// let the thread finish and clean up
	WaitForSingleObject(thread, INFINITE);
	CloseHandle(thread);
}

int main(void)
{
	HANDLE proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());

	// inject code into self using thread injection
	injectCodeUsingThreadInjection(proc, &printStringManyTimes, 2, "injected\n");

	// inject code into self using thread hijacking
	//   we need to do it from a secondary thread or else
	//   the hijacking code would hijack itself.. which
	//   doesn't work
	CreateThread(NULL, 0, hijackThread, proc, 0, NULL); 

	LoadDll(proc, L"Chapter7_CodeInjection_DLL.dll");


	while (true) // stay busy
	{
		Sleep(100);
	}
}