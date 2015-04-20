#include "DirectXHook.h"
#include "DirectXHookCallbacks.h"
#include "memory.h"

DirectXHook* DirectXHook::instance = NULL;
unsigned char* DirectXHook::originalEndSceneCode = NULL;
DWORD DirectXHook::endSceneAddress = NULL;
LPDIRECT3DDEVICE9 DirectXHook::hookedDevice = NULL;

_reset DirectXHook::origReset = NULL;
_endScene DirectXHook::origEndScene = NULL;
_drawPrimitive DirectXHook::origDrawPrimitive = NULL;
_drawIndexedPrimitive DirectXHook::origDrawIndexedPrimitive = NULL;

bool DirectXHook::hookReady = false;
bool DirectXHook::hookReadyPre = false;


void DirectXHook::initialize()
{
	while (!GetModuleHandleA("d3d9.dll"))
        Sleep(10);

	DirectXHook::endSceneAddress = this->locateEndScene();
	if (DirectXHook::endSceneAddress)
		DirectXHook::originalEndSceneCode = hookWithJump(DirectXHook::endSceneAddress, (DWORD)&endSceneTrampoline);

	while (!DirectXHook::hookReadyPre)
		Sleep(10);

	DirectXHook::hookReady = true;
}

void DirectXHook::addDrawFrameCallback(_drawFrameCallback cb)
{
	if (!DirectXHook::hookReady)
		this->drawFrameCallbacks.push_back(cb);
}
void DirectXHook::addDrawPrimitiveCallback(_drawPrimitiveCallback cb)
{
	if (!DirectXHook::hookReady)
		this->drawPrimitiveCallbacks.push_back(cb);
}
void DirectXHook::addDrawIndexedPrimitiveCallback(_drawIndexedPrimitiveCallback cb)
{
	if (!DirectXHook::hookReady)
		this->drawIndexedPrimitiveCallbacks.push_back(cb);
}

LPDIRECT3DTEXTURE9 DirectXHook::addTexture(std::wstring imagePath)
{
	if (DirectXHook::hookReady)
	{
		LPDIRECT3DTEXTURE9 texture;

		if (D3DXCreateTextureFromFile(this->hookedDevice, imagePath.c_str(), &texture) < 0)
			return NULL;

		return texture;
	}
	return NULL;
}

int DirectXHook::addSpriteImage(std::wstring imagePath)
{
	if (DirectXHook::hookReady)
	{
		LPDIRECT3DTEXTURE9 texture;
		LPD3DXSPRITE sprite;
		D3DSURFACE_DESC desc;

		if (D3DXCreateTextureFromFile(this->hookedDevice, imagePath.c_str(), &texture) < 0)
			return -1;
		if(D3DXCreateSprite(this->hookedDevice, &sprite) < 0)
			return -1;
		texture->GetLevelDesc(0, &desc);

		this->imageBitmaps.push_back(texture);
		this->imageSprites.push_back(sprite);
		this->imageDescriptions.push_back(desc);
		return this->imageBitmaps.size()-1;
	}

	return -1;
}

void DirectXHook::drawText(int x, int y, D3DCOLOR color, const char *text, ...)
{
	RECT rect;
	va_list va_alist;
	char buf[256] = {0};	

	va_start (va_alist, text);
	_vsnprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), text, va_alist);
	va_end (va_alist);

	rect.left=x+1; 
	rect.top=y+1; 
	rect.right=rect.left+1000; 
	rect.bottom=rect.top+1000;

	this->font->DrawTextA(NULL, buf, -1, &rect, 0, D3DCOLOR_ARGB(255, 10, 10, 10));
	rect.left--;
	rect.top--;
	this->font->DrawTextA(NULL, buf, -1, &rect, 0, color);  
}

void DirectXHook::drawSpriteImage(int imageID, int x, int y, int w, int h)
{
	if (imageID >= this->imageBitmaps.size() || imageID < 0)
		return;

	D3DSURFACE_DESC desc = this->imageDescriptions[imageID];
	D3DXVECTOR2 scaleFactor;
	if (h == -1 && w == -1)
	{
		scaleFactor = D3DXVECTOR2(1.0, 1.0);
	}
	else if (h == -1)
	{
		float scale = (float)w / (float)desc.Width;
		scaleFactor = D3DXVECTOR2(scale, scale);
	}
	else if (w == -1)
	{
		float scale = (float)h / (float)desc.Height;
		scaleFactor = D3DXVECTOR2(scale, scale);
	}
	else
		scaleFactor = D3DXVECTOR2((float)w / (float)desc.Width, (float)h / (float)desc.Height);
	
	D3DXMATRIX spriteMatrix;
	D3DXMatrixTransformation2D(&spriteMatrix, NULL, 0, &scaleFactor, NULL, 0, NULL);

	D3DXVECTOR3 imagepos((float)x * (1.0f / scaleFactor.x), (float)y * (1.0f / scaleFactor.y), 0);
	this->imageSprites[imageID]->Begin(D3DXSPRITE_ALPHABLEND);
	this->imageSprites[imageID]->SetTransform(&spriteMatrix);
	this->imageSprites[imageID]->Draw(this->imageBitmaps[imageID], NULL, NULL, &imagepos, 0xFFFFFFFF);
	this->imageSprites[imageID]->End();
}

