#include "cloud.hpp"

namespace cpe
{

// Constructeurs
cloud::cloud()
    :centerData(vec3(0.5f,0.5f,0.0f)),cloudWidthData(1.0f),noiseFreqData(25.0f)
{}

cloud::cloud(vec3 _center, float _cloudWidth, float _noiseFreq)
    :centerData(_center),cloudWidthData(_cloudWidth),noiseFreqData(_noiseFreq)
{}

vec3 cloud::center() const
{
    return centerData;
}

vec3& cloud::center()
{
    return centerData;
}

float cloud::cloudWidth() const
{
    return cloudWidthData;
}

float& cloud::cloudWidth()
{
    return cloudWidthData;
}

float cloud::noiseFreq() const
{
    return noiseFreqData;
}

float& cloud::noiseFreq()
{
    return noiseFreqData;
}





}