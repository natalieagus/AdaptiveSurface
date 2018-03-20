//
//  Vector3D.cpp
//  surfaceIntegration
//
//  Created by Natalie Agus on 31/8/16.
//  Copyright © 2016 Hans. All rights reserved.
//

#include "Vector3D.hpp"

float Vector3D::dotProduct(Vector3D vec2){
    float out = this->x * vec2.x + this->y * vec2.y + this->z * vec2.z;
    return out;
}

Vector3D Vector3D::crossProduct(Vector3D vec2){
    

        Vector3D out = Vector3D(this->y * vec2.z - this->z*vec2.y,
                                this->z*vec2.x - this->x*vec2.z,
                                this->x*vec2.y - this->y*vec2.x);
        return out;
    }
    
Vector3D Vector3D::add(Vector3D vec2){
    Vector3D out = Vector3D(this->x + vec2.x, this->y + vec2.y , this->z + vec2.z);
    return out;
}

Vector3D Vector3D::subtract(Vector3D vec2){
    Vector3D out = Vector3D(this->x - vec2.x, this->y - vec2.y , this->z - vec2.z);
    return out;
}

Vector3D Vector3D::scalarMult(float scalar){
    Vector3D out = Vector3D(this->x * scalar, this->y * scalar, this->z * scalar);
    return out;
}

float Vector3D::magnitude(){
    return sqrtf(powf(x, 2) + powf(y, 2) + powf(z, 2));
}

Vector3D Vector3D::normalize(){
    Vector3D out = Vector3D(this->x/magnitude(), this->y/magnitude(), this->z/magnitude());
    return out;
}

float Vector3D::distance(Vector3D vec2){
    Vector3D d = subtract(vec2);
    return d.magnitude();
}




