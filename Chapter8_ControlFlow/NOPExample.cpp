#include "main.h"

// this is the simulated list of creatures in the game
std::vector<_creature> creatures;
int creaturesDrawn = 0;

// this is the simulated function to draw creature names
void drawHealthBar(int healthbar) 
{
	creaturesDrawn++; // make a note that we drew it
	Sleep(healthbar); // just an example, we're not really doing anything
}

// this is the function where we place the NOP.
void drawCreatureHealthBarExample()
{
	for (int i = 0; i < creatures.size(); i++) {
		auto c = creatures[i];
		if (c.isEnemy && c.isCloaked)
		{
			// our NOP is esentially going to remove this continue statement,
			// which will be a JUMP that evades the drawHealthBar car
			continue;
		}
		drawHealthBar(c.healthBar);
	}
}

// this is the function that actually does the NOP
template<int SIZE>
void writeNop(DWORD address)
{
	auto oldProtection = protectMemory<BYTE[SIZE]>(address, PAGE_EXECUTE_READWRITE);
	for (int i = 0; i < SIZE; i++)
		writeMemory<BYTE>(address + i, 0x90);
	protectMemory<BYTE[SIZE]>(address, oldProtection);
}


// I cannot hard-code any addresses from within this application,
// as they may change upon re-compile. For this reason, I'll locate
// the address of the JMP that I want to replace programatically
// by scanning for the 'JMP -67' statement (bytes 0xEB 0xBD)
// somewhere within 1000 bytes of the start of the function
DWORD getAddressForNOP(DWORD functionStart)
{
	auto oldProtection = protectMemory<BYTE[1000]>(functionStart, PAGE_EXECUTE_READ); // make sure memory is readable, just incase
	auto mem = pointMemory<BYTE>(functionStart);

	DWORD ret = 0;
	for (int i = 0; i < 999; i++) {
		if (mem[i] == 0xEB && mem[i+1] == 0xBD) {
			ret = functionStart + i;
			break;
		}
	}
	protectMemory<BYTE[1000]>(functionStart, oldProtection); // restore old memory protection
	return ret;
}

// This ties the entire example together
void NOPExample()
{
	creaturesDrawn = 0;

	// nop it
	auto address = getAddressForNOP((DWORD)&drawCreatureHealthBarExample);
	if (address)
		writeNop<2>(address);

	// add some make creatures
	creatures.push_back(_creature(0, true, true));
	creatures.push_back(_creature(0, true, false));
	creatures.push_back(_creature(0, false, true));
	creatures.push_back(_creature(0, false, false));

	//call the function
	drawCreatureHealthBarExample();

	//check if NOP worked
	if (creaturesDrawn == 4)
		printf("NOP worked! Drew all creatures!\n");
	else
		printf("NOP failed! :( Only drew %d/4 creatures.\n", creaturesDrawn);
}