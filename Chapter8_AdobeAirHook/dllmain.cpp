#include <Windows.h>
#include "AdobeAirHook.h"
AdobeAirHook* hook;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		AdobeAirHook::getInstance()->execute();
		break;
	case DLL_PROCESS_DETACH:
		AdobeAirHook::getInstance()->terminate();
		AdobeAirHook::deleteInstance();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

