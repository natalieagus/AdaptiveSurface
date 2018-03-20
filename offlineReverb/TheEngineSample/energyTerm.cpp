//
//  energyTerm.cpp
//  TheEngineSample
//
//  Created by Natalie Agus on 13/9/16.
//  Copyright © 2016 A Tasty Pixel. All rights reserved.
//

#include "energyTerm.hpp"
//
//  BRDFNormalizationTerm.cpp
//  TheEngineSample
//
//  Created by Natalie Agus on 13/9/16.
//  Copyright © 2016 A Tasty Pixel. All rights reserved.
//



void energyTerm::fillNormalizationTerm()
{

    
    collectPointsOnHemisphere();
    
    float thetaStep = M_PI/2.f/THETASEGMENTATION;

    int index = 0;
    
    SpecularBRDFnormalization = new float[THETASEGMENTATION];
    
    for (int i = 0; i<THETASEGMENTATION; i++){
        

            Vector3D S = unitSphericalToCartesianConversion(thetaStep * (float)(i) + thetaStep/2.0f, M_PI/4);
            
            SpecularBRDFnormalization[index] = 1.0f/getEnergyTerm(S, Vector3D(0,0,1.f)) * ENERGYREDUCTIONSPEC;
            
           // printf(" theta: %f val : %f \n",thetaStep * (float)(i) + thetaStep/2,SpecularBRDFnormalization[index]);
            index ++;

    }

    
}



float energyTerm::getEnergyTerm(Vector3D S, Vector3D N){
    //        for (int i = 0; i< monteCarlo; i++){
    //        printf("{%f,%f,%f},",hemispherePoints[i].x,hemispherePoints[i].y, hemispherePoints[i].z);
    //    }
    
    
    float montecarloBRDF = 0.0f;
    
    float HMA = 2.0 * M_PI; //Hemisphere area
    
    for (int i = 0; i < MONTECARLOBRDF; i++){
        float phongVal =  phongSpecularUnormalizedValue(S, hemispherePoints[i], N);
        
        montecarloBRDF +=  HMA *  phongVal / MONTECARLOBRDF;
        
    }
    
    return montecarloBRDF;
    
}

float energyTerm::phongSpecularUnormalizedValue(Vector3D S, Vector3D L, Vector3D N){
    
    float kS = 1.0f;
    Vector3D R = directionVectorCalculation(S, N);
    //
    //    printf("R %f %f %f \n", R.x, R.y, R.z);
    //    printf("S %f %f %f \n", S.x, S.y, S.z);
    float cosAlpha = R.dotProduct(L);
    
    if(cosAlpha < 0.0f){
        cosAlpha = 0.0f;
    }
    
    
    float cosOutput = L.dotProduct(N);
    
    //Even if L is near the R, we still can't get the maximum impact if R != N because of the cosOutput term, that
    //forces L to be near N for a large result
    float answer = kS * powf(cosAlpha, NSPECULAR) * cosOutput;
    
    return answer;
}


Vector3D energyTerm::unitSphericalToCartesianConversion(float theta, float phi){
    //theta is from z to xy plane
    //phi is xy plane, from x to y (azimuth way)
    return Vector3D(sinf(theta) * cosf(phi), sinf(theta) * sinf(phi), cosf(theta));
    
}

Vector3D energyTerm::directionVectorCalculation(Vector3D S, Vector3D N){
    return N.scalarMult(2*S.normalize().dotProduct(N)).subtract(S.normalize());
}


void energyTerm::collectPointsOnHemisphere(){
    hemispherePoints = new Vector3D[MONTECARLOBRDF];
    
//    srand(time(NULL));
    
    float LO = -1.0f;
    float HI = 1.0f;
    
    int index = 0;
    while (index < MONTECARLOBRDF){
        
        Vector3D pt =  Vector3D(LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO))), LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO))), static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI))));
        
        if (pt.magnitude() < 1.0f){
            //keep the point
            hemispherePoints[index] = pt.normalize();
            index ++;
        }
    }
    //    for (int i = 0; i < numPoints; i++){
    //        hemispherePoints[i] = Vector3D(LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO))), LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO))), static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI)))).normalize();
    //    }
    
    
}







