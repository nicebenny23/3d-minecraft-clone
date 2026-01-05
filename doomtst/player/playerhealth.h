#pragma once
#include "../util/dynamicarray.h"
#include "../renderer/uibox.h"
#include "../game/ecs/game_object.h"
#include <conio.h>
#include "../game/entitystate.h"
#include "playerinventory.h"
#include "../util/cached.h"

struct playerhealth: ecs::component
{
	Cptr::cptr<ui_image_component> damage_decal;
	timename::duration damage_decal_duration;
	
	bool dmgimmune = false;
	void killplayer() {
		system("cls");
		glfwTerminate();
		std::exit(0);
	
	
	}
	array<Cptr::cptr<ui_image_component>>healthboxes;
	void start(){
		damage_decal_duration=CtxName::ctx.Ecs->ensure_resource<timename::TimeManager>().create_dur();
		size_t max_health = owner().get_component<estate>().maxhealth;
		v2::Vec2 scale = v2::unitv / 100;
		healthboxes = array<Cptr::cptr<ui_image_component>>();
		for (int i = 0; i < max_health; i++) {
			v2::Vec2 pos = v2::Vec2(i / 40.f-.4f, -.45);

			damage_decal= ui::createuielement<ui_image_component>("images\\red_back.png", "on_dmg_texture", v2::unitv, v2::zerov, -3);
			damage_decal->disable();
			healthboxes.push(ui::createuielement<ui_image_component>("images\\health.png","HealthTexture",scale,pos,55));
		}
	}
	void update() {
		
		damage_decal->state.enabled= owner().get_component<estate>().model_red_dur.is_active();
		size_t max_health = owner().get_component<estate>().maxhealth;
		float dmgmul = 1;
		for (int i = 0; i < 2; i++)
		{
			item* itm = owner().get_component<inventory>().playermenu.armor.at(i).helditem;
			if (itm != nullptr)
			{
				dmgmul *= itm->properties.armor;
			}
		}
		owner().get_component<estate>().damagemultiplyer = dmgmul;
		int health = owner().get_component<estate>().health;
		if (health <= 0)
		{

			killplayer();


		}
		if (owner().get_component<estate>().invincablilitymax != owner().get_component<estate>().timetilldmg)
		{
			if (!dmgimmune) {

				dmgimmune = true;
				for (int i = 0; i < 2; i++)
				{
					item* itm = owner().get_component<inventory>().playermenu.armor.at(i).helditem;
					if (itm != nullptr)
					{
						if (itm->properties.armor != 1) {
							itm->amt -= 1;
						}
					}

				}
			}
			else
			{
				dmgimmune = false;
			}
			for (int i = 0; i < health; i++) {

				healthboxes[i]->enable();
			}
			for (int i = health; i < max_health; i++) {

				healthboxes[i]->disable();
			}
		}
	}
};

namespace ecs {
	template<>
	inline constexpr ComponentInfo ComponentTraits<playerhealth> = {
		.priority = -233,
	};
}