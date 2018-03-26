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
    
    //create temp array
    Plane3D *surfaces_temp = new Plane3D[numOfSurfaces];
    int surfaces_temp_index = 0;
    int prev_iter = 0;
    this->numberOfSurfaceGroups = 0;
    
    //Iterate through rayIndex_surfaceIndex, sorted by rayIndex
    for (std::set<std::pair<int, int>>::iterator it=rayIndex_surfaceIndex.begin(); it!= rayIndex_surfaceIndex.end(); ++it)
    {
        //first: rayIndex, second: surfaceIndex
        std::cout << it->first << " " << it->second <<std::endl;
        if (it->first == prev_iter){
            //there's no change in ray
            //add planes to surfaces_temp
            surfaces_temp[surfaces_temp_index] = surfaces[it->second];
            surfaces_temp_index ++;
            //update prev_iter
            prev_iter = it->first;
        }
        
        else if (it->first != prev_iter && surfaces_temp_index == 0){
            //only goes here if the first ray isn't 0
            //add planes to surfaces_temp
            surfaces_temp[surfaces_temp_index] = surfaces[it->second];
            surfaces_temp_index ++;
            //update prev_iter
            prev_iter = it->first;
        }
        
        else if (it->first != prev_iter && surfaces_temp_index > 0){
            //ray has changed
            //create Plane3D group
            this->surfaceGroups[numberOfSurfaceGroups] = Plane3DGroup(surfaces_temp, surfaces_temp_index, points[prev_iter]);
            //update number of groups of surfaceGroup in this class
            this->numberOfSurfaceGroups ++;
            //reset the surfaces_temp_index
            surfaces_temp_index = 0;
            //add new planes to surfaces_temp
            surfaces_temp[surfaces_temp_index] = surfaces[it->second];
            surfaces_temp_index ++;
            //update prev_iter
            prev_iter = it->first;
        }
        else{
            std::cout << " This code shall never be executed " << std::endl;
        }
        
        
        
    }
    
    
}

/*Given a set of *surfaces on the same plane, and a set of *points on that same plane, assigneach surface to the nearest point (nearest neighbour problem).
 *
 *@param surfaces       a set of Plane3D surfaces
 *@param numOfSurfaces  number of elements in surfaces
 *@param points         a set of Vecto3D points
 *@param numOfPoints    number of elements in points
 */
void CuboidGroup::assign_and_group_SurfacesBasedOnNearestNeighbour_onWall(Plane3D *surfaces, int numOfSurfaces, Vector3D *points, int numOfPoints){
    
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
        // setup this->surfaceGroups
        this->surfaceGroups = new Plane3DGroup[set_surfaceRayIndex.size()];
        groupSurfacesBasedOnNearestNeighbour(surfaces,
                                             numOfSurfaces,
                                             points,
                                             numOfPoints,
                                             surfaceRayIndex);
        return;
    }
    else{
        //handling the case where not all rays are accounted for (rarer case, esp when number of rays << number of patches)
        
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


/*Checks if point M is within bounded rectangle P
 *Condition:    M has to already be on the same plane as P
 *              Only Use this after rayPlaneIntersection returns true
 *@returns      bool true if point M is within P, false otherwise
 */
bool CuboidGroup::isWithinRectangularPlane(Plane3D P, Vector3D M){
    
    //check if M is on plane P
    Vector3D MP = P.corner.subtract(M);
    float d_prod = MP.dotProduct(P.getNormal());
    
    //    printf("d prod %f \n", fabs(d_prod - 0));
    
    if (fabs(d_prod - 0) >= 0.00001){
        printf("M is not on the plane P \n");
        return false;
    }
    
    
    Vector3D P1 = P.corner.add(P.S1);
    Vector3D P2 = P1.add(P.S2);
    Vector3D P3 = P.corner.add(P.S2);
    Vector3D P4 = P.corner;
    
    Vector3D V3 = P4.subtract(P3);
    Vector3D V1 = P2.subtract(P1);
    Vector3D V4 = M.subtract(P1);
    Vector3D V5 = M.subtract(P3);
    
    float V1_dot_V4 = V1.dotProduct(V4);
    float V3_dot_V5 = V3.dotProduct(V5);
    
    if ( V1_dot_V4 >= 0 && V3_dot_V5 >= 0){
        std::cout << " \n within \n ";
        return true;
    }
    
    std::cout << " \n is not within \n ";
    return false;
}


/*Checks if a ray r intersects an unbounded plane p
 *
 *@param p      (unbounded) Plane3D p
 *@param r      Ray at interest
 *
 *@modifies u   the scalar part of Ray r if r and p intersects
 *@returns      bool true if intersects, false otherwise
 */
bool CuboidGroup::rayPlaneIntersection(Plane3D p, Ray r, float* u){
    
    //Check if ray origins is at plane's corner
    if (fabs(r.p.subtract(p.corner).magnitude()) < 0.00001){
        *u = 0;
        std::cout << "\nt : " << *u << " \n";
        std::cout << " intersects at origin = plane corner";
        return true;
    }
    
    float denominator = p.getNormal().dotProduct(r.d);
    
    std::cout << "\ndenominator : " << denominator << " \n";
    if (fabs(denominator) > 0.000001){
        *u = p.corner.subtract(r.p).dotProduct(p.getNormal()) / denominator;
        std::cout << "\nu : " << *u << " \n";
        if (*u >= 0){
            std::cout << " intersects ";
            return true;
        }
    }
    
    std::cout << " does not intersect ";
    return false;
}


/*Given a list of bauer Vectors, create each a ray at listener, with direction vector corresponding to element at bauerVector
 *
 *@param    bauerVector     Vector3D from bauersMethodOnListener
 *@param    n               number of elements in bauerVector
 *@param    listener        Vector3D listener location
 *@param    out             rays containing bauerRays (modified by this method)
 */
void CuboidGroup::createBauersRayOnListener(int n, Vector3D bauerVector, Ray* out, Vector3D listener){
    
}



/*Given a set of Bauer rays, find the intersection points on that wall (if any)
 *This is a ray-plane intersection problem
 *
 *@param    wall (each rectangular room's dimension plane i.e: cube.sides[k])
 *@param    bauerRays (rays originating from listener)
 *@param    intersectionPoints (Vector3D points that lie on this wall, modified by this method)
 *returns   number of elements in intersectionPoints
 */
int CuboidGroup::findBauerPointsOnWall (Plane3D wall, Ray* bauerRays, Vector3D* intersectionPoints){
    //TODO access rayPlaneIntersection method, and then isWithinRectangularPlane
    return 0;
}




/*Given an evenly subdivided rube (Cuboid object), do:
    1. Create a total of 'bauerRays' Vector3D objects emanating from listener
    2. Create a total of 'bauerRays' Rays emanating from listener
    3. Iterate through each of the 6 original wall sides: cube.sides[0-5] and findBauerPointsOnWall
    4. For each wall, assign_and_groupSurfacesBasedOnNearestNeighbour_onWall, such that this->surfaceGroups are properly filled (1 group for each Ray)
 *
 *@param bauerRays      number of Rays emanated from listener
 *@param listener       listener's location (Vector3D object)
 */
void CuboidGroup::assign_and_group_SurfacesBasedOnNearestNeighbour_inRoom(Vector3D listener, int bauerRays){
    //TODO ...
    
}

