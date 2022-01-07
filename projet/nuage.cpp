#include "geometry.h"

#include <limits>
#include <fstream>
#include <vector>
#include <algorithm>

void render(){
    const int   width    = 1024;
    const int   height   = 768;
    const float fov      = M_PI/3.;
    std::vector<vec3> framebuffer(width*height);

    #pragma omp parallel for
    for (size_t j = 0; j<height; j++) { // actual rendering loop
        for (size_t i = 0; i<width; i++) {
            // float dir_x =  (i + 0.5) -  width/2.;
            // float dir_y = -(j + 0.5) + height/2.;    // this flips the image at the same time
            // float dir_z = -height/(2.*tan(fov/2.));
            // framebuffer[i+j*width] = cast_ray(vec3{0,0,0}, vec3{dir_x, dir_y, dir_z}.normalize(), spheres, lights);
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