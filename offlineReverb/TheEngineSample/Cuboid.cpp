//
//  Cuboid.cpp
//  surfaceIntegration
//
//  Created by Natalie Agus on 31/8/16.
//  Copyright © 2016 Hans. All rights reserved.
//

#include "Cuboid.hpp"
#include <algorithm>    // std::sort
using namespace std;

//Slices random remaining planes
/*
 *int slices                    = the supposed number of surfaces
 *int remainder                 = the number of surfaces we need to make
 *Plane3D* newSegmentedSides    = the planes
 */
void Cuboid::sliceRemainder(int slices, int remainder, Plane3D* newSegmentedSides){

    int cubicTiles = slices-remainder;
    int index = slices - remainder; //last index
//    elements = slices;
    
    srand(11);
    //            srand(time(NULL));
    int randNum = rand()%(cubicTiles-1);
    
    //divide some plane into two
    for (int i = 0; i<remainder; i++){
        Plane3D refPlane = segmentedSides[randNum];
        //divide into two along S1
        if (randNum %2 == 0){
            Vector3D newCorner = refPlane.corner.add(refPlane.S1.scalarMult(0.5f));
            Vector3D newS1 = refPlane.S1.scalarMult(0.5f);
            newSegmentedSides[randNum] = Plane3D(refPlane.corner, newS1, refPlane.S2);
            newSegmentedSides[index] = Plane3D(newCorner, newS1, refPlane.S2);
            index++;
            randNum = rand()%(cubicTiles-1);
        }
        else{
            Vector3D newCorner = refPlane.corner.add(refPlane.S2.scalarMult(0.5f));
            Vector3D newS2 = refPlane.S2.scalarMult(0.5f);
            newSegmentedSides[randNum] = Plane3D(refPlane.corner,  refPlane.S1, newS2);
            newSegmentedSides[index] = Plane3D(newCorner,refPlane.S1, newS2);
            index++;
            randNum = rand()%(cubicTiles-1);
        }
    }
}


//Even subdivision, then slice the rest randomly
void Cuboid::sliceCube(int slices){
    int toSlice = 0;
    
    while(true){
        if (6*toSlice * toSlice < slices){
            toSlice += 1;
        }
        if (6*toSlice * toSlice > slices){
            toSlice -= 1;
            break;
        }
        
        }
    
    segmentCube(toSlice*toSlice);
    
    int cubicTiles = 6*toSlice*toSlice;
    Plane3D* newSegmentedSides = new Plane3D[slices];
    
    //copy into newSegmentedSides
    for (int i = 0; i<cubicTiles; i++){
        newSegmentedSides[i] = Plane3D(segmentedSides[i].corner, segmentedSides[i].S1, segmentedSides[i].S2);
    }
    
    elements = slices;
    int remainder = slices - cubicTiles;
    int index = cubicTiles;
    
    srand(11);
//            srand(time(NULL));
    
    int randNum = rand()%(cubicTiles-1);
    //divide some plane into two
    for (int i = 0; i<remainder; i++){
        Plane3D refPlane = segmentedSides[randNum];
        //divide into two along S1
        if (randNum %2 == 0){
            Vector3D newCorner = refPlane.corner.add(refPlane.S1.scalarMult(0.5f));
            Vector3D newS1 = refPlane.S1.scalarMult(0.5f);
            newSegmentedSides[randNum] = Plane3D(refPlane.corner, newS1, refPlane.S2);
            newSegmentedSides[index] = Plane3D(newCorner, newS1, refPlane.S2);
            index++;
            randNum = rand()%(cubicTiles-1);
        }
        else{
            Vector3D newCorner = refPlane.corner.add(refPlane.S2.scalarMult(0.5f));
            Vector3D newS2 = refPlane.S2.scalarMult(0.5f);
            newSegmentedSides[randNum] = Plane3D(refPlane.corner,  refPlane.S1, newS2);
            newSegmentedSides[index] = Plane3D(newCorner,refPlane.S1, newS2);
            index++;
            randNum = rand()%(cubicTiles-1);
        }
    }
    
    segmentedSides = new Plane3D[slices];
    //copy back into segmentedsides
    for (int i = 0; i<elements; i++){
        segmentedSides[i] = Plane3D(newSegmentedSides[i].corner, newSegmentedSides[i].S1, newSegmentedSides[i].S2);
    }
    
    free(newSegmentedSides);
    
////    
//    for (int i = 0; i<(slices); i++){
//        printf("{{%f, %f, %f}, {%f, %f, %f}, {%f, %f, %f}},", segmentedSides[i].corner.x, segmentedSides[i].corner.y, segmentedSides[i].corner.z, segmentedSides[i].S1.x , segmentedSides[i].S1.y, segmentedSides[i].S1.z, segmentedSides[i].S2.x, segmentedSides[i].S2.y, segmentedSides[i].S2.z );
//    }
}



