//
//  CuboidGroup.cpp
//  offlineReverb
//
//  Created by Natalie Agus on 23/3/18.
//  Copyright © 2018 Hans. All rights reserved.
//

#include "CuboidGroup.hpp"


/*
 * Bauer's method: create (approx) evenly distributed points on a unit sphere with centre (0,0,0)
 * @param       n: number of points to create on unit sphere
 * @param       out : array of Vector3D as output
 */
void CuboidGroup::bauersMethod(int n, Vector3D* out){
    
    float L = sqrtf((float) n * M_PI);
    
    for (int i = 1; i < (n+1); i++){
        float z = 1.f - (2.f * (float) i - 1.f) / (float) n;
        out[i-1] = Vector3D(
                            sinf(acosf(z)) * cosf(acosf(z) * L),
                            sinf(acosf(z)) * sinf(acosf(z) * L),
                            z
                            ).normalize();
    }
    
    //    for (int i = 0; i < n; i++){
    //        //        out[i] = Vector3D(x[i], y[i], z[i]);
    //        printf("{%f, %f, %f},", out[i].x, out[i].y, out[i].z);
    //    }
    
}


/*
 * Bauer's method: create (approx) evenly distributed points on a unit sphere with centre on listener
 * @param       n: number of points to create on unit sphere
 * @param       out : array of Vector3D as output
 * @param       listener : listener's location 
 */
void CuboidGroup::bauersMethodOnListener(int n, Vector3D* out, Vector3D listener){
    
    bauersMethod(n, out);
    
    // transform to shorter ray and shift origin to listener's location
    for (int i = 0; i < n; i++){
        out[i] = out[i].scalarMult(0.1).add(listener);
    }
    
    for (int i = 0; i < n; i++){
        printf("{%f, %f, %f},", out[i].x, out[i].y, out[i].z);
    }
    
}


/*Given a set of *surfaces on the same plane, and a set of *points on that same plane, assign
 *each surface to the nearest point. Store the output at surfaceGroups.
 *
 */
void CuboidGroup::groupSurfacesBasedOnNearestPoint(Plane3D *surfaces, int numOfSurfaces, Vector3D* points, int numOfPoints){
    
    int *surfaceRayIndex = new int[numOfSurfaces];
    
    //naive brute-force method of assigning nearest neighbour
    for (int i = 0; i < numOfSurfaces; i++){
        
        float nearest = INFINITY;
        int surface_ray_index = -1;
        
        for (int j = 0; j < numOfPoints; j++){
            float distance = surfaces[i].getMidpoint().distance(points[j]);
            if (distance < nearest){
                nearest = distance;
                surface_ray_index = j;
            }
        }
        
        assert(nearest != INFINITY && surface_ray_index > -1);
        
        surfaceRayIndex[i] = surface_ray_index;

    }
    
    //check if all numbers from 0 to numOfPoints are accounted for in surfaceRayIndex
    //if not assign the rays into the tiles it is within
    //brute force method, assuming the "stray rays" are small
    
    
}


/*Given a set of Bauer rays, find the intersection points on that wall (if any)
 *@param    wall (each rectangular room's dimension plane i.e: cube.sides[k])
 *@param    bauerRays (rays originating from listener)
 *@param    intersectionPoints (Vector3D points that lie on this wall)
 *returns   number of elements in intersectionPoints
 */
int CuboidGroup::findBauerPointsOnWall (Plane3D wall, Ray* bauerRays, Vector3D* intersectionPoints){
    return 0;
}
