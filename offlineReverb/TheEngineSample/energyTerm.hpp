//
//  energyTerm.hpp
//  TheEngineSample
//
//  Created by Natalie Agus on 13/9/16.
//  Copyright © 2016 A Tasty Pixel. All rights reserved.
//

#ifndef energyTerm_hpp
#define energyTerm_hpp

#include <stdio.h>
#import "Vector3D.hpp"
#import "Plane3D.hpp"
#include <stdlib.h>
#include <time.h>
#define MONTECARLOBRDF 10
#define THETASEGMENTATION 1

#define NSPECULAR 100
#define ENERGYREDUCTIONSPEC 1.0f
#define ENERGYREDUCTIONDIF 1.0f
#define ENERGYREDUCTIONDIFREB 1.0f

typedef struct energyTerm{
    energyTerm(){
        this->n = NSPECULAR;
        fillNormalizationTerm();
    };
    
    
    energyTerm(int n){
        this->n = n;
        
        fillNormalizationTerm();
    }
    
    void collectPointsOnHemisphere();
    float phongSpecularUnormalizedValue(Vector3D S, Vector3D L, Vector3D N);
    float getEnergyTerm(Vector3D S, Vector3D N);
    void fillNormalizationTerm();
    Vector3D unitSphericalToCartesianConversion(float theta, float phi);
    Vector3D directionVectorCalculation(Vector3D S, Vector3D N);
    
    int n;
    Vector3D* hemispherePoints;
    float* SpecularBRDFnormalization;
    
    
    
    
}energyTerm;


#endif /* energyTerm_hpp */
