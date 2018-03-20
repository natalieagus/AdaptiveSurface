//
//  Point2d.hpp
//  TheEngineSample
//
//  Created by Hans on 28/10/15.
//  Copyright © 2015 A Tasty Pixel. All rights reserved.
//

#ifndef Point2d_hpp
#define Point2d_hpp

#include <stdio.h>

typedef struct Point2d {
    Point2d(float x, float y){this->x = x; this->y = y; this->mark = true;z = 0.0f;};
    Point2d(){x = 0.0f; y = 0.0f; z = 0.0f;};
    float distance(Point2d p);
    float length();
    float dotProduct(Point2d p);
    Point2d normalize();
    Point2d scalarMul(float s);
    Point2d normal();
    
    float x,y,z;
    
    bool mark;
    
    friend Point2d operator-(Point2d lhs, const Point2d& rhs);
    friend Point2d operator+(Point2d lhs, const Point2d& rhs);
    
} Point2d;

#endif /* Point2d_hpp */
