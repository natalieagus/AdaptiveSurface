//
//  Gains.cpp
//  surfaceIntegration
//
//  Created by Natalie Agus on 31/8/16.
//  Copyright © 2016 Hans. All rights reserved.
//

#include "Gains.hpp"
#include <iostream>
#include <algorithm>

#import <Accelerate/Accelerate.h>

using namespace std;


float Gains::pointCollectionFunction(Vector3D x, Vector3D L, Vector3D N, float visibility, float absorptionRate){
    Vector3D xLnormalized = Lambda(x, L);
    Vector3D xL = L.subtract(x);
    float g = xLnormalized.dotProduct(N) / powf(xL.magnitude(), 2);
    float airabsorption = exp(-absorptionRate * xL.magnitude());
    return visibility * g * airabsorption;
    
}


float Gains::getDBRDF(){
    return (1.0f-absorptionCoefficient)/M_PI;
}


float Gains::reflectionKernel(Vector3D x, Vector3D L, Vector3D S, Vector3D N, float visibility){
    Vector3D xSnormalized = Lambda(x, S);
    Vector3D xS = S.subtract(x);
    float g = xSnormalized.dotProduct(N) / powf(xS.magnitude(), 2);
    float airabsorption = exp(-ALPHA * xS.magnitude());
    return visibility  * g * getDBRDF() * airabsorption;
    
    
}


Vector3D Gains::Lambda(Vector3D u, Vector3D x){
    Vector3D ux = x.subtract(u);
    ux = ux.normalize();
    return ux;
}



void Gains::monteCarloUpsilon(Vector3D *points, Vector3D L, Vector3D S, Vector3D N, int numPoints, float* up, float area){
    
    //Integrate h over the surface patch
    float hInt = 0.0f;
    for (int i = 0; i<numPoints; i++){
//        printf("{%f, %f, %f}, \n", points[i].x, points[i].y, points[i].z);
//        printf("PCF %f\n", pointCollectionFunction(points[i], L, N, 1.0f, 0.0f));
        hInt += (area * pointCollectionFunction(points[i], L, N, 1.0f, 0.0f))/(float)numPoints;
    }
    
//    printf("hInt ups %f \n", hInt);
    *up = sqrtf(((float) numberDelays / (M_PI * totalSurfaceArea)) * hInt);
    
}

void Gains::monteCarloBeta(Vector3D *points, Vector3D L, Vector3D S, Vector3D N, int numPoints, float* beta, float area){
    
    //Integrate h * R over the surface patch
    float hRInt = 0.0f;
    for (int i = 0; i<numPoints; i++){
        //        printf("points  %f %f %f \n", points[i].x, points[i].y, points[i].z);
        float h = pointCollectionFunction(points[i], L, N, 1.0f, ALPHA);
        float R = reflectionKernel(points[i], L, S, N, 1.0f);
        hRInt += (area * h * R)/(float)numPoints * this->energyReceived;
    }
    
    *beta = sqrtf(dmin*dmin * hRInt);
    
}

/*Computes squared of upsilon before normalization by NUM_MONTECARLO and surface area, and
 *(float) numberDelays / (M_PI * Room->cube.area) term that adjust the energy based on delay length
 *
 *@param    points      the monte-carlo points in a surface patch
 *@param    L, S, N     listener, source, and normal vector of this patch
 */
void Gains::monteCarloUpsilon_Squared_Group(Vector3D *points, Vector3D L, Vector3D S, Vector3D N, float* patchUpsilon){
    
    //Integrate h over the surface patch
    float hInt = 0.0f;
    for (int i = 0; i<NUM_MONTECARLO; i++){
        hInt += (pointCollectionFunction(points[i], L, N, 1.0f, 0.0f));
    }
    
    *patchUpsilon += hInt;
    
}

/*Computes squared of beta before normalization by NUM_MONTECARLO and surface area,
 *
 *@param    points      the monte-carlo points in a surface patch
 *@param    L, S, N     listener, source, and normal vector of this patch
 */
void Gains::monteCarloBeta_Squared_Group(Vector3D *points, Vector3D L, Vector3D S, Vector3D N, float* patchBeta){
    
    //Integrate h * R over the surface patch
    float hRInt = 0.0f;
    for (int i = 0; i<NUM_MONTECARLO; i++){
        //        printf("points  %f %f %f \n", points[i].x, points[i].y, points[i].z);
        float h = pointCollectionFunction(points[i], L, N, 1.0f, ALPHA);
        float R = reflectionKernel(points[i], L, S, N, 1.0f);
        hRInt += (h * R) * this->energyReceived;
    }
    
    *patchBeta += dmin*dmin*hRInt;
    
}



