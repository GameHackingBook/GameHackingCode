#include "main.h"

// this is the function that scans the import table and
// overwrites the target function address with our hook
// destination address
DWORD hookIAT(const char* functionName, DWORD newFunctionAddress)
{
	DWORD baseAddress = (DWORD)GetModuleHandle(NULL);

	auto dosHeader = pointMemory<IMAGE_DOS_HEADER>(baseAddress);
	if (dosHeader->e_magic != 0x5A4D)
		return 0;

	auto optHeader = pointMemory<IMAGE_OPTIONAL_HEADER>(baseAddress + dosHeader->e_lfanew + 24);
	if (optHeader->Magic != 0x10B)
		return 0;

	if (optHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size == 0 ||
		optHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress == 0)
		return 0;

	IMAGE_IMPORT_DESCRIPTOR* importDescriptor = pointMemory<IMAGE_IMPORT_DESCRIPTOR>(baseAddress + optHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress); //what is the rule of adding them?
	while (importDescriptor->FirstThunk)
	{
		int n = 0;
		IMAGE_THUNK_DATA* thunkData = pointMemory<IMAGE_THUNK_DATA>(baseAddress + importDescriptor->OriginalFirstThunk);
		while (thunkData->u1.Function)
		{
			char* importFunctionName = pointMemory<char>(baseAddress + (DWORD)thunkData->u1.AddressOfData + 2);
			if (strcmp(importFunctionName, functionName) == 0)
			{
				auto vfTable = pointMemory<DWORD>(baseAddress + importDescriptor->FirstThunk);

				DWORD original = vfTable[n];

				auto oldProtection = protectMemory<DWORD>((DWORD)&vfTable[n], PAGE_READWRITE);
				vfTable[n] = newFunctionAddress;
				protectMemory<DWORD>((DWORD)&vfTable[n], oldProtection);

				return original;
			}

			n++;
			thunkData++;
		}
		importDescriptor++;
	}

	return 0;
}

// this our our type-def that minmics the function type
// of the function being hooked. This allows us to use a 
// clean call to the original function just knowing it's address
typedef VOID (WINAPI _origSleep)(DWORD ms);
_origSleep* originalSleep;

// this is the function we re-direct any Sleep() call to.
// it simply denies all Sleep calls that last for more than
// 100 miliseconds, printing some text upon success
VOID WINAPI newSleepFunction(DWORD ms)
{
	if (ms > 100)
		printf("Sleep hook worked! Denied sleep for %d miliseconds.\n", ms);
	else
		originalSleep(ms);
}

// This is the function that ties everything together
void IATHookExample()
{
	originalSleep = (_origSleep*)hookIAT("Sleep", (DWORD)&newSleepFunction);
	Sleep(1234);
}