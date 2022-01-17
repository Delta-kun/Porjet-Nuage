#include <iostream>
#include <math.h>
#include <cmath>
#include <vector>
#include "cloud.hpp"
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
    f  = 0.6000*noise( p ); p = m*p*2.03;
    f += 0.3500*noise( p ); p = m*p*2.04;
    f += 0.2000*noise( p );
    return f;
}

float filtre_abs(float length, float maxL, bool light)
{
    if(light){
        return 0.8f*std::fabs(length/maxL);
    } else {
        return 0.6f*(1.0f-std::fabs(length/maxL));
    }
}

float filtre_gauss(float length, float maxL, bool light)
{
    if(light){
        return 1.0f-std::exp(-std::fabs(length/maxL));
    } else {
        return 0.4f*std::exp(-std::fabs(length/maxL));
    }
}

std::vector<cloud> CloudsCreation()
{
    //Creation Clouds
    std::vector<cloud> Clouds;

    vec3 c1(0.25f,0.5f,0.35f);
    float w1 = 1.5f;
    float f1 = 25.0f;
    cloud nuage1(c1,w1,f1);

    Clouds.push_back(nuage1);

    vec3 c2(0.75f,0.5f,0.65f);
    float w2 = 1.3f;
    float f2 = 20.0f;
    cloud nuage2(c2,w2,f2);

    Clouds.push_back(nuage2);

    return Clouds;
}

float scene(vec3 p, float b, std::vector<cloud> Clouds)
{
    float l;
    float blinn = 1.0f;
    // float l_min = 100.0f; //Utile pour distance min
    float somme = 0.0f;

    std::vector<cloud>::iterator it;

    it = Clouds.begin();

    while(it != Clouds.end())
    {
        l = norm(p - it->center());
        if(b != 0.0f) blinn = std::exp(-b*std::pow(l,2));
        somme = somme + blinn*(-l * it->cloudWidth() + fbm(p * it->noiseFreq()));
        it++;
    }

    return  somme; 
}

