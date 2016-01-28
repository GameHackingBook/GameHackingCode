#include "DummyWindow.h"

/*
	THIS CODE EXISTS ONLY TO DISPLAY A WINDOW,
	AND IS NOT RELEVANT TO THE EXAMPLE CODE
*/

std::map<HWND, DummyWindow*> DummyWindow::windowMap;

DummyWindow::DummyWindow(HINSTANCE _instance) :  windowHandle(NULL), instance(_instance)
{}

DummyWindow::~DummyWindow(void)
{
	DummyWindow::unregisterWindow(this, this->windowHandle);
}

bool DummyWindow::initialize()
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = this->instance;
	wcex.hIcon          = NULL; //LoadIcon(this->instance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName  = L"gamhakwind";
	wcex.hIconSm        = NULL; //LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	if (!RegisterClassEx(&wcex))
	{
		auto err = GetLastError();
		return false;
	}

	this->windowHandle = CreateWindowA
	(
		"gamhakwind",
		"Game Hacking - Chapter 11 - SearchAlgorithms",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		420, 490,
		NULL,
		NULL,
		this->instance,
		NULL
	);
	if (this->windowHandle == NULL)
	{
		auto err = GetLastError();
		//return false;
	}

	DummyWindow::registerWindow(this, this->windowHandle);

	SetTimer(this->windowHandle, NULL, 50, NULL);

	ShowWindow(this->windowHandle, SW_SHOW);
	UpdateWindow(this->windowHandle);
	return true;
}

void DummyWindow::finalize()
{
	this->callbackMap.clear();
}

int32_t DummyWindow::doMessageLoop()
{
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return static_cast<int32_t>(msg.wParam);
}

void DummyWindow::setMessageHandler(UINT message, windowMessageCallbackType callback)
{
	this->callbackMap[message] = callback;
}

LRESULT DummyWindow::onMessageReceived(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	auto found = this->callbackMap.find(message);
	if (found != this->callbackMap.end())
		return found->second(this, message, wparam, lparam);
	return DefWindowProc(window, message, wparam, lparam);
}

void DummyWindow::registerWindow(DummyWindow* dummy, HWND window)
{
	DummyWindow::windowMap[window] = dummy;
}
void DummyWindow::unregisterWindow(DummyWindow* dummy, HWND window)
{
	auto found = DummyWindow::windowMap.find(window);
	if (found != DummyWindow::windowMap.end())
		DummyWindow::windowMap.erase(found);
}
DummyWindow* DummyWindow::findWindow(HWND window)
{
	auto found = DummyWindow::windowMap.find(window);
	if (found != DummyWindow::windowMap.end())
		return found->second;
	return nullptr;
}

LRESULT DummyWindow::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto dummy = DummyWindow::findWindow(hWnd);
	if (dummy)
		return dummy->onMessageReceived(hWnd, uMsg, wParam, lParam);
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}