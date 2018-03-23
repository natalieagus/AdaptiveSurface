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
    
    Ray(){
        this-> p = Vector3D();
        this-> d = Vector3D();
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
    
    
    void updateDirectionVector(float theta_azimuth)
    {
        if (theta_azimuth > 360.f){
            theta_azimuth = remainder(theta_azimuth, 360.f);
        }
        
        theta_azimuth = theta_azimuth / 180.f * M_PI;
        
        this->d = Vector3D(sin(theta_azimuth), cos(theta_azimuth), d.z);
        this->d = d;
        this->u = 0.f;
    };
    
    void print_self_ray(){
        printf("p:{%f %f %f}, d:{%f %f %f}, u:{%f} \n ", this->p.x, this->p.y, this->p.z, this->d.x, this->d.y, this->d.z, this->u);
    }
    
    void print_vector(Vector3D in){
        printf("{%f, %f, %f}, ", in.x, in.y, in.z);
    }
    
    void print_self(){
        print_vector(get_vector());
    }
    
    Vector3D get_vector(){
        return this->p.add(this->d.scalarMult(this->u));
    }
    
    Vector3D get_vector(float u){
        return this->p.add(this->d.scalarMult(u));
    }
    
    bool get_intersection(Ray x){
        float dx = x.p.x - this->p.x;
        //        std::cout << dx << " dx \n";
        float dy = x.p.y - this->p.y;
        //        std::cout << dy << " dy \n";
        
        float D = x.d.x * this->d.y - x.d.y * this->d.x;
        //        std::cout << D << " D \n";
        
        //rays are not parallel
        if (fabs(D) > 0.00001){
            float intersection_scalar_this = (dy * x.d.x - dx * x.d.y)/D;
            float intersection_scalar_x = (dy * this->d.x - dx * this->d.y)/D;
            
            //            printf("this ray's scalar %f, other ray scalar %f \n", intersection_scalar_this, intersection_scalar_x);
            
            if (intersection_scalar_this >= 0 && intersection_scalar_x >= 0 &&
                ((this->wall_length > 0 && intersection_scalar_this <= wall_length) ||
                 (this->wall_length < 0))
                )
            {
                if (abs(intersection_scalar_this) <= 0.00001 ) intersection_scalar_this = 0.f;
                
                this->u = intersection_scalar_this;
                //                return intersection_scalar;
                return true;
                
            }
            
            // Negative intersection means Ray x is behind this ray
            else return false;
            
            //this will not set anything, but this is the formula for
            //the scalar in the other ray x
            //        x.u = (dy * this->d.x - dx * this->d.y)/D;
            
        }
        
        //parallel rays, they do not intersect
        else{
            return false;
        }
    }
    
    
    Vector3D p;
    Vector3D d;
    float u;
    float wall_length = -1;
    
    
};

#endif /* Ray_hpp */
