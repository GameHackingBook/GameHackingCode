#include "main.h"

// This is a dummy base-class
class someBaseClass
{
	public:
		virtual DWORD someFunction(DWORD arg1) { return 0; }
};

// This is the class for which we'll
// actually hook the VF table. It inherits
// the dummy base-class to ensure that
// someFunction() is in a virtual table.
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


// This is where we re-direct the VF calls to
DWORD originalVFFunction;
DWORD __stdcall someNewVFFunction(DWORD arg1)
{
	// notice how we take ECX and store it in a variable..
	// this is done because it stores the class instance pointer ("this")
	// and we want to make sure our code doesn't overwrite it (the compiler
	// doesn't understand that this is a VF hook, so it may think ECX is 
	// free for it to use)

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

// This is the function that actually places the hook
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

// This is the function that ties everything together
void VFHookExample()
{
	someClass* inst = new someClass();

	originalVFFunction = hookVF((DWORD)inst, 0, (DWORD)&someNewVFFunction);
	inst->someFunction(0);
	delete inst;
}