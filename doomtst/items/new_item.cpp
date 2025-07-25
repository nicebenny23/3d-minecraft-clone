#include "new_item.h"


bool item_name::item::can_give(item& other, size_t cnt)
{
	if (!can_interact(*this,other))
	{
		return false;

	}
	
	if (type != item_type::block)
	{
		return false;
	}
	return owner.getcomponent<item_stack>().can_give(other.owner.getcomponent<item_stack>(), cnt);

}

size_t item_name::item::get_count()
{
	if (type==item_type::tool)
	{
		return owner.getcomponent<item_durability>().durability();
	}
	else {

		return owner.getcomponent<item_stack>().count();
	}
}

size_t item_name::item::get_max()
{
	if (type == item_type::tool)
	{
		return owner.getcomponent<item_durability>().max_dur();
	}
	else {

		return owner.getcomponent<item_stack>().capacity();
	}
}