/*Fill upsilon and beta values from CuboidGroup
 *
 *@param Room   the room segmented into groups of Plane3D
 *@param L      the listener's location
 *@param S      the source location
 */

float Gains::calculateGainsGroup(CuboidGroup* Room, Vector3D L, Vector3D S){
    
    //init all the coefficients memory
    mu = new float[Room->numOfBauerRays];
    upsilon = new float[Room->numOfBauerRays];
    beta = new float[Room->numOfBauerRays];
    
    //set them all to zero
    memset(mu, 0, numberDelays*sizeof(float));
    memset(upsilon, 0, numberDelays*sizeof(float));
    memset(beta, 0, numberDelays*sizeof(float));
    
    //get temp points for montecarlo method
    Vector3D *points = new Vector3D[NUM_MONTECARLO];
    
    //iterate through each ray associated with surface groups
    printf("Number delays with gain (rays associated with patch group) is : %d \n", numberDelays);
    int number_of_rays_with_patch = Room->numOfBauerRays - Room->rays_without_patches;
    assert(numberDelays == number_of_rays_with_patch);
    int gain_index = 0;
    
    //print the total number of surface groups
    int patch_group = 0;
    for (int i = 0; i<6; i++){
        patch_group += Room->numOfSurfaceGroupsInEachWall[i];
    }
    
    printf("print the total number of surface groups %i \n", patch_group);
    
    //iterate through the walls
    for (int i = 0; i < 6; i++){
        
//        int number_of_groups_in_wall = Room->numOfSurfaceGroupsInEachWall[i];
        
        //iterate through each group in each wall
        for (int j = 0; j < Room->numOfSurfaceGroupsInEachWall[i]; j++){
            
//            Plane3DGroup groupsOfSurfaces = Room->surfaceGroups[i][j];
            int number_of_patches_in_group = Room->surfaceGroups[i][j].numberOfPlanes;
            
            float patch_beta = 0;
            float patch_hInt_upsilon = 0;
            //iterate through each patch in each group
            for (int k = 0; k<number_of_patches_in_group; k++){
//                Vector3D c = Room->surfaceGroups[i][j].planeGroup[k].corner;
//                Vector3D s1 = Room->surfaceGroups[i][j].planeGroup[k].S1;
//                Vector3D s2 = Room->surfaceGroups[i][j].planeGroup[k].S2;
//                Vector3D normal = Room->surfaceGroups[i][j].planeGroup[k].normal;
//
//                float patch_area = Room->surfaceGroups[i][j].planeGroup[k].getArea();
                
                randomPointsOnRectangle(Room->surfaceGroups[i][j].planeGroup[k].corner,
                                        Room->surfaceGroups[i][j].planeGroup[k].S1,
                                        Room->surfaceGroups[i][j].planeGroup[k].S2,
                                        points, NUM_MONTECARLO);
                
                monteCarloBeta_Squared_Group(points, L, S, Room->surfaceGroups[i][j].planeGroup[k].normal, &patch_beta);
                monteCarloUpsilon_Squared_Group(points, L, S, Room->surfaceGroups[i][j].planeGroup[k].normal, &patch_hInt_upsilon);
                
            }
            
            
            //store beta and upsilon
            beta[gain_index] = sqrtf((patch_beta * (float) Room->surfaceGroups[i][j].area)  / ((float) (NUM_MONTECARLO * Room->surfaceGroups[i][j].numberOfPlanes)));
            
            upsilon[gain_index] = sqrtf(((float) numberDelays / (M_PI * Room->cube.area)) * (patch_hInt_upsilon * (float) Room->surfaceGroups[i][j].area) / ((float) NUM_MONTECARLO * Room->surfaceGroups[i][j].numberOfPlanes));

            gain_index ++;
            
            
        }
        
    }
    
    printf("The number of elements filled in beta and upsilon is %i, and the number of rays with patch (numberDelays) is %i \n", gain_index, numberDelays);
    assert(gain_index == numberDelays);
    
    vDSP_vdiv(upsilon, 1, beta, 1, mu, 1, numberDelays);
    vDSP_vdiv(feedbackTapGains, 1, mu, 1, mu, 1, numberDelays);
    
    
    //computing correct input energy and total energy
    float sumbeta = 0.0;
    float sumup = 0.0;
    for (int i = 0; i<numberDelays; i++){
        //print beta and upsilon
//        printf("i: %i, beta %f upsilon %f mu %f feedbacktapGains %f\n", i, beta[i], upsilon[i], mu[i], feedbackTapGains[i]);
        sumbeta += beta[i];
        sumup += upsilon[i];
    }
    
    printf("Sumbeta: %f sumUp: %f \n", sumbeta, sumup);
    
    for (int i = 0; i< numberDelays; i++){
        //        if want to randomise sign of mu and upsilon
        //        mu[i] *= powf(-1, rand()%2);
        //        upsilon[i] *=powf(-1, rand()%2);
        totalInputEnergy += mu[i] * mu[i];
    }
    
    printf("Total Input Energy: %f \n", totalInputEnergy);
    printf("Input energy should be : %f \n", correctInputEnergy);
    printf("We feed too much input energy by a factor of : %f (if < 1 then we put too little, if > 1 then we put too much)\n", totalInputEnergy/correctInputEnergy);
    
    
    return correctInputEnergy - totalInputEnergy;
}


