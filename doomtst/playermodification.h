#include "renderer/uirender.h"
#include "game/gameobject.h"
#include "game/collision.h"
#include "world/managegrid.h"
#include "game/objecthelper.h"
#include "world/voxeltraversal.h"
#include "util/dynamicarray.h"
using namespace objutil;
#ifndef  playerplace_Hpp
#define playerplace_Hpp
struct playermod:gameobject::component
{

	array<uirender::uibox*> box;
	array<uirender::uibox*> blockbox;
	int curplaceid;
	void start() {
		box = array<uirender::uibox*>();
		blockbox = array<uirender::uibox*>();
	
		
		for (int i = 0; i < 6; i++)
		{
			switch (i)
			{
			case 0:

				blockbox[i] = uirender::newbox("stone.png", v2::unitv / 30.f, v2::Vector2(i / 10.f, -.4f));
				break;
			case 1:

				blockbox[i] = uirender::newbox("glass.png", v2::unitv / 30.f, v2::Vector2(i / 10.f, -.4f));
			case 3:

				blockbox[i] = uirender::newbox("torch.png", v2::unitv / 30.f, v2::Vector2(i / 10.f, -.4f));
				break;
			case 4:

				blockbox[i] = uirender::newbox("water.png", v2::unitv / 30.f, v2::Vector2(i / 10.f, -.4f));
				break;
			default:
				blockbox[i] = uirender::newbox("wood.png", v2::unitv / 30.f, v2::Vector2(i / 10.f, -.4f));
				break;
			}box[i] = uirender::newbox("blockholder.png", v2::unitv / 20.f, v2::Vector2(i/10.f, -.4f));
			box[i]->shouldrender = true;
			blockbox[i]->shouldrender = true;
		}
	
		
		curplaceid = 0;
	}


	void update() {
		if (userinput::getinputkey('1').pressed)
		{
			curplaceid = 0;
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
		}
			ray cameraray = ray(Vector3(camera::campos), Vector3(camera::campos) + camera::direction() * 7);
			voxtra::raycolwithgrid closest = voxtra::travvox(cameraray, 1000);

			if (closest.box != nullptr)
			{


				toblk(closest.box->owner).lightval = 14;
				for (int i = 0; i < 6; i++)
				{
					(*toblk(closest.box->owner).mesh)[i].light = 14;

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
							int previd = plamentblock->id;
							//i dont know why i create it and remove itit like this but it makes the core much simpler
							gridutil::setblock(plamentblock->pos,curplaceid);
							if (plamentblock->solid)
							{


								if (collision::aabbcollideswithent(&plamentblock->getcomponent<colrect>()))
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
