#include "textrender.h"
#include "renderer.h"
texturearray textarray;
integertext::integertext(v2::Vector2 textcenter, float textscale)
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

array<float> datbuf;
array<unsigned int> indbuf;
void integertext::render()
{
	indbuf = array<unsigned int>();
	datbuf = array<float>();
	renderer::changerendertype(renderer::rendertext);
	recalculateword();

	write();
	vao Voa = vao();
	vbuf VBO = vbuf();
	vbuf ibo = vbuf();
	Voa.generate();
	VBO.generate(GL_ARRAY_BUFFER);
	ibo.generate(GL_ELEMENT_ARRAY_BUFFER);
	textarray.apply();
	renderer::render2dtextarray(Voa, ibo, VBO, datbuf, indbuf);
	datbuf.destroy();
	indbuf.destroy();
	Voa.destroy();
	VBO.destroy();
	ibo.destroy();
}


void inittextarray()
{

    array<const char*> texlist = array<const char*>();
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
    textarray = texturearray(16, 16, texlist);
   
}
void writeletter(geometry::Box2d location, int letter)
{
	const int baselocation = datbuf.length / 5;
	for (int j = 0; j < 4; j++) {
		// Index of unique vertex in each face
		v2::Vector2 pointtoappend = location.scale * offset[j] + location.center;
		// Actual location
		datbuf.append(pointtoappend.x);

		datbuf.append(pointtoappend.y);

		// UV coordinates
		v2::Vector2 coords = cubeuv[j];
		datbuf.append(coords.x);
		datbuf.append(coords.y);

		datbuf.append(letter);

		
	}	

	indbuf.append( 0+baselocation);
	indbuf.append(1+ baselocation);
	indbuf.append(3 + baselocation);
	indbuf.append(0 + baselocation);
		indbuf.append(3 + baselocation);
	indbuf.append(2 + baselocation);


}
void integertext::write()
{
	v2::Vector2 min = center - (v2::Vector2(1.5f*word.length(), 1.f) * scale) / 2;
	v2::Vector2 boxoffset = v2::Vector2(1.5, 1) * scale / 2.0;
	v2::Vector2 increse=  v2::Vector2(1.5,0) * scale;
	Box2d charlocation = Box2d(min+boxoffset, v2::unitv * scale);
	for (int i = 0; i < word.length(); i++)
	{
		writeletter(charlocation, int(word[i]-'0'));
		charlocation.center += increse;
	}

}
