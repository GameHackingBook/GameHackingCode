#include "AdobeAirHook.h"
#include "AdobeAirHookCallbacks.h"
#include "ExecutableModule.h"
#include "DebugConsole.h"
#include "ThreadLock.h"


AdobeAirHook* AdobeAirHook::instance = NULL;

const char encodePattern[16] = {0x8B, 0xCE, 0xE8, 0xA6, 0xFF, 0xFF, 0xFF, 0x83, 0xF8, 0xFF, 0x74, 0x16, 0x03, 0xF8, 0x3B, 0xBE};
const char decodePattern[12] = {0x8B, 0xCE, 0xE8, 0x7F, 0xF7, 0xFF, 0xFF, 0x83, 0xF8, 0xFF, 0x89, 0x86};


AdobeAirHook::AdobeAirHook()
{
	this->airModule = new ExecutableModule(L"Adobe AIR.dll");
	this->socketLock = new ThreadLock();

	this->socketLock->enter();
	this->socketLock->leave();
}
AdobeAirHook::~AdobeAirHook()
{
	this->socketLock->enter();
	this->socketLock->leave();

	delete this->airModule;
	delete this->socketLock;
}

void AdobeAirHook::execute()
{
	DebugConsole::getInstance();
	do
	{
		if (!this->airModule->isValid())
		{
			printf("invalid module!\n");
			break;
		}

		DWORD encodeAddress = this->airModule->findPattern(encodePattern, 16);
		DWORD decodeAddress = this->airModule->findPattern(decodePattern, 12);
		if (!encodeAddress || !decodeAddress)
		{
			printf("invalid encode/decode address!\n");
			break;
		}
		
		encodeAddress += 2; decodeAddress += 2; //call is 2 bytes past start of each pattern

		encodeHookFunction = this->airModule->addCallHook("encode", encodeAddress, &myEncode);
		if (!encodeHookFunction)
		{
			printf("encode hook failed!\n");
			break;
		}

		decodeHookFunction = this->airModule->addCallHook("decode", decodeAddress, &myDecode);
		if (!encodeHookFunction)
		{
			printf("encode hook failed!\n");
			break;
		}

		printf("hooks installed!\n");
		return; //success
	} while (0);

	//error is here
	this->terminate();
}
void AdobeAirHook::terminate()
{
	this->airModule->clearCallHooks();
	DebugConsole::deleteInstance();
}

DWORD AdobeAirHook::getEncodeHookFunction()
{
	return this->encodeHookFunction;
}

DWORD AdobeAirHook::getDecodeHookFunction()
{
	return this->decodeHookFunction;
}

void AdobeAirHook::encodeHookCallback(const unsigned char* buffer, unsigned int size)
{
	if (size == 0xFFFFFFFF) return;

	this->socketLock->enter();
	DebugConsole::getInstance()->dumpBuffer(buffer, size, "Outgoing packet collected");
	this->socketLock->leave();
}

void AdobeAirHook::decodeHookCallback(const unsigned char* buffer, unsigned int size)
{
	if (size == 0xFFFFFFFF) return;

	this->socketLock->enter();
	DebugConsole::getInstance()->dumpBuffer(buffer, size, "Incoming packet collected");
	this->socketLock->leave();
}
