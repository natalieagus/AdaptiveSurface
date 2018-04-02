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
        out[i] = out[i].add(listener);
    }
    
//    for (int i = 0; i < n; i++){
//        printf("{%f, %f, %f},", out[i].x, out[i].y, out[i].z);
//    }
    
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
//        printf(" i %i \n", i);
        
        for (int j = 0; j < numOfPoints; j++){
            float distance = surfaces[i].getMidpoint().distance(points[j]);
//            std::cout << distance << std::endl;
            if (distance < nearest){
                nearest = distance;
                surface_ray_index = j;
            }
        }
        
        if (!(surface_ray_index > -1)) printf("ERROR This wall doesnt have ray!\n");
        
        assert(nearest != INFINITY && surface_ray_index > -1);
        
        surfaceRayIndex[i] = surface_ray_index;
//        printf("Surface %i is assigned to Ray %i \n", i, surface_ray_index);

    }
}

/*Helper function for assign_and_group_SurfacesBasedOnNearestNeighbour method. Create groups of Plane3D
 *that belong to each ray
 *
 *@param surfaces           a set of Plane3D surfaces
 *@param numOfSurfaces      number of elements in surfaces
 *@param points             a set of Vecto3D intersection points of bauerRay in the room
 *@param numOfPoints        number of elements in points
 *@param surfaceRayIndex    the index of ray in points of which surface[i] is closest to, found using Nearest Neighbor algorithm in groupSurfacesBasedOnNearestNeighbour. Total number of elements in surfaceRayIndex == numOfSurfaces
 *@param wallIndex          walls 1-6 (ceiling, floor, side 1-4)
 *@param numberOfDistinctRays the number of distinct rays that are going to form groups. If each rays are accounted for, this number shall be equal to numOfPoints
 *
 *@modifies                 surfaceGroups
 */
void CuboidGroup::groupSurfacesBasedOnNearestNeighbour(Plane3D *surfaces, int numOfSurfaces, Vector3D* points, int numOfPoints, int *surfaceRayIndex, int wallIndex, int numberOfDistinctRays){
    
    //init class variable for this wall's surface group
    this->surfaceGroups[wallIndex] = new Plane3DGroup[numberOfDistinctRays];
    
    //want to sort based on RayIndex to create groups, because each surface groups represent one ray
    //format: rayIndex_surfaceIndex = {ray_index, surface_index}
    std::set<std::pair<int, int>> rayIndex_surfaceIndex;
    
    for (int i = 0; i < numOfSurfaces; i++){
        rayIndex_surfaceIndex.insert(make_pair(surfaceRayIndex[i], i));
    }
    
    //create temp array
    Plane3D *surfaces_temp = new Plane3D[numOfSurfaces];
    int surfaces_temp_index = 0;
    int prev_iter = 0;
    int numberOfSurfaceGroupsOnThisWall = 0;
    
    int not_write_state = 0;
    
    //Iterate through rayIndex_surfaceIndex, sorted by rayIndex
    for (std::set<std::pair<int, int>>::iterator it=rayIndex_surfaceIndex.begin(); it!= rayIndex_surfaceIndex.end(); ++it)
    {
        //first: rayIndex, second: surfaceIndex
//        std::cout << "ray index: " << it->first << " " << "surface index: " << it->second <<std::endl;
        
        if (it->first == prev_iter){
//            std::cout << " case 1" << std::endl;
            //there's no change in ray
            //add planes to surfaces_temp
            surfaces_temp[surfaces_temp_index] = surfaces[it->second];
            surfaces_temp_index++;
            not_write_state = 1;
            //update prev_iter
            prev_iter = it->first;
        }
        
        else if (it->first != prev_iter && surfaces_temp_index == 0){
//            std::cout << " case 2" << std::endl;
            //only goes here if the first ray isn't 0
            //add planes to surfaces_temp
            surfaces_temp[surfaces_temp_index] = surfaces[it->second];
            surfaces_temp_index++;
            not_write_state = 1;
            //update prev_iter
            prev_iter = it->first;
        }
        
        else if (it->first != prev_iter && surfaces_temp_index > 0){
//            std::cout << " case 3" << std::endl;
            //ray has changed
            //create Plane3D group
            this->surfaceGroups[wallIndex][numberOfSurfaceGroupsOnThisWall] = Plane3DGroup(surfaces_temp, surfaces_temp_index, points[prev_iter]);
            not_write_state = 0;
            //update number of groups of surfaceGroup in this class
            numberOfSurfaceGroupsOnThisWall++;
            //reset the surfaces_temp_index
            surfaces_temp_index = 0;
            //add new planes to surfaces_temp
            surfaces_temp[surfaces_temp_index] = surfaces[it->second];
            surfaces_temp_index++;
            //update prev_iter
            prev_iter = it->first;
            //update the write state
            not_write_state = 1;
        }
        else{
            std::cout << " This code shall never be executed " << std::endl;
        }
        
        
        
    }
    
    //store the last ray
    if (not_write_state){
        this->surfaceGroups[wallIndex][numberOfSurfaceGroupsOnThisWall] = Plane3DGroup(surfaces_temp, surfaces_temp_index, points[prev_iter]);
        numberOfSurfaceGroupsOnThisWall++;
    }
    
//    printf("number of surface groups on this wall : %i \n", numberOfSurfaceGroupsOnThisWall);
    //update the total number of surfaceGroups
    this->numOfSurfaceGroupsInEachWall[wallIndex] = numberOfSurfaceGroupsOnThisWall;
    
    //free memory
    delete [] surfaces_temp;

}