float Gains::calculateGains(Plane3D *surfaces, Vector3D L, Vector3D S){
    
    printf("SourceLoc : %f %f %f lLoc %f %f %f \n", S.x, S.y, S.z, L.x, L.y, L.z);
    printf("dmin %f \n", dmin);
    
    mu = new float[numberDelays];
    upsilon = new float[numberDelays];
    beta = new float[numberDelays];
    
    for (int i = 0; i<numberDelays; i++){
        beta[i] = 0.0f;
        upsilon[i] = 0.0f;
        mu[i] = 0.0f;
    }
    
    Vector3D points[NUM_MONTECARLO];
    
    printf("Number delays : %d \n", numberDelays);
    for (int i = 0; i < numberDelays; i++){
        
//        printf("%i \n" , i);
        Vector3D c = surfaces[i].corner;
        Vector3D s1 = surfaces[i].S1;
        Vector3D s2 = surfaces[i].S2;
        
        
//        printf("Surfaces %f %f %f, %f %f %f, %f %f %f \n", surfaces[i].corner.x, surfaces[i].corner.y, surfaces[i].corner.z, surfaces[i].S1.x, surfaces[i].S1.y, surfaces[i].S1.z, surfaces[i].S2.x,surfaces[i].S2.y, surfaces[i].S2.z  );
        
        randomPointsOnRectangle(c, s1, s2, points, NUM_MONTECARLO);
        
        //        printf("\n\n ");
        //        for (int k = 0; k<NUM_MONTECARLO; k++){
        //            printf("{%f, %f, %f},", points[k].x, points[k].y, points[k].z );
        //        }
        
        //                printf("Surfaces %f %f %f, %f %f %f, %f %f %f \n", surfaces[i].corner.x, surfaces[i].corner.y, surfaces[i].corner.z, surfaces[i].S1.x, surfaces[i].S1.y, surfaces[i].S1.z, surfaces[i].S2.x,surfaces[i].S2.y, surfaces[i].S2.z  );
        monteCarloUpsilon(points, L, S, surfaces[i].normal, NUM_MONTECARLO, &upsilon[i], surfaces[i].getArea());
        monteCarloBeta(points, L, S, surfaces[i].normal, NUM_MONTECARLO, &beta[i], surfaces[i].getArea());
//        printf("i: %d Beta %f Up : %f \n", i, beta[i], upsilon[i]);
        
    }
    
    
    
    vDSP_vdiv(upsilon, 1, beta, 1, mu, 1, numberDelays);
    vDSP_vdiv(feedbackTapGains, 1, mu, 1, mu, 1, numberDelays);
    
    float sumbeta = 0.0;
    float sumup = 0.0;
    for (int i = 0; i<numberDelays; i++){
        sumbeta += beta[i];
        sumup += upsilon[i];
    }
    
    printf("Sumbeta: %f sumUp: %f \n", sumbeta, sumup);
    
    for (int i = 0; i< numberDelays; i++){
        //        mu[i] *= powf(-1, rand()%2);
        //        upsilon[i] *=powf(-1, rand()%2);
        //        printf("i: %i, beta %f upsilon %f mu %f feedbacktapGains %f\n", i, beta[i], upsilon[i], mu[i], feedbackTapGains[i]);
        totalInputEnergy += mu[i] * mu[i];
    }
    
    printf("Total Input Energy: %f \n", totalInputEnergy);
    printf("Input energy should be : %f \n", correctInputEnergy);
    printf("We feed too much input energy by a factor of : %f (if < 1 then we put too little, if > 1 then we put too much)\n", totalInputEnergy/correctInputEnergy);
    
    
    return correctInputEnergy - totalInputEnergy;
}


void Gains::getGains(float *inputGains, float *outputGains){
    for (int i = 0; i<numberDelays; i++){
        inputGains[i] = mu[i];
        outputGains[i] = upsilon[i];
    }
}



void Gains::cartesianToSpherical(Vector3D x, float *angles){
    
    angles[0] = acosf(x.normalize().z);
    angles[1] = acosf(x.normalize().x / sqrtf(powf(x.normalize().x, 2)+ powf(x.normalize().x, 2)));
}


