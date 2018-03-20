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
        hInt += (area * pointCollectionFunction(points[i], L, N, 1.0f, 0.0f))/(float)numPoints;
    }
    
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
    
    Vector3D points [NUM_MONTECARLO];
    
    printf("Number delays : %d \n", numberDelays);
    for (int i = 0; i < numberDelays; i++){

        Vector3D c = surfaces[i].corner;
        Vector3D s1 = surfaces[i].S1;
        Vector3D s2 = surfaces[i].S2;

        randomPointsOnRectangle(c, s1, s2, points, NUM_MONTECARLO);
//        printf("\n\n ");
//        for (int k = 0; k<NUM_MONTECARLO; k++){
//            printf("{%f, %f, %f},", points[k].x, points[k].y, points[k].z );
//        }
        
//                printf("Surfaces %f %f %f, %f %f %f, %f %f %f \n", surfaces[i].corner.x, surfaces[i].corner.y, surfaces[i].corner.z, surfaces[i].S1.x, surfaces[i].S1.y, surfaces[i].S1.z, surfaces[i].S2.x,surfaces[i].S2.y, surfaces[i].S2.z  );
        monteCarloUpsilon(points, L, S, surfaces[i].normal, NUM_MONTECARLO, &upsilon[i], surfaces[i].getArea());
        monteCarloBeta(points, L, S, surfaces[i].normal, NUM_MONTECARLO, &beta[i], surfaces[i].getArea());
//        printf("i: %d Beta %f \n", i, beta[i]);
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