/*Given a set of *surfaces on the same plane, and a set of *points on that same plane, assigneach surface to the nearest point (nearest neighbour problem).
 *
 *@param surfaces       a set of Plane3D surfaces
 *@param numOfSurfaces  number of elements in surfaces
 *@param points         a set of Vector3D bauerRay intersection points with the room
 *@param numOfPoints    number of elements in points
 *@param wallIndex      walls 1-6 (ceiling, floor, side 1-4)
 */
int CuboidGroup::assign_and_group_SurfacesBasedOnNearestNeighbour_onWall(Plane3D *surfaces, int numOfSurfaces, Vector3D *points, int numOfPoints, int wallIndex){
    
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
    
//    printf("\n The number of rays assigned to patches is %i ,  total rays that we require on this patch is %i \n", (int) set_surfaceRayIndex.size(), numOfPoints);
    
    //STEP 3: if all rays are accounted for,
    if (set_surfaceRayIndex.size() == numOfPoints){
        // setup this->surfaceGroups

        groupSurfacesBasedOnNearestNeighbour(surfaces,
                                             numOfSurfaces,
                                             points,
                                             numOfPoints,
                                             surfaceRayIndex,
                                             wallIndex,
                                             (int) set_surfaceRayIndex.size());
        
        
        //free memory
        delete [] surfaceRayIndex;
        
        return 0;
    }

    else if (set_surfaceRayIndex.size() < numOfPoints){
//        std::cout << " \n This means 2 rays fall on the same segmentedSides! we are going to discard some rays " << std::endl;
//        printf("Number of rays with patch: %i, num of rays on this wall %i \n", (int) set_surfaceRayIndex.size(), numOfPoints);
        
        groupSurfacesBasedOnNearestNeighbour(surfaces,
                                             numOfSurfaces,
                                             points,
                                             numOfPoints,
                                             surfaceRayIndex,
                                             wallIndex,
                                             (int) set_surfaceRayIndex.size());
        
        
        //free memory
        delete [] surfaceRayIndex;
        
        return (int) (numOfPoints - set_surfaceRayIndex.size());
    
    
//
//        //Alternative idea: assign attached rays to Plane3D group using groupSurfacesBasedOnNearestNeighbour
//        /// TODO...
//
//        //Attach rays without surface to the nearest surface
//        //We first iterate through the set
//        int idx_true = 0; //this has to be from 1 to numOfPoints
//        for (std::set<int>::iterator it=set_surfaceRayIndex.begin(); it!=set_surfaceRayIndex.end(); ++it)
//        {
//            int idx = *it;
//
//            std::cout << " Element of set idx : " << idx << std::endl;
//
//            //same value
//            if (idx==idx_true) continue;
//            //increment by 1, update idx_true
//            else if ( (idx - idx_true) == 1) idx_true++;
//            //there is a jump here
//            else if ( (idx - idx_true) > 1 ){
//
//
//                //check how much is the jump
//                int jump = idx - idx_true;
//                std::cout << "There's a jump of " << jump << " and  set element idx is " << idx << " idx_true is " << idx_true << std::endl;
//                while ( jump > 1 ){
//                    //attach that ray to a patch
//                    idx_true++; //this is the ray index that doesn't have a patch
//                    jump = idx - idx_true;
//
//                    std::cout << "Left jump of " << jump << " and set element idx is " << idx << " idx_true is " << idx_true << std::endl;
//
//                    //TODO: attach ray to nearest patch
//
//                }
//
//                //at this point jump == 1, continue as per normal
//                idx_true++;
//
//            }
//        }
    
    }
    else{
        std::cout << "\n This code should never be reached. " << std::endl;
        return -1;
    }
    
    
}


