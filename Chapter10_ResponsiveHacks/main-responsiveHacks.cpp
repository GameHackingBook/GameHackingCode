#include <iostream>
#include <Windows.h>
#include <vector>

// WARNING: if this code is killed mid-execute or you switch to another window (not the console)
// while it's executing, it may cause the system to think a modifier key is stuck.
// If this happens, you can tap shift, ctrl, and alt on the LEFT side of your keyboard to "unstick it"





// SendInput() example code
	void sendKeyWithSendInput(WORD key, bool up)
	{
		INPUT input = {0};
		input.type = INPUT_KEYBOARD;
		input.ki.wVk = key;
		input.ki.dwFlags = 0;

		if (up)
			input.ki.dwFlags |= KEYEVENTF_KEYUP;
		SendInput(1, &input, sizeof(input));
	}

	void sendModifiersWithSendInput(DWORD flags, bool up)
	{
		if (flags & 1)
			sendKeyWithSendInput(VK_LSHIFT, up);
		if (flags & 2)
			sendKeyWithSendInput(VK_LCONTROL, up);
		if (flags & 4)
			sendKeyWithSendInput(VK_LMENU, up);
	}

	void sendCharWithSendInput(char letter)
	{
		SHORT keyFlags = VkKeyScanA(letter);

		WORD key = keyFlags & 0xFF;
		DWORD flags = (keyFlags >> 8) & 0xFF;

		sendModifiersWithSendInput(flags, false);

		sendKeyWithSendInput(key, false);
		sendKeyWithSendInput(key, true);
	
		sendModifiersWithSendInput(flags, true);
	}

	void typeStringWithSendInput(const char* string)
	{
		for (int i = 0; i < strlen(string); i++)
		{
			sendCharWithSendInput(string[i]);
			Sleep(80);
		}
	}

// SendMessage example code
	void sendKeyWithSendMessage(HWND window, WORD key, char letter)
	{
		SendMessageA(window, WM_KEYDOWN, key, 0);
		if (letter != 0)
			SendMessageA(window, WM_CHAR, letter, 1);
		SendMessageA(window, WM_KEYUP, key, 1);
	}


	void sendCharWithSendMessage(HWND window, char letter)
	{
		SHORT keyFlags = VkKeyScanA(letter);

		WORD key = keyFlags & 0xFF;
		DWORD flags = (keyFlags >> 8) & 0xFF;

		sendKeyWithSendMessage(window, key, letter);
	}

	void typeStringWithSendMessage(HWND window, const char* string)
	{
		for (int i = 0; i < strlen(string); i++)
		{
			sendCharWithSendMessage(window, string[i]);
			Sleep(80);
		}
	}




DWORD WINAPI exampleThread(LPVOID lpParam)
{
	Sleep(500);
	typeStringWithSendInput("Typing using SendInput()!\rHow does it look? :)\r\r");

	auto window = FindWindowA(NULL, "Chapter10 Input Example");
	typeStringWithSendMessage(window, "Typing using SendMessage()!\rEffectively the same, but more powerful :-)\r\r");

	std::cout << std::endl << std::endl << "DONE! you can close this now!" << std::endl;

	return 0;
}


int main(void)
{
	std::cout << "WARNING: Don't switch between windows until this application is done typing" << std::endl;
	system("pause");
	std::cout << "Everything below here is being programmatically typed using the keyboard" << std::endl << std::endl;

	SetConsoleTitleA("Chapter10 Input Example");

	CreateThread(NULL, 0, exampleThread, 0, 0, NULL);

	char temp;
	while (true) {
		std::cin >> temp;
	}
}