#include "new_item.h"
#include "../renderer/uibox.h"
#include "../renderer/textrender.h"

#include "../game/System.h"
#pragma once

struct item_ui :gameobject::component {
	void disable() {
		sprite.state.enabled = false;
		count.state.enabled = false;
	}
	void enable() {
		sprite.state.enabled = false;
		count.state.enabled = false;
	}
	void set_count(size_t cnt) {
		count.value = cnt;
	}
	void set_position(v2::Vec2 pos)
	{
		sprite.box.center = pos;
		count.center = pos;
	}
	void set_texture(const char* location, const char* name) {
		sprite.LoadTex(location, name);
	}
	void set_scale(v2::Vec2 scale) {
		sprite.box.scale=scale;
	}
	void destroy_hook() {
		sprite.destroy();
		count.destroy();
	}
private:
	uiboxname::uibox sprite;
	integertext count;
};
struct ItemUisync:System{
	
	void run(gameobject::Ecs* ecs) {
		
	

            query::View< item_name::item,item_ui> items(ecs);
            for (auto [itm, ui] : items) {
				if (itm->type==item_name::item::item_type::block)
				{
					ui->set_count(itm->get_count());
				}
				else {
					ui->set_count(float(itm->get_count()*5)/itm->get_max());
				}
            }


	}
};