/*Checks if point M is within bounded rectangle P
 *Precondition:     M has to already be on the same plane as P
 *                  Only Use this after rayPlaneIntersection returns true
 *@returns          bool true if point M is within P, false otherwise
 */
bool CuboidGroup::isWithinRectangularPlane(Plane3D P, Vector3D M){
    
    //check if M is on plane P
    Vector3D MP = P.corner.subtract(M);
    float d_prod = MP.dotProduct(P.getNormal());
    
//    printf("M %f %f %f, plane p {{%f, %f, %f}, {%f, %f, %f}, {%f, %f, %f}} \n", M.x, M.y, M.z, P.corner.x, P.corner.y, P.corner.z, P.S1.x, P.S1.y, P.S1.z, P.S2.x, P.S2.y, P.S2.z);
    
    if (fabs(d_prod) > 0.001){
        printf("FLOATING POINT ERROR!, ray-plane intersection M isn't on plane. \n");
    }
    
//    printf("d_prod %f \n", d_prod);
    
    //assert that M is on P, if not it will be floating point error
    assert(fabs(d_prod) < 0.001);
    
    //for debugging purposes, print out if M is still not on P, that means rayPlaneIntersection does not work as intended
//    //    printf("d prod %f \n", fabs(d_prod - 0));
//    if (fabs(d_prod - 0) >= 0.00001){
//        printf("M is not on the plane P \n");
//        return false;
//    }

    
    Vector3D A = P.corner.add(P.S1);
    Vector3D B = P.corner;
    Vector3D C = P.corner.add(P.S2);
    
    Vector3D AB = B.subtract(A);
    Vector3D BC = C.subtract(B);
    
    Vector3D AM = M.subtract(A);
    Vector3D BM = M.subtract(B);
    
    float AB_dot_AM = AB.dotProduct(AM);
    float AB_dot_AB = AB.dotProduct(AB);
    float BC_dot_BM = BC.dotProduct(BM);
    float BC_dot_BC = BC.dotProduct(BC);
    
    if ( 0 <= AB_dot_AM && AB_dot_AM <= AB_dot_AB && 0 <= BC_dot_BM && BC_dot_BM <= BC_dot_BC){
//        std::cout << " \n within \n ";
        return true;
    }
    
//    std::cout << " \n is not within \n ";
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
//    //print the ray
//    Vector3D r_1 = r.get_vector(1);
//    Vector3D r_2 = r.get_vector(10);
//
//    printf("{%f, %f, %f}, ", r_1.x, r_1.y, r_1.z);
//    printf("{%f, %f, %f}, ", r_2.x, r_2.y, r_2.z);
    
    //Check if ray origins is at plane's corner
    if (fabs(r.p.subtract(p.corner).magnitude()) < 0.00001){
        *u = 0;
//        std::cout << "\nt : " << *u << " \n";
//        std::cout << " intersects at origin = plane corner";
        return true;
    }
    
    float denominator = p.getNormal().dotProduct(r.d);
    
//    std::cout << "\ndenominator : " << denominator << " \n";
    if (fabs(denominator) > 0.000001){
        *u = p.getMidpoint().subtract(r.p).dotProduct(p.getNormal()) / denominator;
//        std::cout << "\nu : " << *u << " \n";
        if (*u >= 0){
//            std::cout << " intersects ";
            return true;
        }
    }
    
//    std::cout << " does not intersect ";
    return false;
}