//Even Segmentations
int Cuboid::segmentCube(int tilesPerSide){
    
    //Make sure tilesPerSide can be exactly square-rooted
    try
    {
        if(sqrtf(tilesPerSide) != floorf(sqrtf(tilesPerSide))){
            throw 1;
    }
    }
    catch (int e){
        printf("Please enter some number that can be square rooted for tilesPerSide \n");
        exit(1);
    }
    

    
    segmentedSides = new Plane3D[tilesPerSide*6];
    
    //now divide each side into tiles per side
    dimensions = static_cast<int>(sqrtf(tilesPerSide));

    
    int index = 0;
    for (int k = 0; k<6; k++){
        
        //get the plane
        Plane3D plane = sides[k];
        
        Vector3D s1 = plane.S1.scalarMult((float)1/dimensions);
        Vector3D s2 = plane.S2.scalarMult((float)1/dimensions);
        
        for(int i = 0; i<dimensions; i++){
            for (int j = 0; j<dimensions; j++){
                //Get the corner
                Vector3D c = plane.corner.add(plane.S1.scalarMult((float)j/dimensions));
                c = c.add(plane.S2.scalarMult((float)i/dimensions));
                
                segmentedSides[index] = Plane3D(c,s1,s2);
                segmentedSides[index].normal = plane.normal;
                index = index + 1;
            }
        }
    }
    
    return index;
//    elements = 6*tilesPerSide;
//    return index;
    
    //Printing the result
//    for (int i = 0; i< 6*tilesPerSide ; i++){
//        printf("c : {%f %f %f}, s1: {%f %f %f}, s2: {%f %f %f}, n: {%f %f %f} \n", segmentedSides[i].corner.x, segmentedSides[i].corner.y, segmentedSides[i].corner.z, segmentedSides[i].S1.x, segmentedSides[i].S1.y, segmentedSides[i].S1.z, segmentedSides[i].S2.x, segmentedSides[i].S2.y, segmentedSides[i].S2.z, segmentedSides[i].normal.x, segmentedSides[i].normal.y, segmentedSides[i].normal.z);
//    }
    
}


float Cuboid::segmentCubeOnce(){
    //segment floor and ceiling
    int index = 0;

    for (int k = 0; k<6; k++){
        
        //get the plane
        Plane3D plane = sides[k];
        
        segmentedSides[index] = plane;
        index ++;
    }

    return index;
    
    
}


