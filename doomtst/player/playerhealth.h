#pragma once
#include "../util/dynamicarray.h"
#include "../renderer/uibox.h"
#include "../game/gameobject.h"
#include <conio.h>
#include "../game/entitystate.h"
#include "playerinventory.h"
#include "../util/cached.h"

struct playerhealth: gameobject::component
{
	Cptr::cptr<uibox> damage_decal;
	timename::duration damage_decal_duration;
	
	bool dmgimmune = false;
	void killplayer() {
		system("cls");
		glfwTerminate();
		std::exit(0);
	
	
	}
	array<Cptr::cptr<uibox>>healthboxes;
	void start(){
		damage_decal_duration=CtxName::ctx.Time->create_dur();
		size_t max_health = owner.getcomponent<estate>().maxhealth;
		priority = -224;
		v2::Vec2 scale = v2::unitv / 100;
		healthboxes = array<Cptr::cptr<uibox>>();
		for (int i = 0; i < max_health; i++) {
			v2::Vec2 pos = v2::Vec2(i / 40.f-.4f, -.45);

			damage_decal= ui::createuielement<uibox>("images\\red_back.png", "on_dmg_texture", v2::unitv, v2::zerov, -3);
			damage_decal->state.enabled = false;
			healthboxes.push(ui::createuielement<uibox>("images\\health.png","HealthTexture",scale,pos,55));
		}
	}
	void update() {
		
		damage_decal->state.enabled= owner.getcomponent<estate>().model_red_dur.is_active();
		size_t max_health = owner.getcomponent<estate>().maxhealth;
		float dmgmul = 1;
		for (int i = 0; i < 2; i++)
		{
			item* itm = owner.getcomponent<inventory>().playermenu.armor.at(i).helditem;
			if (itm != nullptr)
			{
				dmgmul *= itm->properties.armor;
			}
		}
		owner.getcomponent<estate>().damagemultiplyer = dmgmul;
		int health = owner.getcomponent<estate>().health;
		if (health <= 0)
		{

			killplayer();


		}
		if (owner.getcomponent<estate>().invincablilitymax != owner.getcomponent<estate>().timetilldmg)
		{
			if (!dmgimmune) {

				dmgimmune = true;
				for (int i = 0; i < 2; i++)
				{
					item* itm = owner.getcomponent<inventory>().playermenu.armor.at(i).helditem;
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

				healthboxes[i]->state.enabled = true;
			}
			for (int i = health; i < max_health; i++) {

				healthboxes[i]->state.enabled = false;
			}
		}
	}
};

