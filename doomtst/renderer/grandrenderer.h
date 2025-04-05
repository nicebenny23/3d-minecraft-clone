#include "../game/camera.h"
#include "../renderer/mesh.h"
#include "../renderer/shader.h"

#include "../renderer/texture.h"
#include "../renderer/Window.h"

#include "../renderer/renderer.h"


#include "../game/particles.h"
#include "../renderer/blockrender.h"


#include "../renderer/uibox.h"

#include "../renderer/textrender.h"


#include "../renderer/model.h"
#include "guirender.h"
#ifndef grandrenderer_HPP
#define grandrenderer_HPP

void rendergame() {

    entityname::runrenderloop();
    blockrender::renderblocks(false);

    //apply postprossising
    ui::updateui();
    guirender::rendergui();

}

#endif 