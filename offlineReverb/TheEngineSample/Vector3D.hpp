//
//  Vector3D.hpp
//  surfaceIntegration
//
//  Created by Natalie Agus on 31/8/16.
//  Copyright © 2016 Hans. All rights reserved.
//

#ifndef Vector3D_hpp
#define Vector3D_hpp
#include <math.h>
#include <stdio.h>
#define HEIGHT 1.7f

typedef struct Vector3D {
    
    //Constructor without arguments
    Vector3D(){
        this->x = 0.0f;
        this->y = 0.0f;
        this->z = HEIGHT;
    };
    
    //Constructor with arguments
    Vector3D(float x, float y, float z){
        this->x = x;
        this->y = y;
        this->z = z;
    }
    
    Vector3D(float x, float y){
        this->x = x;
        this->y = y;
        this->z = HEIGHT; //if not set, then default 1.6 metres from the floor
    }
    

    
    //Methods
    float dotProduct(Vector3D vec2);
    Vector3D crossProduct(Vector3D vec2);
    Vector3D add(Vector3D vec2);
    Vector3D scalarMult(float scalar);
    float magnitude();
    float distance(Vector3D vec2);
    Vector3D normalize();
    Vector3D subtract(Vector3D vec2);
    
    //Variables
    float x,y,z;
    bool mark = true;

} Vector3D;

#endif /* Vector3D_hpp */
