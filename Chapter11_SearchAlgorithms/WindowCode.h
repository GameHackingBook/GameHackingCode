#pragma once

/*
	THIS CODE EXISTS ONLY TO DISPLAY A WINDOW,
	AND IS NOT RELEVANT TO THE EXAMPLE CODE
*/

#include "DummyWindow.h"
#include <Windows.h>
#include <windowsx.h>
#include <memory>
#include <functional>

const static int TILESIZE = 20;
const static int OFFSETX = 0;
const static int OFFSETY = 60;

const static int YSIZE = 20;
const static int XSIZE = 20;

const static int UNBLOCKING_TILE = 0;
const static int BLOCKING_TILE = 1;
const static int START_TILE = 2;
const static int END_TILE = 3;


int map[YSIZE][XSIZE] = {};
int path[YSIZE][XSIZE] = {};

std::function<bool(
		int map[YSIZE][XSIZE],
		int startx, int starty,
		int endx, int endy,
		int path[YSIZE][XSIZE])> searchFunction;

#define DUMMY_SET_MESSAGE_HANDLER(dummy, message, callback) \
	do { dummy->setMessageHandler(message, callback); } while(0)

typedef std::shared_ptr<DummyWindow> DummyWindowSharedPtr;

DummyWindowSharedPtr dummyWindow;


void clearPath()
{
	memset((void*)&path, 0, sizeof(path));
}

LRESULT keyUpHandler(DummyWindow* dummy, UINT message, WPARAM wparam, LPARAM lparam)
{
	if (wparam == VK_RETURN)
	{
		clearPath();
		searchFunction(map, 0, 10, 19, 10, path);
	}
	return DefWindowProc(dummy->getHandle(), message, wparam, lparam);
}
LRESULT mouseUpHandler(DummyWindow* dummy, UINT message, WPARAM wparam, LPARAM lparam)
{
	POINT mousePoint;
	mousePoint.x = GET_X_LPARAM(lparam) - OFFSETX; 
	mousePoint.y = GET_Y_LPARAM(lparam) - OFFSETY;
	mousePoint.x /= TILESIZE;
	mousePoint.y /= TILESIZE;
	
	mousePoint.x = mousePoint.x > XSIZE ? XSIZE : mousePoint.x;
	mousePoint.y = mousePoint.y > YSIZE ? YSIZE : mousePoint.y;
	mousePoint.x = mousePoint.x < 0 ? 0 : mousePoint.x;
	mousePoint.y = mousePoint.y < 0 ? 0 : mousePoint.y;

	if (map[mousePoint.x][mousePoint.y] == BLOCKING_TILE)
		map[mousePoint.x][mousePoint.y] = UNBLOCKING_TILE;
	else if (map[mousePoint.x][mousePoint.y] == UNBLOCKING_TILE)
		map[mousePoint.x][mousePoint.y] = BLOCKING_TILE;

	clearPath();
	return DefWindowProc(dummy->getHandle(), message, wparam, lparam);
}

void paintString(HDC hdc, const wchar_t* string, int x, int y)
{
	TextOut(hdc, x, y, string, wcslen(string));
}

