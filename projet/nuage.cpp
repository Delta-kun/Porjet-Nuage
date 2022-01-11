#include "vec2.hpp"
#include "vec3.hpp"
#include "mat3.hpp"
#include "vec4.hpp"
#include "fonctions.h"

#include <limits>
#include <fstream>
#include <vector>
#include <algorithm>
#include <math.h>

using namespace cpe;

vec3 raymarching(float jh, float iw, vec3 dir) {
    
    int nbSample = 64;
    int zMax = 40.0f;
    float step = zMax/nbSample;
    vec3 p = vec3(jh, iw, 0.0f);
    float T = 2.0f;
    float absorption = 90.0f;
    vec3 color = vec3(0.0f,0.0f,0.0f);
    //vec4 color = vec4(0.0f,0.0f,0.0f,0.0f);
    //vec4 color = vec4(135.0f,206.0f,235.0f,1.0f)/255; //bleu ciel

    int nbSampleLight = 6;
    float zMaxLight = 20.0f;
    float stepLight = zMaxLight/float(nbSampleLight);
    vec3 sun_direction = normalized(vec3(2.0f,1.0f,1.0f));

    float maxLength = norm(vec3(0.5f,0.5f,0.0f)+nbSample*dir);

    for(int k = 0; k<nbSample; k++)
    {
        float length = norm(p);
        float density = scene(p);
        // density *= filtre_abs(length, maxLength, false); // Application d'un filtre absolu
        density *= filtre_gauss(length, maxLength, false); // Application d'un filtre gaussien
        if (density>0.0f){
            float tmp = density/float(nbSample);
            T *= 1.0f-tmp*absorption;
            if(T<=0.01){
                break;
            }

            // Light
            float Tl = 1.0f;
            float maxLengthLight = norm(p+sun_direction*nbSampleLight);
            for(int l = 0; l<nbSampleLight; l++)
            {
                float lengthLight = norm(p+sun_direction*float(l)*stepLight);
                float densityLight = scene(p+sun_direction*float(l)*stepLight);
                // density *= filtre_abs(lengthLight, maxLengthLight, true); // Filtre absolu
                density *= filtre_gauss(lengthLight, maxLengthLight, true); // Filtre gaussien
                if(densityLight>0.)
                	Tl *= 1. - densityLight * absorption/float(nbSample);
                if (Tl <= 0.01)
                    break;
            }

            // Color ambiant & light
            color += vec3(1.0f,1.0f,1.0f)*70.0f*tmp*T + vec3(0.9f,0.9f,0.85f)*8.0*tmp*T*Tl;
        }
        p += dir*step;
    }

    return color;
}

void render() {
    const int   width    = 512;
    const int   height   = 384;
    const float fov      = M_PI/3.;
    std::vector<vec3> framebuffer(width*height);

    const vec2 offSet(-0.0f,-0.0f);

    #pragma omp parallel for
    for (int j = 0; j<height; j++) { // actual rendering loop
        for (int i = 0; i<width; i++) {
            float dir_x =  (i + 0.5) -  width/2.;
            float dir_y = -(j + 0.5) + height/2.;    // this flips the image at the same time
            float dir_z = -height/(2.*tan(fov/2.));

            vec3 dir = vec3(dir_x,dir_y,dir_z);
            framebuffer[i+j*width] = raymarching(float(j)/float(height)+offSet.x(), float(i)/float(width)+offSet.y(), normalized(dir));
        }
    }

    std::ofstream ofs; // save the framebuffer to file
    ofs.open("./out.ppm", std::ios::binary);
    ofs << "P6\n" << width << " " << height << "\n255\n";
    for (vec3 &c : framebuffer) {
        float max = std::max(c[0], std::max(c[1], c[2]));
        if (max>1) c = c/max;
        ofs << (char)(255 * c[0]) << (char)(255 * c[1]) << (char)(255 * c[2]);
    }
    ofs.close();
}

int main() {
    render();
    return 0;
}

