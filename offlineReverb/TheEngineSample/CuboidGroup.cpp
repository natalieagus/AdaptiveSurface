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



/*Helper function for assign_and_group_SurfacesBasedOnNearestNeighbour method after nearest-neighbour.
 *This method assigns surface based on nearest neighbour of points in points
 *@precondition             elements in surfaces are of the same size (even segmenetation)
 *
 *@param surfaces           a set of Plane3D surfaces
 *@param numOfSurfaces      number of elements in surfaces
 *@param points             a set of Vecto3D points
 *@param numOfPoints        number of elements in points
 *@param surfaceRayIndex    the index of ray in points of which surface[i] is closest to, found using Nearest Neighbor algorithm in groupSurfacesBasedOnNearestNeighbour. Total number of elements in surfaceRayIndex == numOfSurfaces
 */
bool compare_function (int i,int j) { return (i<j); }
void CuboidGroup::assignSurfacesBasedOnNearestNeighbour(Plane3D *surfaces, int numOfSurfaces, Vector3D* points, int numOfPoints, int *surfaceRayIndex){
    
    //set all to -1
    memset(surfaceRayIndex, -1, sizeof(int)*numOfSurfaces);
    
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
}

/*Helper function for assign_and_group_SurfacesBasedOnNearestNeighbour method. Create groups of Plane3D
 *that belong to each ray
 *
 *@param surfaces           a set of Plane3D surfaces
 *@param numOfSurfaces      number of elements in surfaces
 *@param points             a set of Vecto3D points
 *@param numOfPoints        number of elements in points
 *@param surfaceRayIndex    the index of ray in points of which surface[i] is closest to, found using Nearest Neighbor algorithm in groupSurfacesBasedOnNearestNeighbour. Total number of elements in surfaceRayIndex == numOfSurfaces
 *
 *@modifies                 surfaceGroups
 */
void CuboidGroup::groupSurfacesBasedOnNearestNeighbour(Plane3D *surfaces, int numOfSurfaces, Vector3D* points, int numOfPoints, int *surfaceRayIndex){
    
    //want to sort based on RayIndex to create groups
    std::set<std::pair<int, int>> rayIndex_surfaceIndex;
    
    for (int i = 0; i < numOfSurfaces; i++){
        rayIndex_surfaceIndex.insert(make_pair(surfaceRayIndex[i], i));
    }
    
    //Iterate through rayIndex_surfaceIndex, sorted by rayIndex
    for (std::set<std::pair<int, int>>::iterator it=rayIndex_surfaceIndex.begin(); it!= rayIndex_surfaceIndex.end(); ++it)
    {
        //first: rayIndex, second: surfaceIndex
        std::cout << it->first << " " << it->second <<std::endl;
        //TODO, create Plane3Dgroups
         // ...
        
    }
    
    
}

/*Given a set of *surfaces on the same plane, and a set of *points on that same plane, assign
 *each surface to the nearest point (nearest neighbour problem).
 *
 *@param surfaces       a set of Plane3D surfaces
 *@param numOfSurfaces  number of elements in surfaces
 *@param points         a set of Vecto3D points
 *@param numOfPoints    number of elements in points
 */
void CuboidGroup::assign_and_group_SurfacesBasedOnNearestNeighbour(Plane3D *surfaces, int numOfSurfaces, Vector3D *points, int numOfPoints){
    
    int *surfaceRayIndex = new int[numOfSurfaces];
    
    //STEP 1: naive brute-force method of assigning nearest neighbour
    assignSurfacesBasedOnNearestNeighbour(surfaces,
                                          numOfSurfaces,
                                          points,
                                          numOfPoints,
                                          surfaceRayIndex);
    
    //STEP 2: check if all numbers from 0 to numOfPoints are accounted for in surfaceRayIndex
    //if not assign the rays into the tiles it is within
    //brute force method, assuming the "stray rays" are small

    //create a set from surfaceRayIndex
    //it returns unique elements from surfaceRayIndex, stored in ascending order from begin() to end()
    std::set<int> set_surfaceRayIndex(surfaceRayIndex, surfaceRayIndex + numOfSurfaces);
    
    
    //STEP 3: if all rays are accounted for,
    if (set_surfaceRayIndex.size() == numOfPoints){
        //TODO: assign to Plane3D group using groupSurfacesBasedOnNearestNeighbour
        /// ...
    }
    else{
        //TODO: assign attached rays to Plane3D group using groupSurfacesBasedOnNearestNeighbour
        /// ...
        
        //Attach rays without surface to the nearest surface
        //We first iterate through the set
        int idx_true = 0; //this has to be from 1 to numOfPoints
        for (std::set<int>::iterator it=set_surfaceRayIndex.begin(); it!=set_surfaceRayIndex.end(); ++it)
        {
            int idx = *it;
            
            std::cout << " Element of set idx : " << idx << std::endl;
            
            //same value
            if (idx==idx_true) continue;
            //increment by 1, update idx_true
            else if ( (idx - idx_true) == 1) idx_true ++;
            //there is a jump here
            else if ( (idx - idx_true) > 1 ){
                
                
                //check how much is the jump
                int jump = idx - idx_true;
                std::cout << "There's a jump of " << jump << " and  set element idx is " << idx << " idx_true is " << idx_true << std::endl;
                while ( jump > 1 ){
                    //attach that ray to a patch
                    idx_true ++; //this is the ray index that doesn't have a patch
                    jump = idx - idx_true;
                    
                    std::cout << "Left jump of " << jump << " and set element idx is " << idx << " idx_true is " << idx_true << std::endl;
                    
                    //TODO: attach ray to nearest patch
                    
                }
                
                //at this point jump == 1, continue as per normal
                idx_true ++;
                
            }
        }
    }
    
    
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