void Cuboid::segmentCubeBasedOnProjectedArea(int numDelays, Vector3D S, Vector3D L){
    segmentedSides = new Plane3D[numDelays];

    elements = numDelays;
    
//    int index = segmentCubeOnce();
    int index = segmentCube(9);
    int delaysCeilingFloor = 0;
    
    while(index<numDelays){
        
        //get the side with the nearest distance to listener
        int maxIndex = 0;
        float maxArea = 100000000;
        
        for (int i = delaysCeilingFloor; i<index; i++){

            float comparisonValue = segmentedSides[i].getMidpoint().distance(L); //compare with the nearest ear's azimuth
            // only segment the walls not the floor

            if (comparisonValue < maxArea && segmentedSides[i].subdivided < 4 &&  segmentedSides[i].getMidpoint().z != 0 && segmentedSides[i].getMidpoint().z != zLength){
                printf("%f %f %f wallmidpt %f %f %f L \n", segmentedSides[i].getMidpoint().x, segmentedSides[i].getMidpoint().y, segmentedSides[i].getMidpoint().z, L.x, L.y, L.z);
                maxArea = comparisonValue;
                maxIndex = i;
            }
//            if (comparisonValue < maxArea && segmentedSides[i].subdivided < 7 && (segmentedSides[i].getMidpoint().y - L.y) > 0 && segmentedSides[i].getMidpoint().z != 0 && segmentedSides[i].getMidpoint().z < 2.9){
//                printf("%f %f %f wallmidpt %f %f %f L \n", segmentedSides[i].getMidpoint().x, segmentedSides[i].getMidpoint().y, segmentedSides[i].getMidpoint().z, L.x, L.y, L.z);
//                maxArea = comparisonValue;
//                maxIndex = i;
//            }

        }
        
        int randNum = rand()%100;
        //divide some plane into two

            //divide into two along S1
            if (randNum %2 == 0){
                Vector3D newCorner = segmentedSides[maxIndex].corner.add(segmentedSides[maxIndex].S1.scalarMult(0.5f));
                Vector3D newS1 = segmentedSides[maxIndex].S1.scalarMult(0.5f);
                int oldIndex = segmentedSides[maxIndex].subdivided;
                segmentedSides[maxIndex] = Plane3D(segmentedSides[maxIndex].corner, newS1, segmentedSides[maxIndex].S2);
                segmentedSides[maxIndex].subdivided = oldIndex + 1;
                segmentedSides[index] = Plane3D(newCorner, newS1, segmentedSides[maxIndex].S2);
                segmentedSides[index].subdivided = segmentedSides[maxIndex].subdivided;
                index++;
            }
            else{
                Vector3D newCorner = segmentedSides[maxIndex].corner.add(segmentedSides[maxIndex].S2.scalarMult(0.5f));
                Vector3D newS2 = segmentedSides[maxIndex].S2.scalarMult(0.5f);
                int oldIndex = segmentedSides[maxIndex].subdivided;
                segmentedSides[maxIndex] = Plane3D(segmentedSides[maxIndex].corner,  segmentedSides[maxIndex].S1, newS2);
                segmentedSides[maxIndex].subdivided = oldIndex + 1;
                segmentedSides[index] = Plane3D(newCorner,segmentedSides[maxIndex].S1, newS2);
                segmentedSides[index].subdivided = segmentedSides[maxIndex].subdivided;
                index++;
            }
        
        
        //split that patch
        index = dividePlane(segmentedSides[maxIndex], index, maxIndex, S, L);
        
        
    }
    
    for (int i = 0; i<(index); i++){
        printf("{{%f, %f, %f}, {%f, %f, %f}, {%f, %f, %f}},", segmentedSides[i].corner.x, segmentedSides[i].corner.y, segmentedSides[i].corner.z, segmentedSides[i].S1.x , segmentedSides[i].S1.y, segmentedSides[i].S1.z, segmentedSides[i].S2.x, segmentedSides[i].S2.y, segmentedSides[i].S2.z );
    }

}


int Cuboid::dividePlaneAlongS1(Plane3D divide, int index, int sourceIndex){
    
    Vector3D newCorner = divide.corner.add(divide.S2.scalarMult(0.5));
    Vector3D newS2 = divide.S2.scalarMult(0.5);
    segmentedSides[sourceIndex] = Plane3D(divide.corner, divide.S1, newS2);
    
    segmentedSides[index] = Plane3D(newCorner, divide.S1, newS2);
    index++;
    
    return index;
}

