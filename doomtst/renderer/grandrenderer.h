#pragma once
#include "../game/camera.h"
#include "../renderer/ModelMesh.h"
#include "../renderer/Window.h"
#include "../renderer/renderer.h"
#include "../game/particles.h"
#include "../renderer/blockrender.h"
#include "../renderer/uibox.h"
#include "../renderer/textrender.h"
#include "../renderer/model.h"
#include "guirender.h"
#include "../renderer/decal.h"

void rendergame() {

    CtxName::ctx.OC->updatecomponents(gameobject::Rendercall);
    blockrender::renderblocks(false);
    render_decals();
    //apply postprossising
    ui::updateui();
    guirender::rendergui();

}
