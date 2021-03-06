
#pragma once

#ifndef CIRCLE_HPP
#define CIRCLE_HPP

#include <iostream>

#include "vec2.hpp"


/** A structure containing parameter of a geometric circle */
struct circle
{
    /** center coordinate */
    vec2 center;
    /** radius coordinate */
    float radius;

    /** Constructor circle (0,0) */
    circle();
    /** Constructor circle ({x,y},R) */
    circle(vec2 const& center_param,float radius_center);

};

/** Check if a point is inside the circle or not */
bool point_inside_circle(vec2 const& p,circle const& c);



#endif
