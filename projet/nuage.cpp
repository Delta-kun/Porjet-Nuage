#include "vec2.hpp"
#include "vec3.hpp"
#include "mat3.hpp"
#include "vec4.hpp"
#include "fonctions.h"

#include <chrono>
#include <limits>
#include <fstream>
#include <vector>
#include <algorithm>
#include <math.h>

using namespace cpe;

vec3 raymarching(float jh, float iw, vec3 dir, float angle, std::vector<cloud> Clouds) {
    
    int nbSample = 64;
    float zMax = 1.0f;
    float step = zMax/float(nbSample);
    float T = 2.0f;
    float absorption = 220.0f;
    float b = 2.0f;
    vec3 color = vec3(0.0f,0.0f,0.0f);
    //vec4 color = vec4(0.0f,0.0f,0.0f,0.0f);
    //vec4 color = vec4(135.0f,206.0f,235.0f,1.0f)/255; //bleu ciel
    vec3 p;

    if(angle <= M_PI/2){
        p = vec3(jh*cos(angle)+(1.0f-jh)*sin(angle),iw,jh*sin(angle));
    }
    else if(angle <= M_PI){
        p = vec3((1.0f-jh)*sin(angle)-cos(angle),iw,jh*sin(angle)-cos(angle));
    }
    else if(angle <= 3*M_PI/2){
        p = vec3(-jh*cos(angle)-jh*sin(angle),iw,-cos(angle)-(1.0f-jh)*sin(angle));
    }
    else {
        p = vec3(-jh*sin(angle)+jh*cos(angle),iw,-(1.0f-jh)*sin(angle));
    }

    int nbSampleLight = 6;
    float zMaxLight = 40.0f;
    float stepLight = zMaxLight/float(nbSampleLight);
    vec3 sun_direction = normalized(vec3(2.0f,1.0f,1.0f));
    float bLight = 0.0f;

    float maxLength = norm(nbSample*dir);

    for(int k = 0; k<nbSample; k++)
    {
        float length = norm(p);
        float density = scene(p,b,Clouds);
        //density *= filtre_abs(length, maxLength, false); // Application d'un filtre absolu
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
                float densityLight = scene(p+sun_direction*float(l)*stepLight,bLight,Clouds);
                //densityLight *= filtre_abs(lengthLight, maxLengthLight, true); // Filtre absolu
                densityLight *= filtre_gauss(lengthLight, maxLengthLight, true); // Filtre gaussien
                if(densityLight>0.)
                	Tl *= 1. - densityLight * absorption/float(nbSample);
                if (Tl <= 0.01)
                    break;
            }

            // Color ambiant & light
            color += vec3(1.0f,1.0f,1.0f)*50.0f*tmp*T + vec3(0.9f,0.9f,0.85f)*80.0*tmp*T*Tl;
        }
        p += dir*step;
    }

    return color;
}

void render() {

    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point end;
    float total_start = 0.0f;
    float diff;

    const int   width    = 512;
    const int   height   = 384;
    const float fov      = M_PI/3.;
    std::vector<cloud> Clouds = CloudsCreation();
    int nbAngles = 60;
    std::vector<vec3> framebuffer(width*height);

    start = std::chrono::high_resolution_clock::now();

    for (int k = 0; k<nbAngles; k++)
    {
        float angle = float(k*2*M_PI)/float(nbAngles);

        #pragma omp parallel for
        for (int j = 0; j<height; j++) { // actual rendering loop
            for (int i = 0; i<width; i++) {
                float dir_x =  float(i)-float(width)/2.;
                float dir_y = float(-j)+float(height)/2.;    // this flips the image at the same time
                float dir_z = float(height)/(2.*tan(fov));

                vec3 dir(dir_x*cos(angle)-dir_z*sin(angle),dir_y,dir_z*cos(angle)+dir_x*sin(angle));

                framebuffer[i+j*width] = raymarching(float(j)/float(height), float(i)/float(width), normalized(dir), angle, Clouds);
            }
        }

        std::ofstream ofs; // save the framebuffer to file
        if(k<10) ofs.open("../Pictures/Gif/out0"+std::to_string(k)+".ppm", std::ios::binary);
        else ofs.open("../Pictures/Gif/out"+std::to_string(k)+".ppm", std::ios::binary);
        ofs << "P6\n" << width << " " << height << "\n255\n";
        for (vec3 &c : framebuffer) {
            float max = std::max(c[0], std::max(c[1], c[2]));
            if (max>1){
                c = vec3(1.0f,0.0f,0.0f);
                max = 1;
            }
            c = max * vec3(1.0f,1.0f,1.0f) + (1-max) * vec3(135.0f,206.0f,235.0f)/255;
            ofs << (char)(255 * c[0]) << (char)(255 * c[1]) << (char)(255 * c[2]);
        }
        ofs.close();

        end = std::chrono::high_resolution_clock::now();
        diff = float((end-start).count()) / pow(10,9);
        std::cout<<"Image "+std::to_string(k+1)+": "<<diff<<" s"<<std::endl;
        total_start += diff;
        start = end;
    }

    std::cout<<"Temps total de construction : "<<total_start/60.0f<<" min"<<std::endl;
}

int main() {
    render();
    return 0;
}

