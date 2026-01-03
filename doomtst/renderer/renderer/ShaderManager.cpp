#pragma once
#include "ShaderManager.h"
#include "../../assets/Assets.h"
#include "../../game/GameContext.h"

void renderer::ww() {
	CtxName::ctx.Ecs->emplace_asset_loader<shader_loader>();

}
