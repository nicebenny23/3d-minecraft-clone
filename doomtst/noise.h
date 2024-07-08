#include <math.h>
#include "random.h"
#include "vector2.h"
#ifndef noise_HPP
#define noise_HPP



float interpolate(float v, float v1, float w) {
    
    return (v * w + v1*(1-w));
   
}



v2::Vector2 randomGradient(iv2::IntVector2 intvec) {
    
    const unsigned w = 8 * sizeof(unsigned);
    const unsigned s = w / 2;
    unsigned a = intvec.x, b = intvec.y;
    a *= 3284157443; b ^= a << s | a >> w - s;
    b *= 1911520717; a ^= b << s | b >> w - s;
    a *= 2048419325;
    float random = a * (3.14159265 / ~(~0u >> 1)); 
    v2::Vector2 v;
    v.x = cos(random); v.y = sin(random);
    return v;
}
float dotGridGradient(iv2::IntVector2 pos, float x, float y) {

    v2::Vector2 gradient = randomGradient(pos);

   
    float dx = x - pos.x;
    float dy = y - pos.y;

    return (dx * gradient.x + dy * gradient.y);
}


float perlin(float x, float y) {
    
    int x0 = (int)floor(x);
    int x1 = x0 + 1;
    int y0 = (int)floor(y);
    int y1 = y0 + 1;

  
    float sx = x - (float)x0;
    float sy = y - (float)y0;

   
    float n0, n1, ix0, ix1, value;

    n0 = dotGridGradient(iv2::IntVector2(x0, y0), x, y);
    n1 = dotGridGradient(iv2::IntVector2(x1, y0), x, y);
    ix0 = interpolate(n0, n1, sx);

    n0 = dotGridGradient(iv2::IntVector2(x0, y1), x, y);
    n1 = dotGridGradient(iv2::IntVector2(x1, y1), x, y);
    ix1 = interpolate(n0, n1, sx);

    value = interpolate(ix0, ix1, sy);
    return value;
}
float trueperlin(float x, float y) {
    float todob = 1;
    float val = 0;
    for (int i = 0; i < 8; i++)
    {
        x /= 2;
        y /= 2;
        val+=todob*perlin(x+.14156, y+.14159);
      
        todob *= 2;
    }
    val /= 128;
    return val;
}
#endif // !noise_HPP
