#pragma once

#pragma once
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#include <stdio.h>
#include <fstream>

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include <vector>

#define DX_API HRESULT WINAPI

typedef HRESULT (WINAPI* _reset)(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
typedef HRESULT (WINAPI* _endScene)(LPDIRECT3DDEVICE9 pDevice);
typedef HRESULT (WINAPI* _drawPrimitive)(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount);
typedef HRESULT (WINAPI* _drawIndexedPrimitive)(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE PrimType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount);


class DirectXHook;
typedef void (*_drawFrameCallback)(DirectXHook* hook, LPDIRECT3DDEVICE9 pDevice);
typedef void (*_drawPrimitiveCallback)(DirectXHook* hook, LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount);
typedef void (*_drawIndexedPrimitiveCallback)(DirectXHook* hook, LPDIRECT3DDEVICE9 device, D3DPRIMITIVETYPE primType, INT baseVertexIndex, UINT minVertexIndex, UINT numVertices, UINT startIndex, UINT primCount);

class DirectXHook
{
public:
	static _reset origReset;
	static _endScene origEndScene;
	static _drawPrimitive origDrawPrimitive;
	static _drawIndexedPrimitive origDrawIndexedPrimitive;

	static DirectXHook* getInstance()
	{
		if (!DirectXHook::instance)
			DirectXHook::instance = new DirectXHook();
		return DirectXHook::instance;
	}
	static void deleteInstance()
	{
		if (DirectXHook::instance)
		{
			delete DirectXHook::instance;
			DirectXHook::instance = NULL;
		}
	}
	void initialize();

	void addDrawFrameCallback(_drawFrameCallback cb);
	void addDrawPrimitiveCallback(_drawPrimitiveCallback cb);
	void addDrawIndexedPrimitiveCallback(_drawIndexedPrimitiveCallback cb);

	LPDIRECT3DTEXTURE9 addTexture(std::wstring imagePath);
	int addSpriteImage(std::wstring imagePath);

	void drawText(int x, int y, D3DCOLOR color, const char *text, ...);
	void drawSpriteImage(int imageID, int x, int y, int w, int h);

	DWORD initHookCallback(LPDIRECT3DDEVICE9 device);
	DX_API resetHookCallback(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
	DX_API endSceneHookCallback(LPDIRECT3DDEVICE9 pDevice);
	DX_API drawPrimitiveHookCallback(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount);
	DX_API drawIndexedPrimitiveHookCallback(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE PrimType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount);


private:
	DirectXHook(void) {};
	~DirectXHook(void) {};


	static DirectXHook* instance;
	static unsigned char* originalEndSceneCode;
	static DWORD endSceneAddress;
	static LPDIRECT3DDEVICE9 hookedDevice;

	static bool hookReadyPre, hookReady;

	std::vector<_drawFrameCallback> drawFrameCallbacks;
	std::vector<_drawPrimitiveCallback> drawPrimitiveCallbacks;
	std::vector<_drawIndexedPrimitiveCallback> drawIndexedPrimitiveCallbacks;

	std::vector<LPDIRECT3DTEXTURE9> imageBitmaps;
	std::vector<D3DSURFACE_DESC> imageDescriptions;
	std::vector<LPD3DXSPRITE> imageSprites;

	LPD3DXFONT font;

	void onLostDevice();

	void placeHooks();
	DWORD locateEndScene();


	struct VFHookInfo
	{
		VFHookInfo(DWORD _index, DWORD cb, DWORD* _origFunc) : index(_index), callback(cb), origFunc(_origFunc) {}
		DWORD index, callback;
		DWORD* origFunc;
	};
};

