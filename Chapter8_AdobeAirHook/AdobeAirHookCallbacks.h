#include "AdobeAirHook.h"
#include <Windows.h>
#include <iostream>


DWORD __stdcall reportEncode(const unsigned char* buffer, unsigned int size, unsigned int loopCounter)
{
	if (loopCounter == 0)
		AdobeAirHook::getInstance()->encodeHookCallback(buffer, size);
	return AdobeAirHook::getInstance()->getEncodeHookFunction();
}

DWORD __stdcall getDecode()
{
	return AdobeAirHook::getInstance()->getDecodeHookFunction();
}
void __stdcall reportDecode(const unsigned char* buffer, unsigned int size)
{
	AdobeAirHook::getInstance()->decodeHookCallback(buffer, size);
}

void __declspec(naked) myEncode()
{
	__asm
	{
		MOV EAX, DWORD PTR SS:[ESP + 0x4] // get buffer
		MOV EDX, DWORD PTR DS:[ESI + 0x3C58] // get full size
		
		PUSH ECX // store ecx

		PUSH EDI // push current pos
		PUSH EDX // push size
		PUSH EAX // push buffer
		CALL reportEncode // report the encode call

		POP ECX // restore ecx

		JMP EAX // jump to original function (returned by reportEncode)
	}
}

void __declspec(naked) myDecode()
{
	__asm
	{
		MOV EAX, DWORD PTR SS:[ESP + 0x4] // get second arg
		MOV EDX, DWORD PTR SS:[ESP + 0x8] // get first arg

		PUSH EDX // re-push first arg ------------------.
		PUSH EAX // re-push second arg -----------------|
		PUSH ECX // store ecx -----------------------.  |
		CALL getDecode // get the function to call   |  |
		POP ECX // restore ecx ----------------------'  |
		CALL EAX // call the original function ---------'


		MOV EDX, DWORD PTR SS:[ESP + 0x4] // get first arg, its the buffer now

		PUSH EAX // store eax ----------------------------.
		PUSH ECX // store ecx --------------------------. |
		PUSH EAX // push the size -------------------.  | |
		PUSH EDX // push the buffer -----------------|  | |
		CALL reportDecode // report the results now -'  | |
		POP ECX // restore ecx -------------------------' |
		POP EAX // restore eax ---------------------------'
		RETN 8 // return
	}
}