#include "recipe.h"
#include "../util/time.h"
void recipemanager::destroy() {

    newitemlocation->destroy();
    resourcecontainer->destroy();
}
//todo add proper previews
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



autocraftstatetype recipemanager::autocraft()
{
    currecipe = searchrecipe();
    state.craftedthisframe = false;
    if (currecipe != nullptr)
    {


        if (newitemlocation->at(0).helditem != nullptr)
        {

            if (currecipe->itemcreated.id == newitemlocation->at(0).helditem->id)
            {
                //if it h
                
            
                if (newitemlocation->at(0).helditem->canadd(currecipe->itemcreated.amt))
                {
                    if (attributes.timetillcraft<0)
                    {
                        craft();
                        newitemlocation->at(0).helditem->amt += currecipe->itemcreated.amt;
                        newitemlocation->at(0).helditem->updateui();
                       
                        return crafted;
                    }
                    return iscrafting;
                }
            }
        }
        if (attributes.timetillcraft < 0)
        {
            craft();
            newitemlocation->at(0).giveitem(currecipe->itemcreated.id, currecipe->itemcreated.amt);
            return crafted;

        }
        return iscrafting;
    }
    return notcrafting;
}
//feture exclusive to normal crafting
void recipemanager::preview() {

    bool shouldredorecipe = !previewvalid();
    bool cancheck = state.craftedthisframe && newitemlocation->at(0).empty();
    shouldredorecipe |= cancheck;
        //if preview is invalid or if we have crafted this frame
        if (shouldredorecipe)
        {
            
            if (!isitempreview())
            {
          //      newitemlocation->update();
                
            }
   
            
            newitemlocation->databuf[0].destroyitem();
            currecipe = searchrecipe();
            if (currecipe != nullptr)
            {


                    newitemlocation->databuf[0].giveitem(currecipe->itemcreated.id, currecipe->itemcreated.amt);

            }
        }
        if (newitemlocation->at(0).helditem!=nullptr)
        {
            newitemlocation->at(0).helditem->updateui();

            
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
    if (attributes.isauto)
    {
        autoupdate();
        return;
    }
        if (state.enabled)
        {
            resourcecontainer->update();
            
               
                if (state.cancraft)
                {


                    newitemlocation->deletebelowzero();


                    preview();
                    state.craftedthisframe = false;
                    if (currecipe != nullptr)
                    {
                        if (newitemlocation->clicked())
                        {
                            if (freeditem==nullptr)
                            {

                                craft();


                                newitemlocation->update();
                            }
                          
                        }
                    }

                        
                    
                }
              
        }
        else
        {
            state.craftedthisframe = false;
        }
    
}

void recipemanager::autoupdate()
{
  
        if (state.enabled)
        {
            resourcecontainer->update();
            newitemlocation->update();
        }

        if (state.cancraft)
        {

            autocraftstatetype currstate = autocraft();
            if (currstate!=notcrafting)
            {

                attributes.timetillcraft -= timename::dt;
              
            }
            if (currstate!=iscrafting)
            {
attributes.timetillcraft = attributes.timetocraft;
            }
            if (currstate == crafted)
            {
                state.craftedthisframe = true;
            }
            else {
                state.craftedthisframe = false;
            }


        }
        return;
    

}

recipemanager::recipemanager(const char* filename, int sizex, int sizey)
{
recipelist= new array<irecipe>();
xsize = sizex;
ysize = sizey;
currecipe = nullptr;
   safefile recipefile= safefile(filename, fileread);
   createcontainers();
   char line[512];
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
       if (line[0] == ' ') {
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

bool recipemanager::previewvalid()
{
    
    if (currecipe==nullptr)
    {
        return false;
    }
    return currecipe->cancraft(resourcecontainer);
}

bool irecipe::cancraft(Container* resourcecont,bool exact) {
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
        if (exact)
        {

            if (itematpos->amt !=recipe[i].amt) {
                return false;
            }
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
  
    
    for (int i = 0; i < resourcecontainer->databuf.length; i++) {
        if (currecipe->recipe[i].id == 0) {
            continue;
        }
        if (resourcecontainer->databuf[i].helditem->itemtype==wear)
        {
            resourcecontainer->databuf[i].destroyitem();
        }
        else
        {

            resourcecontainer->databuf[i].helditem->amt -= currecipe->recipe[i].amt;
        }
    }
    //lets the player swap
    state.craftedthisframe = true;
    
}

bool recipemanager::isitempreview()
{
    //no recipe can give null item
    if (currecipe==nullptr)
    {
        return false;
    }
    //no recipe can give null item
    if (newitemlocation->at(0).helditem==nullptr)
    {
        return false;
    
    }
    if (currecipe->itemcreated.amt==newitemlocation->at(0).helditem->amt)
    {
        if (currecipe->itemcreated.id == newitemlocation->at(0).helditem->id)
        {
            return true;
        }
    }
    return  false;
}
