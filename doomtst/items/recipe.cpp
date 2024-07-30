#include "recipe.h"

void recipemanager::destroy() {

    newitemlocation->destroy();
    resourcecontainer->destroy();
}
void recipemanager::createcontainers() {

    newitemlocation = new Container(1, 1, 2.5, 3.5);
    resourcecontainer = new Container(xsize, ysize, -1.5, 4.5);

}
void recipemanager::addrecipe(irecipe recipe)
    {
    
    for (int i = 0; i < recipelist->length; i++) {
        if (recipelist->at(i).xsize != resourcecontainer->sizex) {
            Assert("xsize not correct for recipe manager");
        }
        if (recipelist[i].at(i).ysize != resourcecontainer->sizey) {
            Assert("ysize not correct for recipe manager");
        }
    }
}

void recipemanager::ontake() {
    // Implementation needed
}

void recipemanager::preview() {
    

    if (true)
    {
        newitemlocation->databuf[0].destroyitem();
        irecipe* todisplay = searchrecipe();

        if (todisplay != nullptr)
        {


            newitemlocation->databuf[0].giveitem(todisplay->itemcreated.id,todisplay->itemcreated.amt);

            newitemlocation->at(0).helditem->updateui();
        }
    }
       
    

}

irecipe* recipemanager::searchrecipe() {
    int maxamt = 0;
 

    for (int i = 0; i < recipelist->length; i++) {
        if (!recipelist->at(i).cancraft(resourcecontainer)) {
            continue;
        }


          
            return  &recipelist->at(i);
        
    }

    return nullptr;
}

void recipemanager::updatestate()
{
    item* itemincont = newitemlocation->at(0).helditem;
   

    state.craftedthisframe = false;
        if (state.enabled)
        {
            
            resourcecontainer->update();
            
                item* freeitem = freeditem;
                if (state.cancraft)
                {


                    newitemlocation->deletebelowzero();


                    preview();
                    if (newitemlocation->clicked() && freeditem == nullptr)
                    {
                        craft();
                    }
                }
        }
    
}

recipemanager::recipemanager(const char* filename, int sizex, int sizey)
{
recipelist= new array<irecipe>();
xsize = sizex;
ysize = sizey;
   safefile recipefile= safefile(filename, fileread);
   createcontainers();
   char line[256];
   int maxlinesize = xsize * ysize + 1;
   int linenumber = 0;

   iteminrecipe returneditem;
   array<iteminrecipe> recipearr;
   int linesize = xsize * ysize;
   while ((fgets(line, sizeof(line), recipefile.fp) != NULL))
    {
    
       

       
       if (line[0] == '\r') {
           continue;
       }
       if (line[0] == '#') {
           continue;
       }
           int itemid;
           int itemamt;
           int matches = sscanf(line, "%d,%d", &itemid, &itemamt);
           if (matches != 2)
           {
               Assert("error in recipe creation");
           }


           recipearr.append(iteminrecipe(itemid, itemamt));
       
             
             linenumber++;

             if (linenumber == maxlinesize)
             {

                 irecipe newrecipe = irecipe(recipearr.getdata(), recipearr[recipearr.length - 1], xsize, ysize);
                 recipelist->append(newrecipe);
                 recipearr.destroy();
                 linenumber = 0;
                 continue;
             }
       
    }

   recipefile.close();
}

void recipemanager::save()
{
    resourcecontainer->writetofile();

    newitemlocation->writetofile();
}

void recipemanager::enable()
{
    state.enabled = true;
    newitemlocation->enable();
    resourcecontainer->enable();
}

void recipemanager::disable()
{
    state.enabled = false;
    newitemlocation->disable();
    resourcecontainer->disable();
}

bool irecipe::cancraft(Container* resourcecont) {
    if (xsize != resourcecont->sizex) {
        Assert("size != recipesize");
    }
    if (ysize != resourcecont->sizey) {
        Assert("size != recipesize");
    }
    for (int i = 0; i < resourcecont->databuf.length; i++) {
        item* itematpos = resourcecont->at(i).helditem;

     
        if (itematpos == nullptr)
        {
            //-1 implies null;
            if (recipe[i].id == 0)
            {
                continue;
            }
            else
            {
                //only  one is n ull ptr
                return false;
            }
        }
        if (recipe[i].id != itematpos->id) {
            return false;
        }

        if (itematpos->amt < recipe[i].amt) {
            return false;
        }
    }

    return true;
}


irecipe::irecipe(iteminrecipe* itemarray, iteminrecipe created, int sizex, int sizey)
{
    recipe =array< iteminrecipe>(itemarray,sizex*sizey);
    itemcreated = created;
    xsize = sizex;
    ysize = sizey;
}



void recipemanager::craft() {
    irecipe* bestrecipe = searchrecipe();
  
    if (bestrecipe==nullptr)
    {
        return;
    }
    for (int i = 0; i < resourcecontainer->databuf.length; i++) {
        if (bestrecipe->recipe[i].id == 0) {
            continue;
        }
        if (resourcecontainer->databuf[i].helditem->itemtype==wear)
        {
            resourcecontainer->databuf[i].destroyitem();
        }
        else
        {

            resourcecontainer->databuf[i].helditem->amt -= bestrecipe->recipe[i].amt;
        }
    }
    state.craftedthisframe = true;
    newitemlocation->update();
}
