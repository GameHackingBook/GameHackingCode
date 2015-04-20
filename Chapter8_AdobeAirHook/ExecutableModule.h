#pragma once
#include <Windows.h>
#include <list>

class ExecutableModule
{
public:
	ExecutableModule(const wchar_t* moduleName);
	~ExecutableModule();

	DWORD findPattern(const char* pattern, unsigned int patternLength, unsigned int occurance = 1);
	DWORD addCallHook(const char* name, DWORD address, LPVOID function);
	void clearCallHooks();

	bool isValid() { return this->base > 0 && this->size > 0; }

	DWORD getBase() { return this->base; }
	DWORD getSize() { return this->size; }

	template <class T>
	T* pointMemory(DWORD address)
	{
		return (T*)address;
	}

	template <class T>
	T readMemory(DWORD address)
	{
		if (address < this->base || address > (this->base + this->size))
		{
			T ret;
			memset(&ret, 0, sizeof(T));
			return ret;
		}
		return *(T*)address;
	}

	template <class T>
	bool writeMemory(DWORD address, T value)
	{
		if (address < this->base || address > (this->base + this->size))
			return false;
		*(T*)(address) = value;
		return true;
	}

private:
	struct callHookInformation
	{
		const char* name;
		DWORD hookAtAddress;
		DWORD newFuncAddress;
		DWORD oldFuncOffset;

		DWORD getOldFunctionAddress()
		{
			return hookAtAddress + oldFuncOffset + 5;
		}
	};

	const wchar_t* name;
	DWORD base, size, oldProtect;
	std::list<callHookInformation> callHooks;

	void getModuleInformation();

	void allowOPCodeModification(DWORD address, unsigned int size);
	void disallowOPCodeModification(DWORD address, unsigned int size);
	DWORD setRegionProtection(DWORD address, unsigned int size, DWORD protection);
};

