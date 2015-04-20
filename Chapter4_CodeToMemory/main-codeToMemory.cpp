#include <iostream>
#include <windows.h>

/* numeric values */
unsigned char ubyteValue = 0xFF;
char byteValue = 0xFE;

unsigned short uwordValue = 0x4142;
short wordValue = 0x4344;

unsigned int udwordValue = 0xDEADBEEF;
int dwordValue = 0xDEADBEEF;

unsigned long long ulongLongValue = 0xEFCDAB8967452301;
long long longLongValue = 0xEFCDAB8967452301;

float floatValue = 1337.7331;

/* string values */
char* thinStringP = "my_thin_terminated_value_pointer";
char thinStringA[40] = "my_thin_terminated_value_array";
wchar_t* wideStringP = L"my_wide_terminated_value_pointer";
wchar_t wideStringA[40] = L"my_wide_terminated_value_array";

/* structures */
struct MyStruct {
	unsigned char ubyteValue;
	char byteValue;
	unsigned short uwordValue;
	short wordValue;
	unsigned int udwordValue;
	int dwordValue;
	unsigned long long ulongLongValue;
	char interruptor;
	long long longLongValue;
	float floatValue;
};

/* classes with no VF tables */
class bar {
public:
	bar() : bar1(0x898989), bar2(0x10203040) {}
	void myfunction() { bar1++; }
	int bar1, bar2;
};

/* classes with VF tables */
class foo {
public:
	foo() : myValue1(0xDEADBEEF), myValue2(0xBABABABA) {}
	int myValue1;
	static int myStaticValue;
	virtual void bar() { printf("foo::bar()\n"); }
	virtual void baz() { printf("foo::baz()\n"); }
	virtual void barbaz() {}
	int myValue2;
};
int foo::myStaticValue = 0x12121212;
class fooa : public foo {
public:
	fooa() : foo() {}
	virtual void bar() { printf("fooa::bar()\n"); }
	virtual void baz() { printf("fooa::baz()\n"); }
};
class foob : public foo {
public:
	foob() : foo() { }
	virtual void bar() { printf("foob::bar()\n"); }
	virtual void baz() { printf("foob::baz()\n"); }
};

/* class protection */
class baz {
public:
	baz() : baz1(0x11111111), baz2(0x22222222),
		baz3(0x33333333), baz4(0x44444444) {}
	int baz1, baz2;
	void printStuff()
	{
		printf("0x%x : baz->baz1\n", &this->baz1);
		printf("0x%x : baz->baz2\n", &this->baz2);
		printf("0x%x : baz->baz3\n", &this->baz3);
		printf("0x%x : baz->baz4\n", &this->baz4);
	}
private:
	int baz3, baz4;
};

int main(void)
{
	/* just using global values so they don't get optimized away */
	ubyteValue = ubyteValue;
	byteValue = byteValue;
	wordValue = wordValue;
	uwordValue = uwordValue;
	udwordValue = udwordValue;
	dwordValue = dwordValue;
	ulongLongValue = ulongLongValue;
	longLongValue = longLongValue;
	floatValue = floatValue;

	thinStringP = thinStringP;
	if (thinStringA){}
	wideStringP = wideStringP;
	if (wideStringA){}

	/* printing addresses so we can easily find the dumps */
	printf("0x%x : ubyteValue\n", &ubyteValue);
	printf("0x%x : thinStringP\n", &thinStringP);

	/* showing structure arrangement */
	MyStruct* m = 0;
	printf("Offsets: %d,%d,%d,%d,%d,%d,%d,%d,%d\n",
			&m->ubyteValue, &m->byteValue,
			&m->uwordValue, &m->wordValue,
			&m->udwordValue, &m->dwordValue,
			&m->ulongLongValue, &m->longLongValue,
			&m->floatValue);

	/* union stuff */
	union {
		BYTE byteValue;
		struct {
			WORD first;
			WORD second;
		} words;
		DWORD value;
	} dwValue;
	dwValue.value = 0xDEADBEEF;
	printf("Size %d; Addresses 0x%x,0x%x; Values 0x%x,0x%x\n",
		sizeof(dwValue), &dwValue.value, &dwValue.words,
		dwValue.words.first, dwValue.words.second);

	/* classes with no VF tables */
	bar _bar = bar();
	printf("Size %d; Address 0x%x : _bar\n", sizeof(_bar), &_bar);

	/* class VF call */
	foo* _testfoo = (foo*)new fooa();
	_testfoo->bar();

	/* classes with VF tables */
	foo _foo = foo();
	fooa _fooa = fooa();
	foob _foob = foob();
	printf("0x%x : _foo\n", &_foo);
	printf("0x%x : _fooa\n", &_fooa);
	printf("0x%x : _foob\n", &_foob);
	
	_foo.barbaz();
	_fooa.bar();
	_foob.baz();

	/* class protection */
	baz* _baz = 0;
	_baz->printStuff();

	system("pause");
}