DWORD DirectXHook::initHookCallback(LPDIRECT3DDEVICE9 device)
{
	DirectXHook::hookedDevice = device;

	while (DirectXHook::originalEndSceneCode == NULL){}
	unhookWithJump(DirectXHook::endSceneAddress, originalEndSceneCode);

	D3DXCreateFont(DirectXHook::hookedDevice, 15, 0, FW_BOLD, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial", &this->font);

	this->placeHooks();
	DirectXHook::hookReadyPre = true;

	return DirectXHook::endSceneAddress;
}

DX_API DirectXHook::resetHookCallback(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	auto result = origReset(pDevice, pPresentationParameters);
	if (result == D3D_OK)
		this->onLostDevice();
	return result;
}
DX_API DirectXHook::endSceneHookCallback(LPDIRECT3DDEVICE9 pDevice)
{
	for (int i = 0; i < drawFrameCallbacks.size(); i++)
		drawFrameCallbacks[i](this, pDevice);

	auto result = origEndScene(pDevice);
	this->placeHooks();
    return result;
}
DX_API DirectXHook::drawPrimitiveHookCallback(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
	for (int i = 0; i < drawPrimitiveCallbacks.size(); i++)
		drawPrimitiveCallbacks[i](this, pDevice, PrimitiveType, StartVertex, PrimitiveCount);
	return origDrawPrimitive(pDevice, PrimitiveType, StartVertex, PrimitiveCount);
}
DX_API DirectXHook::drawIndexedPrimitiveHookCallback(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE PrimType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{
	for (int i = 0; i < drawIndexedPrimitiveCallbacks.size(); i++)
		drawIndexedPrimitiveCallbacks[i](this, pDevice, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);

	return origDrawIndexedPrimitive(pDevice, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
}


void DirectXHook::placeHooks()
{
	static const DWORD VHHookCount = 4;
	static VFHookInfo VFHooks[VHHookCount] = 
	{
		VFHookInfo(16, (DWORD)&myReset, (DWORD*)&DirectXHook::origReset),
		VFHookInfo(42, (DWORD)&myEndScene, (DWORD*)&DirectXHook::origEndScene),
		VFHookInfo(81, (DWORD)&myDrawPrimitive, (DWORD*)&DirectXHook::origDrawPrimitive),
		VFHookInfo(82, (DWORD)&myDrawIndexedPrimitive, (DWORD*)&DirectXHook::origDrawIndexedPrimitive),
	};

	for (int hook = 0; hook < VHHookCount; hook++)
	{
		DWORD ret = hookVF((DWORD)DirectXHook::hookedDevice, VFHooks[hook].index, VFHooks[hook].callback);
		if (ret != VFHooks[hook].callback)
			*VFHooks[hook].origFunc = ret;
	}
}

void DirectXHook::onLostDevice()
{
	if (this->font)
		this->font->OnLostDevice();

	for (int i = 0; i < this->imageSprites.size(); i++)
		this->imageSprites[i]->OnLostDevice();
}


DWORD DirectXHook::locateEndScene()
{
	WNDCLASSEXA wc =
	{
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		DefWindowProc,
		0L,0L,
		GetModuleHandleA(NULL),
		NULL, NULL, NULL, NULL,
		"DX", NULL
	};

    RegisterClassExA(&wc);
    HWND hWnd = CreateWindowA("DX",NULL,WS_OVERLAPPEDWINDOW,100,100,600,600,GetDesktopWindow(),NULL,wc.hInstance,NULL);
   
	LPDIRECT3D9 pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if (!pD3D)
		return 0;

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWnd;
	
    LPDIRECT3DDEVICE9 pd3dDevice;
	HRESULT res = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pd3dDevice);

	if (FAILED(res))
		return 0;

	DWORD EndSceneAddress = getVF((DWORD)pd3dDevice, 42);

	pD3D->Release();
	pd3dDevice->Release();
    DestroyWindow(hWnd);

	return EndSceneAddress;
}