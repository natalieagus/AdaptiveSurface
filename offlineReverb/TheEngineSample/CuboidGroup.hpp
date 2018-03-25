//
//  CuboidGroup.hpp
//  offlineReverb
//
//  Created by Natalie Agus on 23/3/18.
//  Copyright © 2018 Hans. All rights reserved.
//


#include "Plane3D.hpp"
#include "Vector3D.hpp"
#include "Plane3DGroup.hpp"
#include "Cuboid.hpp"
#include <math.h>
#include <stdio.h>
#include <algorithm>    // std::sort
#include <iostream>
#include <cstdlib>
using namespace std;

#ifndef CuboidGroup_hpp
#define CuboidGroup_hpp

class CuboidGroup{
  
public:
    
    //Constructor
    CuboidGroup(float xLength, float yLength, float zLength, int tilesPerWall){
        this->cube = Cuboid(xLength, yLength, zLength);
        cube.segmentCube(tilesPerWall);
    }
    
    //Destructor
    ~CuboidGroup(){
        
    }
    
    
    //Adaptive lateral decomposition methods for Paper 5B
    void groupSurfacesBasedOnNearestPoint(Plane3D *surfaces, int numOfSurfaces, Vector3D* points, int numOfPoints);
    int findBauerPointsOnWall (Plane3D wall, Ray* bauerRays, Vector3D* intersectionPoints);
    void bauersMethod(int n, Vector3D* out);
    void bauersMethodOnListener(int n, Vector3D* out, Vector3D listener);
    
    //variables
    Cuboid cube;
    Plane3DGroup *surfaceGroups; //groups of surfaces that make a room
    int numberOfSurfaceGroups;
    
    
};

#endif /* CuboidGroup_hpp */
