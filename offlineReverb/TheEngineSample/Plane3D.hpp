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

typedef struct Plane3D {
    
    //Constructor without arguments
    Plane3D(){
        this->corner = Vector3D();
        this->S1 = Vector3D();
        this->S2 = Vector3D();
        this->normal = this->S1.crossProduct(S2).normalize();
        bouncePoint = corner.add(S1.scalarMult(S1.magnitude()/2)).add((S2.scalarMult(S2.magnitude()/2)));
        
    };
    
    //Constructor with arguments
    Plane3D(Vector3D corner, Vector3D S1, Vector3D S2){
        this->corner = corner;
        this->S1 = S1;
        this->S2 = S2;
        this->normal = this->S1.crossProduct(S2).normalize();
        bouncePoint = corner.add(S1.scalarMult(S1.magnitude()/2)).add((S2.scalarMult(S2.magnitude()/2)));
    }
    
    
    //Methods
    void setNormal(Vector3D normal){
        this->normal = normal;
    }
    
    void setLength(){
        width = S1.magnitude();
        height= S2.magnitude();
    }
    
    Vector3D getMidpoint(){
        Vector3D out = corner.add(S1.scalarMult(0.5f));
        out = out.add(S2.scalarMult(0.5f));
        return out;
    }
    
    float getArea(){
        return S1.magnitude() * S2.magnitude();
    }
    
    void setBouncePoint(Vector3D bp){
        bouncePoint = Vector3D(bp.x, bp.y, bp.z);
    }

    //Variables
    Vector3D corner, S1, S2;
    Vector3D normal;
    Vector3D bouncePoint;
    
    float width; //length of S1
    float height; //length of S2
    
    int subdivided = 0;
    
} Plane3D;


#endif /* Plane3D_hpp */
