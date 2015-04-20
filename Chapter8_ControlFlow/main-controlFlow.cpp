#include <iostream>
#include <Windows.h>
#include <vector>


//helpers
	template<typename T>
	T readMemory(DWORD address)
	{
		return *((T*)address);
	}

	template<typename T>
	T* pointMemory(DWORD address)
	{
		return ((T*)address);
	}

	template<typename T>
	void writeMemory(DWORD address, T value)
	{
		*((T*)address) = value;
	}

	template<typename T>
	DWORD protectMemory(DWORD address, DWORD prot)
	{
		DWORD oldProt;
		VirtualProtect((LPVOID)address, sizeof(T), prot, &oldProt);
		return oldProt;
	}



// NOP example code
	struct _creature
	{
		_creature(int hb, bool e, bool c) : healthBar(hb), isEnemy(e), isCloaked(c)  {}
		int healthBar;
		bool isEnemy, isCloaked;
	};
	std::vector<_creature> creatures;
	int creaturesDrawn = 0;

	void drawHealthBar(int healthbar) 
	{
		creaturesDrawn++; // make a note that we drew it
		Sleep(healthbar); // just an example, we're not really doing anything
	}
	void drawCreatureHealthBarExample()
	{
		for (int i = 0; i < creatures.size(); i++) {
			auto c = creatures[i];
			if (c.isEnemy && c.isCloaked) continue;
			drawHealthBar(c.healthBar);
		}
	}

	template<int SIZE>
	void writeNop(DWORD address)
	{
		auto oldProtection = protectMemory<BYTE[SIZE]>(address, PAGE_EXECUTE_READWRITE);
		for (int i = 0; i < SIZE; i++)
			writeMemory<BYTE>(address + i, 0x90);
		protectMemory<BYTE[SIZE]>(address, oldProtection);
	}

	DWORD getAddressForNOP(DWORD functionStart)
	{
		// i cannot hard-code any addresses from within this application,
		// as they may change upon re-compile. For this reason, I'll locate
		// the address of the JMP that I want to replace programatically
		// by scanning for the 'JMP -67' statement (bytes 0xEB 0xBD)
		// somewhere within 1000 bytes of the start of the function

		

		auto oldProtection = protectMemory<BYTE[1000]>(functionStart, PAGE_EXECUTE_READ); // make sure memory is readable, just incase
		auto mem = pointMemory<BYTE>(functionStart);

		DWORD ret = 0;
		for (int i = 0; i < 999; i++) {
			if (mem[i] == 0xEB && mem[i+1] == 0xBD) {
				ret = functionStart + i;
				break;
			}
		}
		protectMemory<BYTE[1000]>(functionStart, oldProtection); // restore old memory protection
		return ret;
	}

	void NOPExample()
	{
		// nop it
		auto address = getAddressForNOP((DWORD)&drawCreatureHealthBarExample);
		if (address)
			writeNop<2>(address);

		// add some make creatures
		creatures.push_back(_creature(0, true, true));
		creatures.push_back(_creature(0, true, false));
		creatures.push_back(_creature(0, false, true));
		creatures.push_back(_creature(0, false, false));

		//call the function
		drawCreatureHealthBarExample();

		//check if NOP worked
		if (creaturesDrawn == 4)
			printf("NOP worked! Drew all creatures!\n");
		else
			printf("NOP failed! :( Only drew %d/4 creatures.\n", creaturesDrawn);
	}


