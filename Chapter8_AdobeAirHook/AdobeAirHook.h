#pragma once
#include <Windows.h>

class ExecutableModule;
class ThreadLock;
class PacketCollector;

class AdobeAirHook
{
public:
	static AdobeAirHook* getInstance()
	{
		if (!AdobeAirHook::instance)
			AdobeAirHook::instance = new AdobeAirHook();
		return AdobeAirHook::instance;
	}
	static void deleteInstance()
	{
		if (AdobeAirHook::instance)
		{
			delete AdobeAirHook::instance;
			AdobeAirHook::instance = NULL;
		}
	}

	void execute();
	void terminate();

	DWORD getEncodeHookFunction();
	DWORD getDecodeHookFunction();
	void encodeHookCallback(const unsigned char* buffer, unsigned int size);
	void decodeHookCallback(const unsigned char* buffer, unsigned int size);

private:
	AdobeAirHook();
	~AdobeAirHook();

	static AdobeAirHook* instance;

	ExecutableModule* airModule;
	ThreadLock* socketLock;

	DWORD encodeHookFunction, decodeHookFunction;
};