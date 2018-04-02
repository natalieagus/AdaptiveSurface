//
//  Plane3DGroup.hpp
//  offlineReverb
//
//  Created by Natalie Agus on 23/3/18.
//  Copyright © 2018 Hans. All rights reserved.
//

#ifndef Plane3DGroup_hpp
#define Plane3DGroup_hpp

#include "Plane3D.hpp"
#include "Vector3D.hpp"
#include <math.h>
#include <stdio.h>
#include <algorithm>    // std::sort
#include <iostream>
#include <cstdlib>
using namespace std;

class Plane3DGroup{
    
public:
    
    //constructor without argument
    Plane3DGroup(){
        this->rayNumber = 0;
        this->numberOfPlanes = 0;
        this->midPoint = Vector3D();
        this->planeGroup = new Plane3D();
        this->area = 0.f;
        this->divisor = 0.f;
    }
    
    //constructor
    Plane3DGroup(Plane3D* planes, int numberOfPlanes, Vector3D midPoint, int rayNumber = -1){
        this->rayNumber = rayNumber;
        this->numberOfPlanes = numberOfPlanes;
        this->midPoint = Vector3D(midPoint.x, midPoint.y, midPoint.z);
        
        this->totalCapacity = numberOfPlanes*2;
        this->planeGroup = new Plane3D[totalCapacity];//get more space
        memcpy(planeGroup, planes, numberOfPlanes * sizeof(Plane3D));
        
        this->area = 0.f;
        for(int i = 0; i<numberOfPlanes; i++){
            area += planeGroup[i].getArea();
        }
        
        this->divisor = 1;
        
        this->area = this->area / (float) this->divisor;
    }
    
    ~Plane3DGroup(){
        //destructor
    }
    
    //methods
    void updateDivisor(int newDivisor);
    void addPlaneToGroup(Plane3D newPlane);
    
    //variables
    Plane3D *planeGroup; //list of surfaces in the same group
    int numberOfPlanes; //number of members in the group
    int rayNumber; //the ray associated with this group
    int divisor;
    float area; //area of this group
    Vector3D midPoint;
    
    int totalCapacity;
    
    
};
#endif /* Plane3DGroup_hpp */
