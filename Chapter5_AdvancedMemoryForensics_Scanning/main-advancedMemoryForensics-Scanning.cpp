#include <iostream>
#include <windows.h>
#include <vector>
#include <list>
#include <map>

struct PlayerVital {
	int current, maximum;
};

PlayerVital health = {450, 500};

std::vector<DWORD> vectorData;
std::list<DWORD> listData;

typedef int keyInt;
typedef int valInt;
std::map<keyInt, valInt> mapData;


void printString(const char* text, int one, int two)
{
	printf(text, one, two);
}

void printHealth()
{
	printString("Health: %d of %d\n", health.current, health.maximum);
}

// VECTOR

void readVector(DWORD vectorAddress)
{
	struct _vector
	{
		DWORD* begin;
		DWORD* end;
		DWORD* tail;
	};

	_vector* vec = (_vector*)vectorAddress;

	DWORD count = ((DWORD)vec->end - (DWORD)vec->begin) / sizeof(DWORD);
	DWORD capacity = ((DWORD)vec->tail - (DWORD)vec->begin) / sizeof(DWORD);

	printf("Vector has %d items and %d capacity\n", count, capacity);

	for (int i = 0; i < count; i++)
		printf("\tValue at %d is %d\n", i, vec->begin[i]);
}

// LIST

void readList(DWORD listAddress)
{
	struct listItem
	{
		listItem* next;
		listItem* prev;
		DWORD value;
	};

	struct _list
	{
		listItem* root;
		DWORD size;
	};

	_list* list = (_list*)listAddress;

	printf("List has %d items\n", list->size);

	for (listItem* it = list->root->next; it != list->root; it = it->next)
		printf("\tForward value is %d\n", it->value);

	for (listItem* it = list->root->prev; it != list->root; it = it->prev)
		printf("\tReverse value is %d\n", it->value);
}

// MAP

struct mapItem {
    mapItem* left;
    mapItem* parent;
    mapItem* right;
    keyInt key;
    valInt value;
};
struct _map {
    DWORD irrelevant;
    mapItem* rootNode;
    int size;
};

mapItem* findItem(keyInt key, mapItem* node, mapItem* root)
{
	if (node != root) {
		if (key == node->key)
			return node;
		else if (key < node->key)
			return findItem(key, node->left, root);
		else
			return findItem(key, node->right, root);
	}
	else
		return root;
}

mapItem* searchMap(keyInt key, _map* map)
{
	mapItem* ret =
		findItem(key, map->rootNode->parent, map->rootNode);
	if (ret == map->rootNode) return NULL;
	return ret;
}


void iterateMap(mapItem* node, mapItem* root)
{
	if (node == root) return;
	iterateMap(node->left, root);
	printf("\tKey %d has value 0x%04x\n", node->key, node->value);
	iterateMap(node->right, root);
}

void readMap(DWORD mapAddress)
{
	_map* map = (_map*)mapAddress;

	printf("Nodes in map: %d\n", map->size);
	iterateMap(map->rootNode->parent, map->rootNode);

	printf("\tMap search for 1 yields: 0x%04x\n", searchMap(1, map)->value);
	printf("\tMap search for 2 yields: 0x%04x\n", searchMap(2, map)->value);
	printf("\tMap search for 3 yields: 0x%04x\n", searchMap(3, map)->value);
	printf("\tMap search for 5 yields: 0x%04x\n", searchMap(5, map)->value);
}


int main(void)
{
	vectorData.reserve(20);
	vectorData.push_back(12345);
	vectorData.push_back(54321);

	listData.push_back(123);
	listData.push_back(321);
	listData.push_back(121);

	mapData.insert(std::pair<DWORD, int>(1, 0x100));
	mapData.insert(std::pair<DWORD, int>(2, 0x200));
	mapData.insert(std::pair<DWORD, int>(3, 0x200));
	mapData.insert(std::pair<DWORD, int>(5, 0x500));

	while (true) // stupid loop to keep anything needed for the example from being optimized away
	{
		auto something = &printString;

		printHealth();
		readVector((DWORD)&vectorData);
		readList((DWORD)&listData);
		readMap((DWORD)&mapData);

		health.current = (health.current == health.maximum) ? 1 : (health.current + 1);
		health.maximum = 500;
		system("pause");
	}
}