int Cuboid::dividePlane(Plane3D divide, int index, int sourceIndex, Vector3D S, Vector3D L ){
//    
    Vector3D omegaS1L= L.subtract(divide.corner.add(divide.S1.scalarMult(0.5))).normalize();
    Vector3D omegaS1S= S.subtract(divide.corner.add(divide.S1.scalarMult(0.5))).normalize();
    float projectedLengthS1 = divide.S1.magnitude() * omegaS1L.dotProduct(divide.normal) + divide.S1.magnitude() * omegaS1S.dotProduct(divide.normal);
    
    Vector3D omegaS2L = L.subtract(divide.corner.add(divide.S2.scalarMult(0.5))).normalize();
    Vector3D omegaS2S = S.subtract(divide.corner.add(divide.S2.scalarMult(0.5))).normalize();
    float projectedLengthS2 = divide.S2.magnitude() * omegaS2L.dotProduct(divide.normal) + divide.S2.magnitude() * omegaS2S.dotProduct(divide.normal);
 
    Vector3D c2 = divide.corner.add(divide.S1.scalarMult(0.5));
    Vector3D s1 = divide.S1.scalarMult(0.5);
    Vector3D c22 = divide.corner.add(divide.S2.scalarMult(0.5));
    Vector3D s2 = divide.S2.scalarMult(0.5);


    if (projectedLengthS1 > projectedLengthS2){
        //divide along S1
        int subdivision = segmentedSides[sourceIndex].subdivided;
        segmentedSides[sourceIndex] = Plane3D(divide.corner, s1, divide.S2);
        segmentedSides[sourceIndex].subdivided = subdivision + 1;

        segmentedSides[index] = Plane3D(c2, s1, divide.S2);
        segmentedSides[index].subdivided = segmentedSides[sourceIndex].subdivided;
        
        index++;
    }
    else{
        //Divide along S2
        int subdivision = segmentedSides[sourceIndex].subdivided;
        segmentedSides[sourceIndex] = Plane3D(divide.corner, divide.S1, s2);
        segmentedSides[sourceIndex].subdivided = subdivision + 1;

        segmentedSides[index] = Plane3D(c22, divide.S1, s2);
        segmentedSides[index].subdivided = segmentedSides[sourceIndex].subdivided;
        index++;
        

    }
    return index;
}

//Either S1 or S2
bool Cuboid::longestDimension(Plane3D patch){
    if (patch.S1.magnitude() > patch.S2.magnitude()){
        return true;
    }
    else{
        return false;
    }
}


void Cuboid::getDelayValues(int *delayValues, Vector3D LLE, Vector3D LRE, Vector3D S, int Hz){
    for (int i =0; i< elements; i++){
        Vector3D p = segmentedSides[i].getMidpoint();
        float d1 = S.subtract(p).magnitude();
        float d2 = LLE.subtract(p).magnitude();
        float d2R = LRE.subtract(p).magnitude();
        
        //set delay times to the nearest ear
        if (d2R < d2){
            d2 = d2R;
        }
        
        delayValues[i] = static_cast<int>((d1+d2)/SOUNDSPEED*Hz);
    }
}




float Cuboid::projAreaSubSec(Plane3D r, Vector3D L){
    // centre of r
    Vector3D c = r.getMidpoint();
    
    // area of r
    float A = r.getArea();
    
    // surface normal of r
    Vector3D n =   r.S1.crossProduct(r.S2).normalize();//  normalize(crossProduct(r.s1, r.s2));
    
    // L-c
    Vector3D cToL = L.subtract(c);
    
    // distance from L to centre of r
    float d = cToL.magnitude();
    
    // direction from c to L
    Vector3D Omega = cToL.normalize();
    
    // projected area = Area (n . Omega) / d^2
    return (A * fabs(n.dotProduct(Omega))) / (d*d);
}




/*!
 * Estimate the area of the rectangle r projected onto the unit sphere
 * centred at L.
 *
 * @param r         the rectangle
 * @param L         point at centre of the unit sphere of projection
 * @param divisions number of times to subdivide the rectangle in each
 *                  direction (l x w). higher values are more accurate.
 */
float Cuboid::ProjectedArea_rectangleSubDiv(Plane3D r, Vector3D L, size_t divisions){
    // how many sub patches?
    size_t numPatches = divisions*divisions;
    
    // allocate memory
    Plane3D* subMesh = new Plane3D[numPatches];
    
    // subdivide r
    Vector3D s1s = r.S1.scalarMult(1.0f/(float)divisions);//  scalarDivide(r.s1,divisions);
    Vector3D s2s = r.S2.scalarMult(1.0f/(float)divisions);;
    for(size_t i=0; i<divisions; i++){
        for(size_t j=0; j<divisions; j++){
            Vector3D cij =  r.corner.add(s1s.scalarMult(i)).add(s2s.scalarMult(j));// add3(r.c,
                             //  scalarProduct(i,s1s),
                             //  scalarProduct(j,s2s));
            Plane3D rij = Plane3D(cij, s1s, s2s);
            subMesh[divisions*i + j] = rij;
        }
    }
    
    // get projected area of each subdivision
    float projArea = 0.0f;
    for(size_t i=0; i<numPatches; i++)
        projArea += projAreaSubSec(subMesh[i],L);
    
    //
    free(subMesh);
    
    return projArea;
}


