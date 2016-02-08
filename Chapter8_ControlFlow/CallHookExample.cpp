#include "main.h"

// the function thaat we're going to hook
DWORD functionToBeHooked(DWORD arg1, DWORD arg2, DWORD arg3)
{
	if (arg1 == arg2 && arg2 == 3 && arg3 == 4)
		printf("Call hook worked! Parameters intercepted and changed!\n");
	else
		printf("Call hook failed!\n");
	return 0;
}

// the hook will replace the call in this function
// to call our hook
void whereHookGoes()
{
	functionToBeHooked(0, 0, 0);
}

// I cannot hard-code any addresses from within this application,
// as they may change upon re-compile. For this reason, I'll locate
// the address of the CALL that I want to replace programatically
// by scanning for the 'CALL' statement (bytes 0xE8)
// somewhere within 1000 bytes of the start of the function
DWORD getAddressForCallHook(DWORD functionStart)
{
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


// this our our type-def that minmics the function type
// of the function being hooked. This allows us to use a 
// clean call to the original function just knowing it's address
typedef DWORD (__cdecl _origFunc)(DWORD arg1, DWORD arg2, DWORD arg3);
_origFunc* originalFunction;

// this is the function we re-direct the CALL to.
// it simply throws out the orignal parameters
// and passes new ones to the original function
DWORD __cdecl someNewFunction(DWORD arg1, DWORD arg2, DWORD arg3)
{
	return originalFunction(3, 3, 4);
}

// this function is what actually places the hook
DWORD callHook(DWORD hookAt, DWORD newFunc)
{
	DWORD newOffset = newFunc - hookAt - 5;

	auto oldProtection = protectMemory<DWORD>(hookAt + 1, PAGE_EXECUTE_READWRITE);

	DWORD originalOffset = readMemory<DWORD>(hookAt + 1);
	writeMemory<DWORD>(hookAt + 1, newOffset);
	protectMemory<DWORD>(hookAt + 1, oldProtection);

	return originalOffset + hookAt + 5;
}

// This ties the entire example together
void callHookExample()
{
	auto address = getAddressForCallHook((DWORD)&whereHookGoes);
	if (address)
		originalFunction = (_origFunc*)callHook(address, (DWORD)&someNewFunction);

	whereHookGoes();
}