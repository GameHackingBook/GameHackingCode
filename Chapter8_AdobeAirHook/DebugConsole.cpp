#include "DebugConsole.h"

#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>

DebugConsole* DebugConsole::instance = NULL;;

DebugConsole::DebugConsole()
{
	this->show();
}
DebugConsole::~DebugConsole()
{
	this->hide();
}

void DebugConsole::show()
{
        using namespace std;

		auto out = freopen("C:\\leaguelog.txt","w",stdout);

		return;

        static const WORD MAX_CONSOLE_LINES = 5000;

        int hConHandle;
        long lStdHandle;
        CONSOLE_SCREEN_BUFFER_INFO coninfo;
        FILE *fp;

        // allocate a console for this app
        AllocConsole();

        // set the screen buffer to be big enough to let us scroll text
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),     &coninfo);
        coninfo.dwSize.Y = MAX_CONSOLE_LINES;
        SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),     coninfo.dwSize);

        // redirect unbuffered STDOUT to the console
        lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
        hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
        fp = _fdopen( hConHandle, "w" );
        *stdout = *fp;
        setvbuf( stdout, NULL, _IONBF, 0 );

        // redirect unbuffered STDIN to the console
        lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
        hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
        fp = _fdopen( hConHandle, "r" );
        *stdin = *fp;
        setvbuf( stdin, NULL, _IONBF, 0 );

        // redirect unbuffered STDERR to the console
        lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
        hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
        fp = _fdopen( hConHandle, "w" );
        *stderr = *fp;
        setvbuf( stderr, NULL, _IONBF, 0 );

        // make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
        // point to console as well
        ios::sync_with_stdio();
}
void DebugConsole::hide()
{
	FreeConsole();
}

void DebugConsole::dumpBuffer(const char* buffer, unsigned int size, const char* title)
{
	printf("%s> %d bytes", title, size);

	for (unsigned int o = 0; o < ceil(size / 16.0f); o++)
	{
		printf("\n  ");
		for (unsigned int t = 0; t < 2; t++)
		{
			for (unsigned int i = 0; i < 16; i++)
			{
				unsigned char val = buffer[(o * 16) + i] & 0xFF;
				if (t == 0)
				{
					if (i == 8)
						printf("| ");

					if ((o * 16) + i < size)
						printf("%02X ", val);
					else
						printf("   ", val);
				}
				else if ((o * 16) + i < size)
				{
					if (val > ' ' && val <= '~')
						printf("%c", val);
					else
						printf(".");
				}
				else
					printf(" ");
			}
			printf("|");
		}
	}

	printf("\n");
}