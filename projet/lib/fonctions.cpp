#include <iostream>
#include <math.h>
#include <cmath>
#include "fonctions.h"

using namespace cpe;

mat3 m = mat3(0.0f,  0.8f,   0.6f,
             -0.8f,  0.36f, -0.48f,
             -0.6f, -0.48f,  0.64f);

float linterp(float a, float b, float t)
{
    return a+t*(b-a);
}

float hash(float n)
{
    float x = sin(n)*43758.5453;
    return 0.8*(x-floor(x))+0.2;
}

float noise (vec3 x)
{
    vec3 p = vec3(floor(x.x()),floor(x.y()),floor(x.z()));
    vec3 f = x-p;

    vec3 f_tmp= 2.0f*f; 
    f_tmp = 3.0f-f_tmp;
    f = f*f*f_tmp;

    float n = p.x() + p.y() * 57.0f + 113.0f * p.z();

    float res = linterp(
        linterp(
            linterp(hash(n+0.0f),hash(n+1.0f),f.x()),
            linterp(hash(n+57.0f),hash(n+58.0f),f.x()),
            f.y()),
        linterp(
            linterp(hash(n+113.0f),hash(n+114.0f),f.x()),
            linterp(hash(n+170.0f),hash(n+171.0f),f.x()),
            f.y()),
        f.z());

    return res;
}

float fbm(vec3 p)
{
    float f;
    f  = 0.4500*noise( p ); p = m*p*2.5;
    f += 0.2000*noise( p ); p = m*p*2.0;
    f += 0.0500*noise( p );
    return f;
}

float filtre_abs(float length, float maxL, bool light)
{
    if(light){
        return 0.2f*(1.0f-std::fabs(length/maxL));
    } else {
        return 0.2f*(std::fabs(length/maxL)+0.1f);
    }
}

float filtre_gauss(float length, float maxL, bool light)
{
    if(light){
        return 0.02f*(1.0f-std::exp(-std::fabs(length/maxL)));
    } else {
        return 0.02f*std::exp(-std::fabs(length/maxL));
    }
}

float scene(vec3 p)
{
    //Creation Clouds
    std::vector<cloud> Clouds;

    vec3 c(0.5f,0.30f,0.0f);
    float w = 1.0f;
    float f = 25.0f;
    cloud nuage1(c,w,f);

    Clouds.push_back(nuage1);

    vec3 c2(0.5f,0.70f,0.0f);
    float w2 = 1.0f;
    float f2 = 2.0f;
    cloud nuage2(c2,w2,f2);

    Clouds.push_back(nuage2);

    float l;
    // float l_min = 100.0f; //Utile pour distance min
    float somme = 0.0f;

    std::vector<cloud>::iterator it;
    std::vector<cloud>::iterator it_min;

    it = Clouds.begin();

    while(it != Clouds.end())
    {
        l = norm(p - it->center());
        somme = somme -l * it->cloudWidth() + fbm(p * it->noiseFreq());
        it++;
    }

    return  somme; 
}

