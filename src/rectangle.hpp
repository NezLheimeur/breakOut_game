#ifndef RECTANGLE_H
#define RECTANGLE_H
#include "vec2.hpp"

struct rectangle
{
    /** center coordinate */
    vec2 position; // somment en haut a gauche
    /** size  */
    float height;
    float width;

    rectangle();

    /** Constructor circle ({x,y},R) */
    rectangle(vec2 const& position_param, float const width_param, float const  height_param);
};

#endif // RECTANGLE_H
