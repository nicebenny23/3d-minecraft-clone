#pragma once
#include "../util/dynamicarray.h"
#include "../renderer/uibox.h"
#include "../game/gameobject.h"
#include <conio.h>
#include "../game/entitystate.h"
#include "playerinventory.h"

struct playerhealth:gameobject::component
{
	bool dmgimmune = false;
	void killplayer() {
		system("cls");
		
		glfwTerminate();
		std::exit(0);
	
	
	}
	array<Cptr::cptr<uibox>,true> healthboxes;
	void start(){
		priority = -224;
		v2::Vector2 scale = v2::unitv / 100;
		healthboxes = array<Cptr::cptr<uibox>,true>(10);
		for (int i = 0; i < 10; i++) {
			v2::Vector2 pos = v2::Vector2(i / 40.f-.4f, -.45);
			healthboxes.push(ui::createuielement<uibox>("images\\health.png","HealthTexture",scale,pos,55));
		}
	}
	void update() {
		float dmgmul = 1;
		for (int i = 0; i < 2; i++)
		{
			item* itm = owner->getcomponent<inventory>().playermenu.armor.at(i).helditem;
			if (itm != nullptr)
			{
				dmgmul *= itm->properties.armor;
			}
		}
		owner->getcomponent<estate>().damagemultiplyer = dmgmul;
		int health = owner->getcomponent<estate>().health;
		if (health <= 0)
		{

			killplayer();


		}
		if (owner->getcomponent<estate>().invincablilitymax != owner->getcomponent<estate>().timetilldmg)
		{
			if (!dmgimmune) {

				dmgimmune = true;
				for (int i = 0; i < 2; i++)
				{
					item* itm = owner->getcomponent<inventory>().playermenu.armor.at(i).helditem;
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
			for (int i = health; i < 10; i++) {

				healthboxes[i]->state.enabled = false;
			}
		}
	}
};