// call hook
	DWORD functionToBeHooked(DWORD arg1, DWORD arg2, DWORD arg3)
	{
		if (arg1 == arg2 && arg2 == 3 && arg3 == 4)
			printf("Call hook worked! Parameters intercepted and changed!\n");
		else
			printf("Call hook failed!\n");
		return 0;
	}

	void whereHookGoes()
	{
		functionToBeHooked(0, 0, 0);
	}

	DWORD getAddressForCallHook(DWORD functionStart)
	{
		// same story as with NOP, except we're looking for the first CALL (0xE8)
		auto oldProtection = protectMemory<BYTE[1000]>(functionStart, PAGE_EXECUTE_READ); // make sure memory is readable, just incase
		auto mem = pointMemory<BYTE>(functionStart);

		DWORD ret = 0;
		for (int i = 0; i < 1000; i++) {
			if (mem[i] == 0xE8) {
				ret = functionStart + i;
				break;
			}
		}
		protectMemory<BYTE[1000]>(functionStart, oldProtection); // restore old memory protection
		return ret;
	}

	
	typedef DWORD (__cdecl _origFunc)(DWORD arg1, DWORD arg2, DWORD arg3);
	_origFunc* originalFunction;

	DWORD __cdecl someNewFunction(DWORD arg1, DWORD arg2, DWORD arg3)
	{
		return originalFunction(3, 3, 4);
	}


	DWORD callHook(DWORD hookAt, DWORD newFunc)
	{
		DWORD newOffset = newFunc - hookAt - 5;

		auto oldProtection = protectMemory<DWORD>(hookAt + 1, PAGE_EXECUTE_READWRITE);

		DWORD originalOffset = readMemory<DWORD>(hookAt + 1);
		writeMemory<DWORD>(hookAt + 1, newOffset);
		protectMemory<DWORD>(hookAt + 1, oldProtection);

		return originalOffset + hookAt + 5;
	}

	void callHookExample()
	{
		auto address = getAddressForCallHook((DWORD)&whereHookGoes);
		if (address)
			originalFunction = (_origFunc*)callHook(address, (DWORD)&someNewFunction);

		whereHookGoes();
	}


// vf table hook
	class someBaseClass
	{
		public:
			virtual DWORD someFunction(DWORD arg1) { return 0; }
	};
	class someClass : public someBaseClass
	{
		public:
			virtual DWORD someFunction(DWORD arg1)
			{
				if (arg1 == 1)
					printf("    VF Table hook worked! Parameters intercepted and changed!\n");
				else
					printf("    VF Table hook failed!\n");
				return 0;
			}
	};


	DWORD originalVFFunction;
	DWORD __stdcall someNewVFFunction(DWORD arg1)
	{
		static DWORD _this, _ret;
		__asm MOV _this, ECX
		printf("VFHook pre\n");
		__asm {
			PUSH 1
			MOV ECX, _this
			CALL [originalVFFunction]
			MOV _ret, EAX
		}
		printf("VFHook Post\n");
		__asm MOV ECX, _this
		return _ret;
	}


	DWORD hookVF(DWORD classInst, DWORD funcIndex, DWORD newFunc)
	{
		DWORD VFTable = readMemory<DWORD>(classInst);
		DWORD hookAddress = VFTable + funcIndex * sizeof(DWORD);

		auto oldProtection = protectMemory<DWORD>(hookAddress, PAGE_READWRITE);
		DWORD originalFunc = readMemory<DWORD>(hookAddress);
		writeMemory<DWORD>(hookAddress, newFunc);
		protectMemory<DWORD>(hookAddress, oldProtection);

		return originalFunc;
	}

	void VFHookExample()
	{
		someClass* inst = new someClass();

		originalVFFunction = hookVF((DWORD)inst, 0, (DWORD)&someNewVFFunction);
		inst->someFunction(0);
		delete inst;
	}

// iat hook
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

	typedef VOID (WINAPI _origSleep)(DWORD ms);
	_origSleep* originalSleep;

	VOID WINAPI newSleepFunction(DWORD ms)
	{
		if (ms > 100)
			printf("Sleep hook worked! Denied sleep for %d miliseconds.\n", ms);
		else
			originalSleep(ms);
	}

	void IATHookExample()
	{
		originalSleep = (_origSleep*)hookIAT("Sleep", (DWORD)&newSleepFunction);
		Sleep(1234);
	}


int main(void)
{

	// due to differences in the way functions are compiled between
	// DEBUG and RELEASE builds, this example will only work in RELEASE.
	// This is only because of the way I'm finding the address to be NOP'd
	NOPExample();
	callHookExample();
	VFHookExample();
	IATHookExample();

	while (true) // stay busy
	{
		Sleep(100);
	}
}