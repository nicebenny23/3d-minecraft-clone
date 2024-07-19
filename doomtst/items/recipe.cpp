#include "recipe.h"

void recipemanager::destroy() {
    newitemlocation->destroy();
    resourcecontainer->destroy();
}

recipemanager::recipemanager(int orginxsize, int orginysize, array<irecipe>& recipes)
    : recipelist(recipes) {
    newitemlocation = new Container(1, 1, .5, .5);
    resourcecontainer = new Container(orginxsize, orginysize, 3, 3);

    for (int i = 0; i < recipelist.length; i++) {
        if (recipelist[i].xsize != resourcecontainer->sizex) {
            Assert("xsize not correct for recipe manager");
        }
        if (recipelist[i].ysize != resourcecontainer->sizey) {
            Assert("ysize not correct for recipe manager");
        }
    }
}

void recipemanager::ontake() {
    // Implementation needed
}

void recipemanager::preview() {
    irecipe* todisplay = searchrecipe();
    newitemlocation->databuf[0].giveitem(todisplay->itemcreated.id);
}

irecipe* recipemanager::searchrecipe() {
    int maxamt = 0;
    irecipe* maximal = nullptr;

    for (int i = 0; i < recipelist.length; i++) {
        if (!recipelist[i].cancraft(resourcecontainer)) {
            continue;
        }

        int recipeamt = 0;
        for (int j = 0; j < xsize*ysize; j++) {
            if (recipelist[i].recipe[j].id != -1) {
                recipeamt += 1;
            }
        }

        if (recipeamt > maxamt) {
            maxamt = recipeamt;
            maximal = &recipelist[i];
        }
    }

    return maximal;
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

        if (recipe[i].id == -1 && itematpos == nullptr) {
            continue;
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

iteminrecipe irecipe::craft(Container* resourcecont) {
    if (xsize != resourcecont->sizex) {
        Assert("size != recipesize");
    }
    if (ysize != resourcecont->sizey) {
        Assert("size != recipesize");
    }

    for (int i = 0; i < resourcecont->databuf.length; i++) {
        if (recipe[i].id == -1) {
            continue;
        }

        resourcecont->databuf[i].helditem->amt -= recipe[i].amt;
    }

    return itemcreated;
}