/*Given a list of bauer Vectors, create each a ray at listener, with direction vector corresponding to element at bauerVector
 *
 *@param    bauerVector     Vector3D from bauersMethodOnListener
 *@param    n               number of elements in bauerVector
 *@param    listener        Vector3D listener location
 *@param    out             rays containing bauerRays (modified by this method)
 */
void CuboidGroup::createBauersRayOnListener(int n, Vector3D *bauerVectors, Vector3D listener, Ray *out){
    
    for (int i = 0; i < n; i++){
        //create rays, starting at listener, at direction bauerVectors
        out[i] = Ray(listener, bauerVectors[i].subtract(listener));
    }
    
}



/*Given a set of Bauer rays, find the intersection points on that wall (if any)
 *This is a ray-plane intersection problem
 *
 *@param    wall (each rectangular room's dimension plane i.e: cube.sides[k])
 *@param    bauerRays (rays originating from listener)
 *@param    intersectionPoints (Vector3D points that lie on this wall, modified by this method)
 *returns   number of elements in intersectionPoints
 */
int CuboidGroup::findBauerPointsOnWall (Plane3D wall, Ray* bauerRays, int numberOfRays, Vector3D* intersectionPoints){
    
    //create a temp placehoder to get the Ray's u value and index value if it intersects wall
    float *uArray = new float[numberOfRays];
    int *indexArray = new int[numberOfRays];
    
    int numberOfBauerPointsOnWall = 0;
    
//    printf("\n\n\n");
    //Access rayPlaneIntersection method, and then isWithinRectangularPlane
    for (int i = 0; i<numberOfRays; i++){
        float u = 0.f;
        bool intersect = rayPlaneIntersection(wall, bauerRays[i], &u);
        
     
        if (intersect){
            //print the ray
//            printf("{{%f, %f, %f},{%f*3, %f*3, %f*3}},", bauerRays[i].p.x, bauerRays[i].p.y, bauerRays[i].p.z, bauerRays[i].d.x, bauerRays[i].d.y, bauerRays[i].d.z);
            
            
            bool isWithinPlane = isWithinRectangularPlane(wall, bauerRays[i].get_vector(u));
            if (isWithinPlane){
                
//                printf("{%f,%f,%f},", bauerRays[i].get_vector(u).x,  bauerRays[i].get_vector(u).y,  bauerRays[i].get_vector(u).z);
                
                uArray[numberOfBauerPointsOnWall] = u;
                indexArray[numberOfBauerPointsOnWall] = i;
                numberOfBauerPointsOnWall++;
            }
            
        }
    }
//    printf("\n\n\n");
    //store bauer points on wall in intersectionPoints
    for (int i = 0; i<numberOfBauerPointsOnWall; i++){
        intersectionPoints[i] = bauerRays[indexArray[i]].get_vector(uArray[i]);
        
    }
    
    //clear memory
    delete [] uArray;
    delete [] indexArray;
    
//    printf("intersecting rays on this wall : %i \n", numberOfBauerPointsOnWall);
    
//    assert(numberOfBauerPointsOnWall > 0);
    
    return numberOfBauerPointsOnWall;
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
int CuboidGroup::assign_and_group_SurfacesBasedOnNearestNeighbour_inRoom(Vector3D listener, int numOfBauerRays){
    
    //store the number of rays, this is the number of delay lines in FDN
    this->numOfBauerRays = numOfBauerRays;
    
//    //print walls
//    for (int i = 0; i<6; i++){
//        Plane3D wall = cube.sides[i];
//        printf("{{%f, %f, %f},{%f, %f, %f},{%f, %f, %f}}",wall.corner.x, wall.corner.y, wall.corner.z, wall.S1.x, wall.S1.y, wall.S1.z, wall.S2.x, wall.S2.y, wall.S2.z);
//
//    }
//    printf("\n\n");
    
    //STEP 1: create vectors emanating from listener using Bauer's method
    Vector3D *bauerVectors = new Vector3D[numOfBauerRays];
    bauersMethodOnListener(numOfBauerRays, bauerVectors, listener);
    
//    //print the vectors
//    for (int i = 0; i<numOfBauerRays; i++){
//        printf("{{%f,%f,%f},{%f,%f,%f}},", listener.x, listener.y, listener.z,bauerVectors[i].x, bauerVectors[i].y, bauerVectors[i].z);
//    }
    
    //STEP 2: convert these vectors into rays
    Ray *bauerRays = new Ray[numOfBauerRays];
    createBauersRayOnListener(numOfBauerRays, bauerVectors, listener, bauerRays);
    
    //print the rays end
//    for (int i = 0; i<numOfBauerRays; i++){
//        printf("{{%f,%f,%f},{%f,%f,%f}},", listener.x, listener.y, listener.z, bauerRays[i].get_vector(5).x, bauerRays[i].get_vector(5).y, bauerRays[i].get_vector(5).z);
//    }
    
//    //print the rays direction
//    for (int i = 0; i<numOfBauerRays; i++){
//        printf("{%f,%f,%f},", bauerRays[i].d.x, bauerRays[i].d.y, bauerRays[i].d.z);
//    }
//
    
    //STEP 3: get points on each wall where these bauerRays intersects
    //to store total number of numOfIntersectionPointsPerWall
    int total_numOfIntersectionPointPerWall = 0;
    //create pointer to get back the intersection points on this wall
    Vector3D *intersectionPoints = new Vector3D[numOfBauerRays];
    //create temp holder to find out which wall doesn't have rays
    int* noRayWall = new int[6];
    
    for (int i = 0; i<6; i++){
        Plane3D wall = cube.sides[i];
        
////        //print the wall
//        printf("{{%f, %f, %f},{%f, %f, %f},{%f, %f, %f}},", wall.corner.x, wall.corner.y, wall.corner.z, wall.S1.x, wall.S1.y, wall.S1.z, wall.S2.x, wall.S2.y, wall.S2.z);

        int numberOfIntersectionPointsOnAWall = findBauerPointsOnWall(wall, bauerRays, numOfBauerRays, intersectionPoints);
        
        noRayWall[i] = numberOfIntersectionPointsOnAWall;
        
        //only group when that wall has at least 1 ray
        if (numberOfIntersectionPointsOnAWall > 0){
            //        print intersection points on this wall
            //        for (int j = 0; j < numberOfIntersectionPointsOnAWall; j++){
            //            printf("{%f,%f,%f},", intersectionPoints[j].x,  intersectionPoints[j].y,  intersectionPoints[j].z);
            //        }
            
            //store the intersection points in global variable
            this->intersectionPointsInRoom[i] = new Vector3D[numberOfIntersectionPointsOnAWall];
            this->numOfIntersectionPointsPerWall[i] = numberOfIntersectionPointsOnAWall;
            memcpy(intersectionPointsInRoom[i], intersectionPoints, numberOfIntersectionPointsOnAWall * sizeof(Vector3D));
            total_numOfIntersectionPointPerWall += this->numOfIntersectionPointsPerWall[i];
            
            //        //ensure the storage is proper, print and check
            //        for (int j = 0; j < numberOfIntersectionPointsOnAWall; j++){
            //            printf("{%f,%f,%f},", intersectionPointsInRoom[i][j].x,  intersectionPointsInRoom[i][j].y,  intersectionPointsInRoom[i][j].z);
            //        }
            //
            
            //Solve nearest neighbour problem for this wall
            //the method below assigns patches to each ray, and create Plane3DGroups
            int rays_without_patches_on_this_wall = assign_and_group_SurfacesBasedOnNearestNeighbour_onWall(&cube.segmentedSides[i*tilesPerWall],
                                                                                                            tilesPerWall,
                                                                                                            intersectionPoints,
                                                                                                            numberOfIntersectionPointsOnAWall,
                                                                                                            i);
            
            this->rays_without_patches += rays_without_patches_on_this_wall;
            this->total_number_of_surface_groups_in_the_room += this->numOfSurfaceGroupsInEachWall[i];
            
            //        printf("In this wall index %i, the number of existing rays is %i, but %i rays don't have patches \n", i, this->numOfIntersectionPointsPerWall[i], rays_without_patches_on_this_wall);
        }
    }
    
    //make sure the total number of intersection points we get in the room is equivalent to the number of rays, because each ray has to intersect exactly one wall
    assert(total_numOfIntersectionPointPerWall == numOfBauerRays);
    
    //TODO: handle the case of wall with no ray here
    
    for (int i = 0; i<6; i++){
        if (noRayWall[i] < 1){
            printf("\nWall %i has no ray\n",i);
            assignRaylessWall(i);
            //find the nearest ray (IntersectionPoints) to group to
        }
    }
    
    //free memory
    delete [] intersectionPoints;
    delete [] noRayWall;
    
    
    //Final checking before returning
    
    //ensure the storage for ray-wall intersection is proper after memory deletion of temp arrays, hence print and check
    //iterate through walls, printing intersection points
//    for (int i = 0; i < 6; i++){
//        //get the number of intersection points per wall
//        for (int j = 0; j < numOfIntersectionPointsPerWall[i]; j ++){
//        printf("{%f,%f,%f},", intersectionPointsInRoom[i][j].x,  intersectionPointsInRoom[i][j].y,  intersectionPointsInRoom[i][j].z);
//        }
//    }

    
    printf("Number of surface groups in the room : %i \n", this->total_number_of_surface_groups_in_the_room);
//
    //now this class's variable **surfaceGroups can be accessed
    //surfaceGroups : 6 of i arrays, where i is numOfSurfaceGroupsInEachWall[1-6];
    //**surfaceGroups = Plane3D group, correspond to a group of patches that has 1 ray
    //checking and print the surfaces
    
//   Printing all walls
//    for (int wallIndex = 0; wallIndex < 6; wallIndex ++){
//        int totalNumberOfSurfaceGroups = this->numOfSurfaceGroupsInEachWall[wallIndex];
////        std::cout<< "number of surface groups in this wall index  " << wallIndex << " is  " << totalNumberOfSurfaceGroups << std::endl;
//        for (int i = 0; i<totalNumberOfSurfaceGroups; i++){
//            Plane3DGroup surfaces_group = this->surfaceGroups[wallIndex][i];
//            for (int j = 0; j<surfaces_group.numberOfPlanes; j++){
//                printf("{ {%f, %f, %f}, {%f, %f, %f}, {%f, %f, %f} },", surfaces_group.planeGroup[j].corner.x, surfaces_group.planeGroup[j].corner.y, surfaces_group.planeGroup[j].corner.z, surfaces_group.planeGroup[j].S1.x, surfaces_group.planeGroup[j].S1.y, surfaces_group.planeGroup[j].S1.z, surfaces_group.planeGroup[j].S2.x, surfaces_group.planeGroup[j].S2.y, surfaces_group.planeGroup[j].S2.z);
//            }
//
////            std::cout << " " << std::endl;
////            std::cout << " " << std::endl;
//        }
//
//    }

//    this->rays_without_patches = rays_without_patches;
    
    printf("Total rays assigned initially %i, but %i rays dont have patches \n", this->numOfBauerRays, this->rays_without_patches);
    
    //final assertion that the number of surface groups in the room is equal to the initial set number of rays, minus the rays without patches
    assert(this->total_number_of_surface_groups_in_the_room == (this->numOfBauerRays - this->rays_without_patches));
    
    
    //same as intersectionPointsInRoom but stored in a single array instead
    this->intersectionPointsInRoom_singleArray = new Vector3D[total_number_of_surface_groups_in_the_room];
    int running_index = 0;
    for (int wallIndex = 0; wallIndex < 6; wallIndex ++){
        int totalNumberOfSurfaceGroups = this->numOfSurfaceGroupsInEachWall[wallIndex];
        //        std::cout<< "number of surface groups in this wall index  " << wallIndex << " is  " << totalNumberOfSurfaceGroups << std::endl;
        for (int i = 0; i<totalNumberOfSurfaceGroups; i++){
            Plane3DGroup surfaces_group = this->surfaceGroups[wallIndex][i];
            this->intersectionPointsInRoom_singleArray[running_index] = surfaces_group.midPoint;
            running_index++;
        }
        
    }
    
    
     printf("\n\n\n END OF CUBOIDGROUP CALCULATION \n\n\n");
    
    return this->rays_without_patches;
}


/*Get delay times in samples for each surface groups
 *
 *@param    delayValues: the delay array in samples
 *@param    LLE & LRE : listener's right and left ear location
 *@param    S : source location
 *@param    Hz : sampling rate
 */
void CuboidGroup::getDelayValues(int *delayValues, Vector3D LLE, Vector3D LRE, Vector3D S, int Hz){
    
    int delayLineIndex = 0;
    for (int i = 0; i<6; i++){
        for (int j = 0; j< numOfSurfaceGroupsInEachWall[i]; j++){
            Vector3D p = this->surfaceGroups[i][j].midPoint;
            
            float d1 = S.subtract(p).magnitude();
            float d2 = LLE.subtract(p).magnitude();
            float d2R = LRE.subtract(p).magnitude();
            
            //set delay times to the nearest ear
            if (d2R < d2){
                d2 = d2R;
            }
            
            delayValues[delayLineIndex] = static_cast<int>((d1+d2)/SOUNDSPEED*Hz);
            delayLineIndex ++;
        }
    }
    
    //ensure that the number of total delay lines is the same as the number of rays
    assert(delayLineIndex == (this->numOfBauerRays - this->rays_without_patches));
    
}


/*Group the rayless wall to other nearest rays instead
 *
 *@param wallIndex = wall without Ray
 */
void CuboidGroup::assignRaylessWall(int wallIndex){
    
    //get the nearest ray to this wall "patch"
    Plane3D* tilesOnThisWall = &cube.segmentedSides[wallIndex*tilesPerWall];
    
    int *wallIndex_closest = new int[tilesPerWall];
    int *pointIndex_inWall_closest = new int[tilesPerWall];
    
    for (int k = 0; k<tilesPerWall; k++){
        Vector3D tile_midpoint = tilesOnThisWall[k].getMidpoint();
        int closest_wall_index = -1;
        int closest_point_index_in_the_wall = -1;
        float nearest = INFINITY;
        
        for (int i = 0; i<6; i++){
            int intersectionPointsOnWall = this->numOfIntersectionPointsPerWall[i];
            
            if (intersectionPointsOnWall > 0){
                
                for (int j = 0; j<intersectionPointsOnWall; j++){
                    Vector3D intersectionPointVector = this->intersectionPointsInRoom[i][j];
                    float distance = intersectionPointVector.distance(tile_midpoint);
                    if (distance < nearest){
                        nearest = distance;
                        closest_wall_index = i;
                        closest_point_index_in_the_wall = j;
                    }
                }
                
            }
            
        }
        
        wallIndex_closest[k] = closest_wall_index;
        pointIndex_inWall_closest[k] = closest_point_index_in_the_wall;
        
        //add plane to relevant group
        Plane3D tileOnThisWall = tilesOnThisWall[k];
//        printf("tile %i on wall %i is closest to another wall %i of ray %i", k, wallIndex, closest_wall_index, closest_point_index_in_the_wall);
        //get the correct surface group and add
        this->surfaceGroups[closest_wall_index][closest_point_index_in_the_wall].addPlaneToGroup(tileOnThisWall);
        
    }
    
    
    
    
}

