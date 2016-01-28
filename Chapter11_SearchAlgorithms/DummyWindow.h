#pragma once

/*
	THIS CODE EXISTS ONLY TO DISPLAY A WINDOW,
	AND IS NOT RELEVANT TO THE EXAMPLE CODE
*/

#include <windows.h>
#include <windowsx.h>
#include <map>
#include <functional>
#include <stdint.h>

class DummyWindow;
typedef std::function<LRESULT(DummyWindow*, UINT, WPARAM, LPARAM)> windowMessageCallbackType;

class DummyWindow
{
public:
	DummyWindow(HINSTANCE _instance);
	~DummyWindow(void);

	bool initialize();
	void finalize();
	int32_t doMessageLoop();
	void setMessageHandler(UINT message, windowMessageCallbackType callback);

	HWND getHandle() { return this->windowHandle; }

private:
	static std::map<HWND, DummyWindow*> windowMap;

	static void registerWindow(DummyWindow* dummy, HWND window);
	static void unregisterWindow(DummyWindow* dummy, HWND window);
	static DummyWindow* findWindow(HWND window);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


private:
	std::map<UINT, windowMessageCallbackType> callbackMap;
	HWND windowHandle;
	HINSTANCE instance;

	LRESULT onMessageReceived(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
};

