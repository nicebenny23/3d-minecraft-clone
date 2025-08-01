#include "gameobject.h"
#include "../util/mathutil.h"
#include "entity.h"
#include "rigidbody.h"
#include "objecthelper.h"
#include "../renderer/model.h"
#pragma once
struct entityeffects {
	float timetilllavaover;

};                 

struct estate : gameobject::component
{
	
	entityeffects effects;
	timename::duration model_red_dur;
	bool inliquid;
	bool takesfalldmg;
	int health;
	bool prevonground;
	int maxhealth;
	float damagemultiplyer;
	v3::Vec3 velocitylast;
	float lastongroundy;
	float invincablilitymax;
	float timetilldmg;
	void update() {
		if (model_red_dur.state()==timename::duration_state::ending)
		{
			if (owner.hascomponent<model>())
			{
				for (ModelMeshName::ModelMesh& msh : owner.getcomponent<model>())
				{
					msh.color = Vec3(1, 1, 1);
				}
			}
		}
		timetilldmg -= CtxName::ctx.Time->dt;
		if (owner.hascomponent<rigidbody>())
		{
			testfalldamage();
			prevonground = owner.getcomponent<rigidbody>().isonground;
		}
		
	}
	void testfalldamage() {
		if (takesfalldmg)
		{



			if (owner.getcomponent<rigidbody>().isonground && !prevonground)
			{
				float ypos = owner.transform().position.y;

				if (owner.getcomponent<rigidbody>().velocity.y < -5)
				{

					damage(-(owner.getcomponent<rigidbody>().oldvelocity.y / 44444));

				}

			}
			if (owner.getcomponent<rigidbody>().isonground) {


				lastongroundy = owner.transform().position.y;
			}
		}
	}
	void start() {
		model_red_dur=CtxName::ctx.Time->create_dur();
		priority = 11;
		health = maxhealth;
		lastongroundy = owner.transform().position.y;
	}
	estate(int maxhp, bool falls){
		maxhealth = maxhp;
		health = maxhp;
		damagemultiplyer = 1;
		invincablilitymax = 1;
		takesfalldmg = falls;
	}
	void remove();
	void damage(int dmg) {
		dmg = dmg * damagemultiplyer;
		if (dmg <= 0)
		{
			return;
		}
		if (timetilldmg < 0)
		{

			model_red_dur.set(.3f);
			if (owner.hascomponent<model>())
			{

				
				for (ModelMeshName::ModelMesh& msh:owner.getcomponent<model>())
				{
					msh.color = Vec3(1.0f, .3f, .3f);
				}
			}
			timetilldmg = invincablilitymax;
			health -= dmg;
			if (health <= 0)
			{
				remove();

				
				
				
			}
			health = clamp(health, 0, maxhealth);
		}


	}
	void heal(int healamt) {
		if (healamt <= 0)
		{
			return;
		}

	timetilldmg = invincablilitymax;
		health += healamt;

		health = clamp(health, 0, maxhealth);

	}
};
 // ! entitystate_HPP;
