#include "../game/camera.h"
#include "../renderer/ModelMesh.h"
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

    CtxName::ctx.OC->updatecomponents(gameobject::Rendercall);
    blockrender::renderblocks(false);

    //apply postprossising
    ui::updateui();
    guirender::rendergui();

}

#endif 