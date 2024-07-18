#include "renderer/uirender.h"
#include "game/gameobject.h"
#include "game/collision.h"
#include "world/managegrid.h"
#include "game/objecthelper.h"
#include "world/voxeltraversal.h"
#include "util/dynamicarray.h"
#include "items/itemstorage.h"
using namespace objutil;
#ifndef  playerplace_Hpp
#define playerplace_Hpp
struct playermod:gameobject::component
{
	Container hotbar;
	int curplaceid;
	void start() {

		hotbar = Container(5, 1, 0, -5);
		hotbar.setviewable(true);
		curplaceid = 0;
	}


	void update() {
		if (userinput::getinputkey('1').pressed)
		{
			curplaceid = 1;
		}
		if (userinput::getinputkey('2').pressed)
		{
			curplaceid = 1;
		}
		if (userinput::getinputkey('3').pressed)
		{
			curplaceid = minecraftstone;
		}
		if (userinput::getinputkey('4').pressed)
		{
			curplaceid = minecraftglass;
		}
		if (userinput::getinputkey('5').pressed)
		{
			curplaceid = minecraftwater;
		}	if (userinput::getinputkey('6').pressed)
		{
			curplaceid = minecrafttorch;
		}
			ray cameraray = ray(Vector3(camera::campos), Vector3(camera::campos) + camera::direction() * 7);
			voxtra::RayCollisionWithGrid closest = voxtra::travvox(cameraray, 1000);

			if (closest.box != nullptr)
			{


			//	toblk(closest.box->owner).lightval = 14;
				for (int i = 0; i < 6; i++)
				{
				//	(*toblk(closest.box->owner).mesh)[i].light = 14;

				}
				if (userinput::mouseleft.pressed)
				{

					if (interactminrange < closest.dist && closest.dist < interactmaxrange)
					{


						gridutil::setblock(toblk(closest.box->owner).pos, minecraftair);
					}
				}


				else if (userinput::mouseright.pressed)
				{

					if (interactminrange < closest.dist && closest.dist < interactmaxrange)
					{

						block* plamentblock = voxtra::findprevblock(cameraray, 1000);
						if (plamentblock != nullptr && !plamentblock->solid)
						{
							int dir = maxdirection( closest.box->center - plamentblock->center());
							int previd = plamentblock->id;
							//i dont know why i create it and remove itit like this but it makes the core much simpler
							int blockdirection = max2ddirection(Vector3( camera::campos)-closest.colpoint);
							
							plamentblock->mesh.direction = blockdirection;
							plamentblock->mesh.attachdir = dir;
							gridutil::setblock(plamentblock->pos,curplaceid);
							
							if (plamentblock->solid)
							{


								if (collision::aabbCollidesWithEntity(&plamentblock->getcomponent<Collider>()))
								{

									gridutil::setblock(plamentblock->pos, previd);
								}
							}
						}

					}


				}

			}
		}





};


#endif // ! playerplace_H
