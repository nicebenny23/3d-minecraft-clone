#include "inventoryblock.h"
#include "menu.h"
#ifndef itemstorage_HPP
#define itemstorage_HPP
struct itemstorage
{
	array<invblock> databuf;
	void swapo(item* held);
};


#endif // !itemstorage_H
