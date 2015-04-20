#include "DirectXHook.h"

int primitivesDrawn = 0;
int gliderImage = 0;
bool initialized = false;


////////////////// CHAPTER 9 LIGTHACK STUFF //////////////////
bool lightHack = false;
void enableLightHackDirectional(LPDIRECT3DDEVICE9 pDevice)
{
    D3DLIGHT9 light;

    ZeroMemory(&light, sizeof(light));
    light.Type = D3DLIGHT_DIRECTIONAL;
    light.Diffuse = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
    light.Direction = D3DXVECTOR3(-1.0f, -0.5f, -1.0f);

    pDevice->SetLight(0, &light);
    pDevice->LightEnable(0, TRUE);

	lightHack = true;
}

void enableLightHackAmbient(LPDIRECT3DDEVICE9 pDevice)
{
    pDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(100, 100, 100));
	lightHack = true;
}

void lightHackFrame(DirectXHook* hook, LPDIRECT3DDEVICE9 pDevice)
{
	if (!lightHack)
	{
		if (GetAsyncKeyState(VK_F1))
			enableLightHackDirectional(pDevice);
		else if (GetAsyncKeyState(VK_F2))
			enableLightHackAmbient(pDevice);
	}

	if (!lightHack)
		hook->drawText(10, 188, D3DCOLOR_ARGB(255, 255, 0, 0), "There is currently no lighting. To enable light hack, press F1 for directional or F2 for ambient.");
	else
		hook->drawText(10, 188, D3DCOLOR_ARGB(255, 255, 0, 0), "Lighthack has been enabled, you should see the cube clearly now.");
}


////////////////// CHAPTER 9 WALLHACK STUFF ////////////////// 
bool wallHack = false;
LPDIRECT3DTEXTURE9 redTexture = NULL;
void wallHackFrame(DirectXHook* hook, LPDIRECT3DDEVICE9 pDevice)
{
	if (!wallHack && GetAsyncKeyState(VK_F3))
		wallHack = true;

	if (!wallHack)
		hook->drawText(10, 203, D3DCOLOR_ARGB(255, 255, 0, 0), "Wallhack example isnt running! Press F3 to enable.");
	else
		hook->drawText(10, 203, D3DCOLOR_ARGB(255, 255, 0, 0), "Wallhack is enabled! The cube should be drawn in red with no z-buffering now!");
}

void onDrawIndexedPrimitive(DirectXHook* hook, LPDIRECT3DDEVICE9 device, D3DPRIMITIVETYPE primType, INT baseVertexIndex, UINT minVertexIndex, UINT numVertices, UINT startIndex, UINT primCount)
{
	primitivesDrawn++;

	if (wallHack && numVertices == 24 && primCount == 12)
	{
		device->SetRenderState(D3DRS_ZENABLE, false);
		if (redTexture) device->SetTexture(0, redTexture);
		DirectXHook::origDrawIndexedPrimitive(device, primType, baseVertexIndex, minVertexIndex, numVertices, startIndex, primCount);
		device->SetRenderState(D3DRS_ZENABLE, true);		
	}
}


////////////////// CHAPTER 8 STUFF ////////////////// 
void initialize(LPDIRECT3DDEVICE9 pDevice)
{
	gliderImage = DirectXHook::getInstance()->addSpriteImage(L"glider.png");
	redTexture = DirectXHook::getInstance()->addTexture(L"red.png"); // CHAPTER 9 WALLHACK STUFF

	initialized = true;
}

void onDrawFrame(DirectXHook* hook, LPDIRECT3DDEVICE9 pDevice)
{
	if (!initialized) initialize(pDevice);

	hook->drawText(10, 10, D3DCOLOR_ARGB(255, 255, 0, 0), "Direct3D hook working! Intercepted drawing of %d primitives!", primitivesDrawn);
	hook->drawText(10, 25, D3DCOLOR_ARGB(255, 255, 0, 0), "Image drawn by hook:");
	hook->drawSpriteImage(gliderImage, 10, 40, 128, 128);

	lightHackFrame(hook, pDevice); // CHAPTER 9 LIGTHACK STUFF
	wallHackFrame(hook, pDevice); // CHAPTER 9 WALLHACK STUFF

	primitivesDrawn = 0;
}

void onDrawPrimitive(DirectXHook* hook, LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
	primitivesDrawn++;
}


DWORD WINAPI LoopFunction(LPVOID lpParam)
{
	DirectXHook::getInstance()->addDrawFrameCallback(&onDrawFrame);
	DirectXHook::getInstance()->addDrawPrimitiveCallback(&onDrawPrimitive);
	DirectXHook::getInstance()->addDrawIndexedPrimitiveCallback(&onDrawIndexedPrimitive);
	DirectXHook::getInstance()->initialize();

    return 0;
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpvReserved)
{
    if(dwReason == DLL_PROCESS_ATTACH)
	{
        CreateThread(0, 0, LoopFunction, 0, 0, 0);
    }
    else if(dwReason == DLL_PROCESS_DETACH)
	{

    }

    return TRUE;
}