//
//  Cuboid.hpp
//  surfaceIntegration
//
//  Created by Natalie Agus on 31/8/16.
//  Copyright © 2016 Hans. All rights reserved.
//

#ifndef Cuboid_hpp
#define Cuboid_hpp

#include <math.h>
#include <stdio.h>
#include <algorithm>    // std::sort
#include <iostream>
#include <cstdlib>
#import <Accelerate/Accelerate.h>

#include "Plane3D.hpp"
#include "Ray.hpp"

#define SOUNDSPEED 340.29f

typedef struct Cuboid{
    
    //Constructor without arguments
    Cuboid(){
        floor = Plane3D(Vector3D(0,0,0),Vector3D(1,0,0),Vector3D(0,1,0));
        ceiling = Plane3D(Vector3D(0,0,1),Vector3D(0,1,0),Vector3D(1,0,0));
        side1 = Plane3D(Vector3D(0,0,0),Vector3D(0,0,1),Vector3D(1,0,0));
        side2 = Plane3D(Vector3D(1,0,0),Vector3D(0,0,1),Vector3D(0,1,0));
        side3 = Plane3D(Vector3D(1,1,0),Vector3D(0,0,1),Vector3D(-1,0,0));
        side4 = Plane3D(Vector3D(0,1,0),Vector3D(0,0,1),Vector3D(0,-1,0));
        
        sides[0] = floor;
        sides[1] = ceiling;
        sides[2] = side1;
        sides[3] = side2;
        sides[4] = side3;
        sides[5] = side4;
        
        dimensions = 1;
        elements = 6;
        area = 6.0f;
        
        this->xLength = 1;
        this->yLength = 1;
        this->zLength = 1;
        
        this->volume = xLength * yLength * zLength;
    }
    
    Cuboid(float xLength, float yLength, float zLength){
        
        //corner, Z-axis pointing +ve vector, and the second vector. S1 to S2 right hand rule forms surface normal
        floor = Plane3D(Vector3D(0,0,0),Vector3D(xLength,0,0),Vector3D(0,yLength,0));
        ceiling = Plane3D(Vector3D(0,0,zLength),Vector3D(0,yLength,0),Vector3D(xLength,0,0));
        side1 = Plane3D(Vector3D(0,0,0),Vector3D(0,0,zLength),Vector3D(xLength,0,0));
        side2 = Plane3D(Vector3D(xLength,0,0),Vector3D(0,0,zLength),Vector3D(0,yLength,0));
        side3 = Plane3D(Vector3D(xLength,yLength,0),Vector3D(0,0,zLength),Vector3D(-xLength,0,0));
        side4 = Plane3D(Vector3D(0,yLength,0),Vector3D(0,0,zLength),Vector3D(0,-yLength,0));
        
        sides[0] = floor;
        sides[1] = ceiling;
        sides[2] = side1;
        sides[3] = side2;
        sides[4] = side3;
        sides[5] = side4;
        
        elements = 6;
        dimensions = 1;
        
        area = (zLength * yLength * 2) + (zLength * xLength * 2) + (xLength * yLength * 2);
        
        this->xLength = xLength;
        this->yLength = yLength;
        this->zLength = zLength;
        
        volume = this->xLength * this->yLength * this->zLength;
    }
    
    //even segmentation, then slice the remainder randomly
    void sliceCube(int slices);
    //even segmentation, output will be 6 (1 by 6), or 24 (4 by 6), or 54 (9 by 6), etc
    int segmentCube(int tilesPerSide);
    //Subdivide some patches randomly until we have last_index+remainder number of patches in total
    int sliceRemainder(int last_index, int remainder, Plane3D* newSegmentedSides);
    
    //Adaptive lateral decomposition method for Paper 5
    void sliceCubeLateral(int tilesPerSide, int slices, int channels, Vector3D listener);
    int splitWalls(Plane3D* walls_in, int walls_per_side, Plane3D* walls_out, int n, Plane3D* lateral_walls, Vector3D listener, float room_width, float room_length);
    Ray getRayFromWallPatch(Vector3D L, Plane3D wall_patch);
    int segmentWallsBasedOnAzimuth(Plane3D* lateral_walls, Vector3D listener, Plane3D* segmented_lateral_walls, int channels);
    
    //segment cube into 6 surfaces
    float segmentCubeOnce();
    
    //getting delay values in samples given a listener and source location in the room
    void getDelayValues(int* delayValues, Vector3D LLE, Vector3D LRE, Vector3D S, int Fs);
    
    /*
     * Heuristic method (currently not used)
     */
    //segment 6-surface rectangular room based on some heuristic
    void segmentCubeBasedOnProjectedArea(int numDelays, Vector3D S, Vector3D L);
    float projectedAreaOfAPlane(Vector3D S, Vector3D L, Plane3D patch);
    int dividePlane(Plane3D divide, int index, int sourceIndex, Vector3D L, Vector3D S);
//    int dividePlaneAlongS1(Plane3D divide, int index, int sourceIndex);
//    bool longestDimension(Plane3D patch);
    float ProjectedArea_rectangleSubDiv(Plane3D r, Vector3D L, size_t divisions);
    float projAreaSubSec(Plane3D r, Vector3D L);
    
    //Variables
    Plane3D floor, ceiling, side1, side2, side3, side4;
    Plane3D sides[6]; //original 6 sides
    Plane3D* segmentedSides; //after subdivision
    
    float xLength, yLength, zLength; //rectangular room dimension
    
    int elements; //number of patches in the room
    int dimensions; //number of patch dimension if there's even subdivision. It is set to 2 if 4 patches per wall side, 3 if 9 patches per wall side, etc
    float volume, area;
    
    
    
} Cuboid;

#endif /* Cuboid_hpp */
