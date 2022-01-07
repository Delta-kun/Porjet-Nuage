#include "vec3.hpp"
#include "mat3.hpp"
#include "fonctions.h"

#include <limits>
#include <fstream>
#include <vector>
#include <algorithm>
#include <math.h>

using namespace cpe;

vec3 raymarching(float jh, float iw, vec3 dir) {
    
    int nbSample = 64;
    int zMax = 40.0;
    float step = zMax/nbSample;
    vec3 p = vec3(jh, iw, 0.0f);
    float T = 1.0f;
    float absorption = 100.0f;
    vec3 color = vec3(0.0f,0.0f,0.0f);

    for(int k = 0; k<nbSample; k++)
    {
        float density = scene(p);
        if (density>0.0f){
            float tmp = density/float(nbSample);
            T *= 1.0f-tmp*absorption;
            if(T<=0.01){
                break;
            }
            color += vec3(1.0f,1.0f,1.0f)*50.0f*tmp*T;
        }
        p += dir*step;
    }

    return color;
}

void render() {
    const int   width    = 1024;
    const int   height   = 768;
    const float fov      = M_PI/3.;
    std::vector<vec3> framebuffer(width*height);

    #pragma omp parallel for
    for (int j = 0; j<height; j++) { // actual rendering loop
        for (int i = 0; i<width; i++) {
            float dir_x =  (i + 0.5) -  width/2.;
            float dir_y = -(j + 0.5) + height/2.;    // this flips the image at the same time
            float dir_z = -height/(2.*tan(fov/2.));

            vec3 dir = vec3(dir_x,dir_y,dir_z);
            framebuffer[i+j*width] = raymarching(float(j)/float(height),float(i)/float(width),dir);
        }
    }

    std::ofstream ofs; // save the framebuffer to file
    ofs.open("./out.ppm", std::ios::binary);
    ofs << "P6\n" << width << " " << height << "\n255\n";
    for (vec3 &c : framebuffer) {
        float max = std::max(c[0], std::max(c[1], c[2]));
        if (max>1) c = c*(1./max);
        ofs << (char)(255 * c[0]) << (char)(255 * c[1]) << (char)(255 * c[2]);
    }
    ofs.close();
}

int main() {
    render();
    return 0;
}

