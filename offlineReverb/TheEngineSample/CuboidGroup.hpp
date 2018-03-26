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
#include <set>
#include <map>
using namespace std;

#ifndef CuboidGroup_hpp
#define CuboidGroup_hpp

class CuboidGroup{
  
public:
    
    //Constructor
    CuboidGroup(float xLength, float yLength, float zLength, int tilesPerWall){
        this->cube = Cuboid(xLength, yLength, zLength);
        //Even segmentation
        cube.segmentCube(tilesPerWall);
        //store number of default surface patches
        this->numberOfDefaultSurfacePatches = tilesPerWall * 6;
    }
    
    //Destructor
    ~CuboidGroup(){
        
    }
    
    //Adaptive lateral decomposition methods for Paper 5B
    void assignSurfacesBasedOnNearestNeighbour(Plane3D *surfaces, int numOfSurfaces, Vector3D* points, int numOfPoints, int *surfaceRayIndex);
    void groupSurfacesBasedOnNearestNeighbour(Plane3D *surfaces, int numOfSurfaces, Vector3D* points, int numOfPoints, int *surfaceRayIndex);
    void assign_and_group_SurfacesBasedOnNearestNeighbour_onWall(Plane3D *surfaces, int numOfSurfaces, Vector3D* points, int numOfPoints);
    void assign_and_group_SurfacesBasedOnNearestNeighbour_inRoom(Vector3D listener, int bauerRays);
    
    bool isWithinRectangularPlane(Plane3D P, Vector3D M);
    bool rayPlaneIntersection(Plane3D p, Ray r, float* u);
    int findBauerPointsOnWall (Plane3D wall, Ray* bauerRays, Vector3D* intersectionPoints);
    void bauersMethod(int n, Vector3D* out);
    void bauersMethodOnListener(int n, Vector3D* out, Vector3D listener);
    void createBauersRayOnListener(int n, Vector3D bauerVector, Ray* out, Vector3D listener);
    
    //variables
    Cuboid cube;
    Plane3DGroup *surfaceGroups; //groups of surfaces that make a room
    int numberOfSurfaceGroups;
    int numberOfDefaultSurfacePatches;
    
    
};

#endif /* CuboidGroup_hpp */