#pragma once
#include "../game/GameContext.h"
#include "textrender.h"
#include "renderer.h"
TextureArray* textarray;
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
	

}

stn::array<float> datbuf;
stn::array<unsigned int> indbuf;
void integertext::render()
{
	indbuf = stn::array<unsigned int>();
	datbuf = stn::array<float>();
	CtxName::ctx.Ren->SetType("Text");
	recalculateword();

	write();
	Mesh text;
	text.Voa.attributes.push<float,2>().push<float,3>();


	CtxName::ctx.Ren->Gen(&text);
	CtxName::ctx.Ren->context.Bind(*textarray);
	CtxName::ctx.Ren->Render(&text, datbuf, indbuf);
	datbuf.destroy();
	indbuf.destroy();
	CtxName::ctx.Ren->Destroy(&text);
}


void inittextarray()
{

    array<const char*> texlist = array<const char*>(10);

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
    textarray = CtxName::ctx.Ren->Textures.GetTexArray(texlist,"Letters");
   
}
void writeletter(geometry::Box2d location, int letter)
{
	const int baselocation = datbuf.length / 5;
	for (int j = 0; j < 4; j++) {
		// Index of unique vertex in each face
		v2::Vec2 pointtoappend = location.scale * offset[j] + location.center;
		// Actual location
		datbuf.push(pointtoappend.x);

		datbuf.push(pointtoappend.y);

		// UV coordinates
		v2::Vec2 coords = cubeuv[j];
		datbuf.push(coords.x);
		datbuf.push(coords.y);

		datbuf.push(letter);

		
	}	

	indbuf.push( 0+baselocation);
	indbuf.push(1+ baselocation);
	indbuf.push(3 + baselocation);
	indbuf.push(0 + baselocation);
		indbuf.push(3 + baselocation);
	indbuf.push(2 + baselocation);


}
void integertext::write()
{
	v2::Vec2 min = center - (v2::Vec2(1.5f*word.length(), 1.f) * scale) / 2;
	v2::Vec2 boxoffset = v2::Vec2(1.5, 1) * scale / 2.0;
	v2::Vec2 increse=  v2::Vec2(1.5,0) * scale;
	geometry::Box2d charlocation = geometry::Box2d(min+boxoffset, v2::unitv * scale);
	for (int i = 0; i < word.length(); i++)
	{
		writeletter(charlocation, int(word[i]-'0'));
		charlocation.center += increse;
	}

}
