#include "rectangle.hpp"
#include <cmath>

rectangle::rectangle()
    :position(),height(0.0f),width(0.0f)
{}

rectangle::rectangle(vec2 const& position_param,float const width_param,float const  height_param)
    :position(position_param),height(height_param),width(width_param)
{}

