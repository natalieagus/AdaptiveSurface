//
//  Plane3DGroup.cpp
//  offlineReverb
//
//  Created by Natalie Agus on 23/3/18.
//  Copyright © 2018 Hans. All rights reserved.
//

#include "Plane3DGroup.hpp"


/*Only use this when a single patch is divided into several rays
 *Valid only when there's only 1 element in this class
 *@param    newDivisor: number of rays sharing this patch
 */
void Plane3DGroup::updateDivisor(int newDivisor){
    this->area = (this->area * (float) this->divisor) / (float) newDivisor;
    this->divisor = newDivisor;
}


/*Add new plane to the group
 *@param    newPlane
 */
void Plane3DGroup::addPlaneToGroup(Plane3D newPlane){
    
    //if have space, insert
    if (totalCapacity >= (numberOfPlanes + 1)){
        //copy to memory
        memcpy(planeGroup+numberOfPlanes, &newPlane, sizeof(Plane3D));
        //print out the plane
//        printf("\n original plane: {{%f, %f, %f}, {%f, %f, %f}, {%f, %f, %f}},\n", newPlane.corner.x, newPlane.corner.y, newPlane.corner.z, newPlane.S1.x, newPlane.S1.y, newPlane.S1.z,newPlane.S2.x, newPlane.S2.y, newPlane.S2.z);
//        printf("\n copied plane: {{%f, %f, %f}, {%f, %f, %f}, {%f, %f, %f}},\n",planeGroup[numberOfPlanes].corner.x, planeGroup[numberOfPlanes].corner.y, planeGroup[numberOfPlanes].corner.z, planeGroup[numberOfPlanes].S1.x, planeGroup[numberOfPlanes].S1.y, planeGroup[numberOfPlanes].S1.z,planeGroup[numberOfPlanes].S2.x, planeGroup[numberOfPlanes].S2.y, planeGroup[numberOfPlanes].S2.z);
        numberOfPlanes ++;
        
        area += newPlane.getArea();
        
    }
    
    else{
        //creating new space
//        printf("\n CREATING NEW SPACE IN PLANE3DGROUP\n");
        Plane3D* newGroup = new Plane3D[numberOfPlanes + 10];
        this->totalCapacity = numberOfPlanes + 10;

        memcpy(newGroup, this->planeGroup, numberOfPlanes * sizeof(Plane3D));
        
        memcpy(newGroup+numberOfPlanes, &newPlane, sizeof(Plane3D));
        //print out the plane
//        printf("\noriginal plane: {{%f, %f, %f}, {%f, %f, %f}, {%f, %f, %f}},\n", newPlane.corner.x, newPlane.corner.y, newPlane.corner.z, newPlane.S1.x, newPlane.S1.y, newPlane.S1.z,newPlane.S2.x, newPlane.S2.y, newPlane.S2.z);
//        printf("\ncopied plane: {{%f, %f, %f}, {%f, %f, %f}, {%f, %f, %f}},\n",newGroup[numberOfPlanes].corner.x, newGroup[numberOfPlanes].corner.y, newGroup[numberOfPlanes].corner.z, newGroup[numberOfPlanes].S1.x, newGroup[numberOfPlanes].S1.y, newGroup[numberOfPlanes].S1.z,newGroup[numberOfPlanes].S2.x, newGroup[numberOfPlanes].S2.y, newGroup[numberOfPlanes].S2.z);
        numberOfPlanes ++;
        
        area += newPlane.getArea();
        this->planeGroup = newGroup;
    }
    
}
