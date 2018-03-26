//
//  Plane3D.hpp
//  surfaceIntegration
//
//  Created by Natalie Agus on 31/8/16.
//  Copyright © 2016 Hans. All rights reserved.
//

#ifndef Plane3D_hpp
#define Plane3D_hpp

#include <stdio.h>
#include "Vector3D.hpp"
#include <math.h>
#include <string>

typedef struct Plane3D {
    
    //Constructor without arguments
    Plane3D(){
        this->corner = Vector3D();
        this->S1 = Vector3D();
        this->S2 = Vector3D();
        this->normal = this->S1.crossProduct(S2).normalize();
        this->subdivided = 0;
        this->width = S1.magnitude();
        this->height = S2.magnitude();
        this->area = getArea();
    };
    
    //Destructor
    ~Plane3D() = default;
    
    /*Constructor with arguments
     *@params   set direction vectors S1 and S2 such that S1 cross S2 is the surface's normal
     */
    Plane3D(Vector3D corner, Vector3D S1, Vector3D S2){
        
//        this->corner = corner;
//        this->S1 = S1;
//        this->S2 = S2;
        
        //use memcpy for deepcopy
        memcpy(&this->corner, &corner, sizeof(Vector3D));
        memcpy(&this->S1, &S1, sizeof(Vector3D));
        memcpy(&this->S2, &S2, sizeof(Vector3D));
        
        this->normal = this->S1.crossProduct(S2).normalize();
        this->subdivided = 0;
        this->width = S1.magnitude();
        this->height = S2.magnitude();
        this->area = getArea();
        
    }
    
    
    //Methods
    Vector3D getMidpoint();
    float getArea();
    Vector3D getNormal();

    //Variables
    Vector3D corner, S1, S2;
    Vector3D normal;
    
    float width; //length of S1
    float height; //length of S2
    float area;
    
    //State used for heuristics method when necessary
    int subdivided = 0;
    
} Plane3D;


#endif /* Plane3D_hpp */
