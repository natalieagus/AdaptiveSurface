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
#include "Plane3D.hpp"
#include <cstdlib>

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
    
    //even slice, then slice the remainder randomly
    void sliceCube(int slices);
    //even slice
    int segmentCube(int tilesPerSide);
    //slice remainders
    void sliceRemainder(int slices, int remainder, Plane3D* newSegmentedSides);
    
    float segmentCubeOnce();
    
    void getDelayValues(int* delayValues, Vector3D LLE, Vector3D LRE, Vector3D S, int Hz);
    void segmentCubeBasedOnProjectedArea(int numDelays, Vector3D S, Vector3D L);

    float projectedAreaOfAPlane(Vector3D S, Vector3D L, Plane3D patch);
    
    int dividePlane(Plane3D divide, int index, int sourceIndex, Vector3D L, Vector3D S);
    int dividePlaneAlongS1(Plane3D divide, int index, int sourceIndex);
    //Either S1 or S2
    bool longestDimension(Plane3D patch);
    
    float ProjectedArea_rectangleSubDiv(Plane3D r, Vector3D L, size_t divisions);
    float projAreaSubSec(Plane3D r, Vector3D L);
    
    //Variables
    Plane3D floor, ceiling, side1, side2, side3, side4;
    Plane3D sides[6];
    Plane3D* segmentedSides;
    
    float xLength, yLength, zLength;
    
    int elements;
    float area;
    int dimensions;
    float volume;
    
    
    
} Cuboid;

#endif /* Cuboid_hpp */
