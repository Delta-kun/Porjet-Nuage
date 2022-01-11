#pragma once

#ifndef CLOUD_HPP
#define CLOUD_HPP

#include "vec3.hpp"

namespace cpe
{

class cloud
{
    private : 

        vec3 centerData;
        float cloudWidthData;
        float noiseFreqData;

    public :

        // Constructeurs
        cloud();
        cloud(vec3 _center, float _cloudWidth, float _noiseFreq);

        //Get/Set

        /** Get center coordinate */
        vec3 center() const;
        /** Get/Set center coordinate */
        vec3& center();
        /** Get cloud width factor coordinate */
        float cloudWidth() const;
        /** Get/Set cloud width factor coordinate */
        float& cloudWidth();
        /** Get noise frequency factor coordinate */
        float noiseFreq() const;
        /** Get/Set noise frequency factor coordinate */
        float& noiseFreq();

};


}

#endif