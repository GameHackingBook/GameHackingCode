#include "ExecutableModule.h"

#include <tlhelp32.h>
#include <string>


ExecutableModule::ExecutableModule(const wchar_t* moduleName) : name(moduleName), base(0), size(0), oldProtect(0)
{
	this->getModuleInformation();
}


ExecutableModule::~ExecutableModule(void)
{
}

DWORD ExecutableModule::findPattern(const char* pattern, unsigned int patternLength, unsigned int occurance)
{
	unsigned int ocur = 0;
	for (DWORD adr = this->base; adr <= this->base + this->size - patternLength; adr++)
	{

		if (memcmp((LPVOID)pattern, (LPVOID)adr, patternLength) == 0)
		{
			ocur++;
			if (ocur == occurance)
				return adr;
		}
	}
	return 0;
}

DWORD ExecutableModule::addCallHook(const char* name, DWORD address, LPVOID function)
{
	callHookInformation hook;
	hook.hookAtAddress = address;
	hook.newFuncAddress = (DWORD)function;

	if (this->readMemory<BYTE>(hook.hookAtAddress) != 0xE8)
	{
		printf("Hook %s is not on a valid opcode (saw 0x%02x at address 0x%08x)!\n",
			name,
			this->readMemory<BYTE>(hook.hookAtAddress),
			hook.hookAtAddress);
		return 0;
	}

	bool hooked = true;
	this->allowOPCodeModification(hook.hookAtAddress + 1, 4);
	{
		hook.oldFuncOffset = this->readMemory<DWORD>(hook.hookAtAddress + 1);
		if (!this->writeMemory<DWORD>(hook.hookAtAddress + 1, hook.newFuncAddress - hook.hookAtAddress - 5))
		{
			printf("Failed to write memory for hook %s!\n", name);
			hooked = false;
		}
	}
	this->disallowOPCodeModification(hook.hookAtAddress + 1, 4);

	if (!hooked)
		return 0;

	this->callHooks.push_back(hook);
	return hook.getOldFunctionAddress();
}

void ExecutableModule::clearCallHooks()
{
	for (auto hook = this->callHooks.begin(); hook != this->callHooks.end(); hook++)
	{
		this->allowOPCodeModification(hook->hookAtAddress + 1, 4);
		this->writeMemory<DWORD>(hook->hookAtAddress + 1, hook->oldFuncOffset);
		this->disallowOPCodeModification(hook->hookAtAddress + 1, 4);
	}
	this->callHooks.clear();
}

void ExecutableModule::getModuleInformation()
{
	MODULEENTRY32 entry;
	entry.dwSize = sizeof(MODULEENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, NULL);

	if (Module32First(snapshot, &entry) == TRUE)
	{
		while (Module32Next(snapshot, &entry) == TRUE)
		{
			std::wstring binaryPath = entry.szModule;
			if (binaryPath.find(this->name) != std::wstring::npos)
			{
				this->size = (DWORD)entry.modBaseSize;
				this->base = (DWORD)entry.modBaseAddr;
				break;
			}
		}
	}

	CloseHandle(snapshot);
}


void ExecutableModule::allowOPCodeModification(DWORD address, unsigned int size)
{
	this->oldProtect = this->setRegionProtection(address, size, PAGE_EXECUTE_READWRITE);
}
void ExecutableModule::disallowOPCodeModification(DWORD address, unsigned int size)
{
	this->setRegionProtection(address, size, (oldProtect) ? oldProtect : PAGE_WRITECOPY);
}
DWORD ExecutableModule::setRegionProtection(DWORD address, unsigned int size, DWORD protection)
{
	DWORD oldProtection;
	VirtualProtect((LPVOID)address, size, protection, &oldProtection);
	return oldProtection;
}