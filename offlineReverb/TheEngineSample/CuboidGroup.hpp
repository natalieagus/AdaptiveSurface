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
#include <string>
using namespace std;

#ifndef CuboidGroup_hpp
#define CuboidGroup_hpp

class CuboidGroup{
  
public:
    
    //Default Constructor
    CuboidGroup(){
        this->cube = Cuboid();
        //Even segmentation
        cube.segmentCube(4);
        //store the tilesPerWall
        this->tilesPerWall = 4;
        //double pointer, first pointer for walls 1-6, and second pointer is for groups of surfaces in that wall
        this->surfaceGroups = new Plane3DGroup*[6];
        //init intersection points vector
        this->intersectionPointsInRoom = new Vector3D*[6];
        //init the array to store number of elements in intersection points vector
        this->numOfIntersectionPointsPerWall = new int[6];
        memset(numOfIntersectionPointsPerWall, 0, 6*sizeof(int));
        //init surface group number on each wall array
        this->numOfSurfaceGroupsInEachWall = new int[6];
        memset(numOfSurfaceGroupsInEachWall, 0, 6*sizeof(int));
        //init rays without patches count
        this->rays_without_patches = 0;
        //init total number surface groups in the room
        this->total_number_of_surface_groups_in_the_room = 0;
    }
    
    //Constructor
    CuboidGroup(float xLength, float yLength, float zLength, int tilesPerWall){
        this->cube = Cuboid(xLength, yLength, zLength);
        //Even segmentation
        cube.segmentCube(tilesPerWall);
        //store the tilesPerWall
        this->tilesPerWall = tilesPerWall;
        //double pointer, first pointer for walls 1-6, and second pointer is for groups of surfaces in that wall
        this->surfaceGroups = new Plane3DGroup*[6];
        //init intersection points vector
        this->intersectionPointsInRoom = new Vector3D*[6];
        //init the array to store number of elements in intersection points vector
        this->numOfIntersectionPointsPerWall = new int[6];
        memset(numOfIntersectionPointsPerWall, 0, 6*sizeof(int));
        //init surface group number on each wall array
        this->numOfSurfaceGroupsInEachWall = new int[6];
        memset(numOfSurfaceGroupsInEachWall, 0, 6*sizeof(int));
        //init rays without patches count
        this->rays_without_patches = 0;
        //init total number surface groups in the room
        this->total_number_of_surface_groups_in_the_room = 0;
    }
    
    //Destructor
    ~CuboidGroup(){
        
    }
    
    //Adaptive lateral decomposition methods for Paper 5B
    void assignSurfacesBasedOnNearestNeighbour(Plane3D *surfaces, int numOfSurfaces, Vector3D* points, int numOfPoints, int *surfaceRayIndex);
    void groupSurfacesBasedOnNearestNeighbour(Plane3D *surfaces, int numOfSurfaces, Vector3D* points, int numOfPoints, int *surfaceRayIndex, int wallIndex, int numberOfDistinctRays);
    int assign_and_group_SurfacesBasedOnNearestNeighbour_onWall(Plane3D *surfaces, int numOfSurfaces, Vector3D* points, int numOfPoints, int wallIndex);
    int assign_and_group_SurfacesBasedOnNearestNeighbour_inRoom(Vector3D listener, int numOfBauerRays);
    void assignRaylessWall(int wallIndex);
    
    bool isWithinRectangularPlane(Plane3D P, Vector3D M);
    bool rayPlaneIntersection(Plane3D p, Ray r, float* u);
    int findBauerPointsOnWall (Plane3D wall, Ray* bauerRays, int numberOfRays, Vector3D* intersectionPoints);
    void bauersMethod(int n, Vector3D* out);
    void bauersMethodOnListener(int n, Vector3D* out, Vector3D listener);
    void createBauersRayOnListener(int n, Vector3D *bauerVectors,  Vector3D listener, Ray* out);
    
    //to be accessed by FDN.cpp
    void getDelayValues(int *delayValues, Vector3D LLE, Vector3D LRE, Vector3D S, int Hz);
    
    
    
    //variables
    Cuboid cube;
    Vector3D **intersectionPointsInRoom;
    Vector3D *intersectionPointsInRoom_singleArray;
    
    int *numOfIntersectionPointsPerWall;
    Plane3DGroup **surfaceGroups; //6 for outer pointer, inner pointer to be determined later in groupSurfacesBasedOnNearestNeighbour
    int *numOfSurfaceGroupsInEachWall; //6 elements
    int tilesPerWall;
    int numOfBauerRays;
    int rays_without_patches;
    int total_number_of_surface_groups_in_the_room;
    
    
};

#endif /* CuboidGroup_hpp */
