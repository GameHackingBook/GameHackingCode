#include <iostream>
#include <Windows.h>
#include <vector>

/*
	THIS IS JUST SOME BOILER-PLATE TO ALLOW THE EXAMPLE CODE TO WORK,
	NOTHING INTERESTING HERE
*/


template<typename T>
T readMemory(DWORD address)
{
	return *((T*)address);
}

template<typename T>
T* pointMemory(DWORD address)
{
	return ((T*)address);
}

template<typename T>
void writeMemory(DWORD address, T value)
{
	*((T*)address) = value;
}

template<typename T>
DWORD protectMemory(DWORD address, DWORD prot)
{
	DWORD oldProt;
	VirtualProtect((LPVOID)address, sizeof(T), prot, &oldProt);
	return oldProt;
}

struct _creature
{
	_creature(int hb, bool e, bool c) : healthBar(hb), isEnemy(e), isCloaked(c)  {}
	int healthBar;
	bool isEnemy, isCloaked;
};
void NOPExample();
void callHookExample();
void VFHookExample();
void IATHookExample();