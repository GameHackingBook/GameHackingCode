#include "main.h"

int main(void)
{
	// due to differences in the way functions are compiled between
	// DEBUG and RELEASE builds, this example will only work in RELEASE.
	// This is only because of the way I'm finding the addresses of 
	// the NOP and CALL targets
	NOPExample();
	callHookExample();
	VFHookExample();
	IATHookExample();

	while (true) // stay busy
	{
		Sleep(100);
	}
}