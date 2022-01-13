#include <iostream>
#include "mat3.hpp"
#include "vec3.hpp"
#include "vec4.hpp"
#include "cloud.hpp"
#include <vector>

float linterp(float a, float b, float t);

float hash(float n);
float noise(cpe::vec3 x);
float fbm(cpe::vec3 p);

float filtre_abs(float length, float maxL, bool light);
float filtre_gauss(float length, float maxL, bool light);

std::vector<cpe::cloud> CloudsCreation();

float scene(cpe::vec3 p, float b, std::vector<cpe::cloud> Clouds);


