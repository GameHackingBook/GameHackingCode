#include "DirectXHook.h"


DWORD __stdcall reportInitEndScene(LPDIRECT3DDEVICE9 discoveredDeviceAddress)
{
	return DirectXHook::getInstance()->initHookCallback(discoveredDeviceAddress);
}
__declspec(naked) void endSceneTrampoline()
{
	__asm
	{
		MOV EAX, DWORD PTR SS:[ESP + 0x4]
		PUSH EAX
		CALL reportInitEndScene
		JMP EAX
	}
}


DX_API myReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	return DirectXHook::getInstance()->resetHookCallback(pDevice, pPresentationParameters);
}

DX_API myEndScene(LPDIRECT3DDEVICE9 pDevice)
{
	return DirectXHook::getInstance()->endSceneHookCallback(pDevice);
}

DX_API myDrawPrimitive(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
	return DirectXHook::getInstance()->drawPrimitiveHookCallback(pDevice, PrimitiveType, StartVertex, PrimitiveCount); 
}

DX_API myDrawIndexedPrimitive(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE PrimType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{
	return DirectXHook::getInstance()->drawIndexedPrimitiveHookCallback(pDevice, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
}