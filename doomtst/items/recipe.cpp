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
    

    if (cancreate)
    {
        newitemlocation->databuf[0].destroyitem();
        irecipe* todisplay = searchrecipe();

        if (todisplay != nullptr)
        {


            newitemlocation->databuf[0].giveitem(todisplay->itemcreated.id);

            newitemlocation->at(0).helditem->updateui();
        }
    }
       
    

}

irecipe* recipemanager::searchrecipe() {
    int maxamt = 0;
    irecipe* maximal = nullptr;

    for (int i = 0; i < recipelist->length; i++) {
        if (!recipelist->at(i).cancraft(resourcecontainer)) {
            continue;
        }

        int recipeamt = 0;
        for (int j = 0; j < xsize*ysize; j++) {
            if (recipelist->at(i).recipe[j].id != -1) {
                recipeamt += 1;
            }
        }

        if (recipeamt > maxamt) {
            maxamt = recipeamt;
            maximal = &recipelist->at(i);
        }
    }

    return maximal;
}

void recipemanager::updatestate()
{
    newitemlocation->deletebelowzero();
  
    resourcecontainer->update();
    preview();
    if (newitemlocation->clicked())
    {
        craft();
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

void recipemanager::setviewable(bool state)
{
    newitemlocation->setviewable(state);
    resourcecontainer->setviewable(state);
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
            if (recipe[i].id == -1)
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
    if (xsize != resourcecontainer->sizex) {
        Assert("size != recipesize");
    }
    if (ysize != resourcecontainer->sizey) {
        Assert("size != recipesize");
    }
    if (bestrecipe==nullptr)
    {
        return;
    }
    for (int i = 0; i < resourcecontainer->databuf.length; i++) {
        if (bestrecipe->recipe[i].id == -1) {
            continue;
        }

        resourcecontainer->databuf[i].helditem->amt -= bestrecipe->recipe[i].amt;
    }
    newitemlocation->update();
}
