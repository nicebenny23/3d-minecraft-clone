#include "../util/dynamicarray.h"
#include "../renderer/uirender.h"
#include "../game/gameobject.h"
#include <conio.h>
#include "../game/entitystate.h"
#ifndef playerhealth_HPP
#define playerhealth_HPP
struct playerhealth:gameobject::component
{
	void killplayer() {
		system("cls");
		
		glfwTerminate();
		std::exit(0);
	
	
	}
	array<uirender::uibox*> healthboxes;
	void start(){
		priority = -224;
		v2::Vector2 scale = v2::unitv / 100;
		healthboxes = array<uirender::uibox*>();
		for (int i = 0; i < 10; i++) {
			v2::Vector2 pos = v2::Vector2(i / 40.f-.4f, -.45);
			healthboxes.append(uirender::newbox("images\\health.png",scale,pos,55));
		}
	}
	void update() {
		int health = owner->getcomponent<estate>().health;
		if (health<=0)
		{
			
			killplayer();
		
		
		}
	
		for (int i =0; i < health; i++) {
			
			healthboxes[i]->shouldrender = true;
		}
		for (int i = health; i < 10; i++) {
			
			healthboxes[i]->shouldrender = false;
		}
	}
};



#endif // !1
