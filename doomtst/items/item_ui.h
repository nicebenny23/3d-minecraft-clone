#include "new_item.h"
#include "../renderer/uibox.h"
#include "../renderer/textrender.h"


#pragma once

struct item_ui :ecs::component{
	void disable() {
		sprite.disable();
		count.disable();
	}
	void enable() {
		sprite.disable();
		count.disable();
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
	uiboxname::ui_image_component sprite;
	text_component count;
};
struct ItemUisync:System{
	
	void run(ecs::Ecs* ecs) {
		
	

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