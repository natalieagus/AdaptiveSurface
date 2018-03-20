//
//  Point2d.cpp
//  TheEngineSample
//
//  Created by Hans on 28/10/15.
//  Copyright © 2015 A Tasty Pixel. All rights reserved.
//

#include "Point2d.hpp"
#include <math.h>

float Point2d::distance(Point2d p){
    float dx = p.x - x;
    float dy = p.y - y;
    return sqrtf(dx*dx + dy*dy);
}

float Point2d::length(){
    return distance(Point2d()); // get distance from this to zero
}

float Point2d::dotProduct(Point2d p){
    return x*p.x + y*p.y;
}

Point2d Point2d::normal(){
    return Point2d(y, -x);
}

Point2d operator-(Point2d lhs, const Point2d& rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    return lhs;
}

Point2d operator+(Point2d lhs, const Point2d& rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    return lhs;
}

Point2d Point2d::scalarMul(float s){
    return Point2d(this->x * s, this->y * s);
}

Point2d Point2d::normalize(){
    float length = sqrtf(x*x + y*y);
    x /= length;
    y /= length;
    return *this;
}