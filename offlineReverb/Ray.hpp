//
//  Ray.hpp
//  offlineReverb
//
//  Created by Natalie Agus on 21/3/18.
//  Copyright © 2018 Hans. All rights reserved.
//

#ifndef Ray_hpp
#define Ray_hpp

#include <stdio.h>
#include "Vector3D.hpp"



class Ray{
    
public:
    
    //constructor
    Ray(){
        this-> p = Vector3D();
        this-> d = Vector3D(0,1,0);
        this-> u = 0;
    }
    
    Ray(Vector3D point, Vector3D direction)
    {
        this-> p = point;
        this-> d = direction;
        this-> u = 0;
    };
    
    Ray(Vector3D point, Vector3D direction, float wall_length)
    {
        this-> p = point;
        this-> d = direction;
        this-> u = 0;
        this-> wall_length = wall_length;
    };
    
    //destructor
    ~Ray(){
        
    }

    //printing methods
    void print_self_ray(){
        printf("p:{%f %f %f}, d:{%f %f %f}, u:{%f} \n ", this->p.x, this->p.y, this->p.z, this->d.x, this->d.y, this->d.z, this->u);
    }
    
    void print_vector(Vector3D in){
        printf("{%f, %f, %f}, ", in.x, in.y, in.z);
    }
    
    void print_self(){
        print_vector(get_vector());
    }
    
    //ray in Vector3D form, with current u
    Vector3D get_vector(){
        return this->p.add(this->d.scalarMult(this->u));
    }
    
    //ray in Vector3D form, with input u
    Vector3D get_vector(float u){
        return this->p.add(this->d.scalarMult(u));
    }
    
    //methods
    bool get_intersection(Ray x);
    void updateDirectionVector(float theta_azimuth);
    
    Vector3D p;
    Vector3D d;
    float u;
    float wall_length = -1;
    
    
};

#endif /* Ray_hpp */