void paintMap(HDC hdc)
{
	static HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
	static HBRUSH redBrush = CreateSolidBrush(RGB(225, 40, 40));
	static HBRUSH greenBrush = CreateSolidBrush(RGB(40, 225, 40));
	static HBRUSH yellowBrush = CreateSolidBrush(RGB(255, 255, 0));
	static HBRUSH greyBrush = CreateSolidBrush(RGB(160, 160, 160));

	paintString(hdc, L"Walls: Black                                  ", 3, 3);
	paintString(hdc, L"Start Pos: Green                               ", 113, 3);
	paintString(hdc, L"End Pos: Red                                  ", 253, 3);

	paintString(hdc, L"Path: Yellow Dots                            ", 3, 22);
	paintString(hdc, L"Closed Nodes: Black Dots                  ", 203, 22);

	paintString(hdc, L"Toggle Wall: Click                          ", 3, 41);
	paintString(hdc, L"Calculate Path: [enter]                     ", 203, 41);

	// draw tiles
	for (int x = 0; x < XSIZE; x++)
	{
		int xStart = OFFSETX + x * TILESIZE;
		for (int y = 0; y < YSIZE; y++)
		{
			auto tile = map[x][y];
			RECT location;
			location.left = OFFSETX + x * TILESIZE;
			location.top = OFFSETY + y * TILESIZE;
			location.right = location.left + TILESIZE;
			location.bottom = location.top + TILESIZE;

			switch (tile)
			{
				case UNBLOCKING_TILE:
					FillRect(hdc, &location, greyBrush);
					break;
				case BLOCKING_TILE:
					FillRect(hdc, &location, blackBrush);
					break;
				case START_TILE:
					FillRect(hdc, &location, greenBrush);
					break;
				case END_TILE:
					FillRect(hdc, &location, redBrush);
					break;
				default:
					break;
			}

		}
		
	}

	// draw path
	for (int x = 0; x < XSIZE; x++)
	{
		int xStart = OFFSETX + x * TILESIZE;
		for (int y = 0; y < YSIZE; y++)
		{
			auto tile = path[x][y];
			RECT location;
			location.left = OFFSETX + x * TILESIZE;
			location.top = OFFSETY + y * TILESIZE;
			location.right = location.left + TILESIZE;
			location.bottom = location.top + TILESIZE;
			location.left += 6;
			location.top += 6;
			location.right -= 6;
			location.bottom -= 6;

			if (tile == 1)
			{
				FillRect(hdc, &location, yellowBrush);
			}
			else if (tile == 2)
			{
				FillRect(hdc, &location, blackBrush);
			}
		}
		
	}

	// draw grid overlay
	for (int x = 0; x < XSIZE; x++)
	{
		int xStart = OFFSETX + x * TILESIZE;
		for (int y = 0; y < YSIZE; y++)
		{
			int yStart = OFFSETY + y * TILESIZE;
			MoveToEx(hdc, OFFSETX, yStart, NULL);
			LineTo(hdc, OFFSETX + XSIZE * TILESIZE, yStart);
		}
		MoveToEx(hdc, xStart + TILESIZE, OFFSETY, NULL);
		LineTo(hdc, xStart + TILESIZE, OFFSETY + YSIZE * TILESIZE);
	}
}

LRESULT paintMessageHandler(DummyWindow* dummy, UINT message, WPARAM wparam, LPARAM lparam)
{
	RECT rect;
	GetClientRect(dummy->getHandle(), &rect);
	int width = rect.right - rect.left;
	int height = rect.bottom + rect.left;

	PAINTSTRUCT ps;
	auto hdc = BeginPaint(dummy->getHandle(), &ps);
	auto secondhdc = CreateCompatibleDC(hdc);
	auto buffer2 = CreateCompatibleBitmap(hdc, width, height);
	SelectObject(secondhdc, buffer2);

	paintMap(secondhdc);

	BitBlt(hdc, 0, 0, width, height, secondhdc, 0, 0, SRCCOPY);
	DeleteObject(buffer2);
	DeleteDC(secondhdc);
	DeleteDC(hdc);
	EndPaint(dummy->getHandle(), &ps);
	return 0;
}
LRESULT timerMessageHandler(DummyWindow* dummy, UINT message, WPARAM wparam, LPARAM lparam)
{
	RedrawWindow(dummy->getHandle(), NULL, NULL, RDW_INVALIDATE);
	return 0;
}

LRESULT closeMessageHandler(DummyWindow* dummy, UINT message, WPARAM wparam, LPARAM lparam)
{
	DestroyWindow(dummy->getHandle());
	return 0;
}
LRESULT destroyMessageHandler(DummyWindow* dummy, UINT message, WPARAM wparam, LPARAM lparam)
{
	PostQuitMessage(0);
	return 0;
}

LRESULT setCursorHandler(DummyWindow* dummy, UINT message, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(dummy->getHandle(), message, wparam, lparam);
}


void showWindow()
{
	memset((void*)&map, 0, sizeof(map));
	memset((void*)&path, 0, sizeof(path));

	map[0][10] = START_TILE;
	map[19][10] = END_TILE;

	dummyWindow = DummyWindowSharedPtr(new DummyWindow(NULL));
	dummyWindow->initialize();

	DUMMY_SET_MESSAGE_HANDLER(dummyWindow, WM_KEYUP,     keyUpHandler);
	DUMMY_SET_MESSAGE_HANDLER(dummyWindow, WM_LBUTTONUP, mouseUpHandler);
	DUMMY_SET_MESSAGE_HANDLER(dummyWindow, WM_PAINT,     paintMessageHandler);
	DUMMY_SET_MESSAGE_HANDLER(dummyWindow, WM_TIMER,     timerMessageHandler);
	DUMMY_SET_MESSAGE_HANDLER(dummyWindow, WM_CLOSE,     closeMessageHandler);
	DUMMY_SET_MESSAGE_HANDLER(dummyWindow, WM_DESTROY,   destroyMessageHandler);
	DUMMY_SET_MESSAGE_HANDLER(dummyWindow, WM_SETCURSOR, setCursorHandler);

	dummyWindow->doMessageLoop();
}