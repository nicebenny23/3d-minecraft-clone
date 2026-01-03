#pragma once
#include "../game/GameContext.h"
#include "textrender.h"
#include "renderer.h"

void integertext::set_handle()
{
	if (!handle)
	{
		handle = CtxName::ctx.Ren->gen_renderable();
		handle.set_material("Text");
		handle.set_layout(vertice::vertex().push<float, 2>().push<float, 3>());
	}
}
integertext::integertext(v2::Vec2 textcenter, float textscale)
{
	center = textcenter;
	scale = textscale; 
	priority = 105500;
	state.enabled = false;
}

 void integertext::recalculateword()
{
	 word.clear();
	word= std::to_string(value);

}
void integertext::customdestroy()
{

	word.clear();
	if (handle)
	{
		handle.destroy();
	}
}

void integertext::render()
{
	set_handle();
	recalculateword();

	write();
		
	
	//handle.render();
}


void inittextarray()
{

    array<std::string> texlist = array<std::string>(10);
	texlist[0] = "bitmaptext\\zeroimg.png";
	texlist[1] = "bitmaptext\\oneimg.png";
	texlist[2] = "bitmaptext\\twoimg.png";
	texlist[3] = "bitmaptext\\threeimg.png";
	texlist[4] = "bitmaptext\\fourimg.png";
	texlist[5] = "bitmaptext\\fiveimg.png";
	texlist[6] = "bitmaptext\\siximg.png";
	texlist[7] = "bitmaptext\\sevenimg.png";
	texlist[8] = "bitmaptext\\eightimg.png";
	texlist[9] = "bitmaptext\\nineimg.png";
	renderer::texture_array_id textarray= CtxName::ctx.Ecs->load_asset_emplaced<TextureArrayPath>(texlist, std::string("Letters")).unwrap();
	CtxName::ctx.Ren->set_uniform("letters", textarray);
	renderer::shader_id decal_shader=CtxName::ctx.Ecs->load_asset_emplaced<shader_load>( "TextShader", "shaders\\textvertex.vs", "shaders\\textfragment.vs").unwrap();

	CtxName::ctx.Ren->construct_material("Text","ui_phase", decal_shader, renderer::RenderProperties(false, false, false, true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA),
		uniforms::uparam("aspect_ratio", "aspectratio"),
		uniforms::uparam("letters", "tex")

	);
 
   
}
void writeletter(renderer::MeshData& mesh_data,geometry::Box2d location, int letter)
{

	const int baselocation = mesh_data.length();
	for (int j = 0; j < 4; j++) {
		v2::Vec2 pointtoappend = location.scale * offset[j] + location.center;
		mesh_data.add_point(pointtoappend,cubeuv[j],letter);
	}	
	mesh_data.add_index( 0+baselocation);
	mesh_data.add_index(1+ baselocation);
	mesh_data.add_index(3 + baselocation);
	mesh_data.add_index(0 + baselocation);
	mesh_data.add_index(3 + baselocation);
	mesh_data.add_index(2 + baselocation);


}
void integertext::write()
{
	float char_offset = 1.5f;
	v2::Vec2 min = center - (v2::Vec2(char_offset *word.length(), 1.f) * scale) / 2;
	v2::Vec2 boxoffset = v2::Vec2(char_offset, 1) * scale / 2.0;
	v2::Vec2 increse=  v2::Vec2(char_offset,0) * scale;
	geometry::Box2d charlocation = geometry::Box2d(min+boxoffset, v2::unitv * scale);
	renderer::MeshData mesh_data=handle.create_mesh();
	for (int i = 0; i < word.length(); i++)
	{
		writeletter(mesh_data,charlocation, int(word[i]-'0'));
		charlocation.center += increse;
	}
	handle.fill(std::move(mesh_data));
	handle.set_order_key(priority);

	CtxName::ctx.Ren->pop();

}
