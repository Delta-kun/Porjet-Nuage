#include "vec2.hpp"
#include "vec3.hpp"
#include "vec4.hpp"
#include "mat3.hpp"
#include "fonctions.h"

#include <limits>
#include <fstream>
#include <vector>
#include <algorithm>
#include <math.h>

vec3 cast_ray(const vec3 &orig, const vec3 &dir, const std::vector<Sphere> &spheres, const std::vector<Light> &lights, size_t depth=0) {
    vec3 point, N;
    Material material;

    if (depth>4 || !scene_intersect(orig, dir, spheres, point, N, material))
        return vec3{0.2, 0.7, 0.8}; // background color

    vec3 reflect_dir = reflect(dir, N).normalize();
    vec3 refract_dir = refract(dir, N, material.refractive_index).normalize();
    vec3 reflect_color = cast_ray(point, reflect_dir, spheres, lights, depth + 1);
    vec3 refract_color = cast_ray(point, refract_dir, spheres, lights, depth + 1);

    float diffuse_light_intensity = 0, specular_light_intensity = 0;
    for (const Light light : lights) {
        vec3 light_dir      = (light.position - point).normalize();

        vec3 shadow_pt, trashnrm;
        Material trashmat;
        if (scene_intersect(point, light_dir, spheres, shadow_pt, trashnrm, trashmat) &&
                (shadow_pt-point).norm() < (light.position-point).norm()) // checking if the point lies in the shadow of the light
            continue;

        diffuse_light_intensity  += light.intensity * std::max(0.f, light_dir*N);
        specular_light_intensity += std::pow(std::max(0.f, -reflect(-light_dir, N)*dir), material.specular_exponent)*light.intensity;
    }
    return material.diffuse_color * diffuse_light_intensity * material.albedo[0] + vec3{1., 1., 1.}*specular_light_intensity * material.albedo[1] + reflect_color*material.albedo[2] + refract_color*material.albedo[3];
}

void render(const std::vector<Sphere> &spheres, const std::vector<Light> &lights) {
    const int   width    = 1024;
    const int   height   = 768;
    const float fov      = M_PI/3.;
    std::vector<vec3> framebuffer(width*height);

    int nbSample = 64;
    int zMax = 40.0;

    #pragma omp parallel for
    for (size_t j = 0; j<height; j++) { // actual rendering loop
        for (size_t i = 0; i<width; i++) {
            float dir_x =  (i + 0.5) -  width/2.;
            float dir_y = -(j + 0.5) + height/2.;    // this flips the image at the same time
            float dir_z = -height/(2.*tan(fov/2.));
            framebuffer[i+j*width] = cast_ray(vec3{0,0,0}, vec3{dir_x, dir_y, dir_z}.normalize(), spheres, lights);
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
    render(spheres, lights);
    return 0;
}

