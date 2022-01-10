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
    f  = 0.4000*noise( p ); p = m*p*2.02;
    f += 0.2000*noise( p ); p = m*p*2.03;
    f += 0.1500*noise( p );
    return f;
}

float scene(vec3 p)
{
    float length = sqrt(pow(p.x(),2)+pow(p.y(),2)+pow(p.z(),2));
    float maxLength = sqrt(3.0f);

    float filtre = 1.0f - std::fabs(length/maxLength);

    float cloudWidth = 1.5f; //dézoom sur le nuage
    float noiseFreq = 25.f; //facteur de fréquence sur le bruit perlin
    return filtre*(-0.1f- length*cloudWidth + fbm(p*noiseFreq));
}

