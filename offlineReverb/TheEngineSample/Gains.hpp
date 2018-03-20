//
//  Gains.hpp
//  surfaceIntegration
//
//  Created by Natalie Agus on 31/8/16.
//  Copyright © 2016 Hans. All rights reserved.
//

#ifndef Gains_hpp
#define Gains_hpp
#include "MonteCarlo.hpp"
#include "Cuboid.hpp"
#include "Vector3D.hpp"
#include "Plane3D.hpp"
#include "Cuboid.hpp"
#include <math.h>
#include <stdio.h>
//#import "energyTerm.hpp"



//change them to make the first order reflections lose energy if you have to.
#define NUM_MONTECARLO 50
#define KD 1.0f
#define KS 0.0f
#define ENERGYINITIAL 1.0f
#define ALPHA 0.0056f

typedef struct Gains{
    
    Gains(){
    
    }
    //Constructor with arguments
    Gains(float dmin, int numberDelays, float totalSurfaceArea, float* feedbackTapGains, float RT60, float totalVolume, float energyReceived){
        this->dmin = dmin ;
        this->numberDelays = numberDelays;
        this->totalSurfaceArea = totalSurfaceArea;
        this->totalInputEnergy = 0.f;
        this->correctInputEnergy = 4.f * M_PI * powf(dmin, 2) * ENERGYINITIAL;
        this->feedbackTapGains = feedbackTapGains;
        this->energyReceived = energyReceived;
        
        //sabine's equation
        this->absorptionCoefficient = 0.1611f * totalVolume / (RT60 * this->totalSurfaceArea);
    };
    
    
    
    //methods
    float calculateGains(Plane3D* surfaces, Vector3D L, Vector3D S);
    float absorptionCoefficient;

    void monteCarloUpsilon(Vector3D* points, Vector3D L, Vector3D S, Vector3D N, int numPoints, float* up, float area);
    void monteCarloBeta(Vector3D* points, Vector3D L, Vector3D S, Vector3D N, int numPoints, float* beta, float area);
    float calculateBeta(Vector3D x, Vector3D L, Vector3D S, Vector3D N, float visibilitySX, float visibilityXL);
    float integrateUpsilon(Vector3D x, Vector3D L, Vector3D N, float visibility);
    float getDBRDF();
    float pointCollectionFunction(Vector3D x, Vector3D L, Vector3D N, float visibility, float absorption);
    float reflectionKernel(Vector3D x, Vector3D L, Vector3D S, Vector3D N, float visibility);
    Vector3D Lambda(Vector3D u, Vector3D x);
    void getGains(float* inputGains, float* outputGains);
    void cartesianToSpherical(Vector3D x, float* angles);

    //variables
    float dmin;
    float totalSurfaceArea;
    int numberDelays;
    float energyReceived;
    
    float* upsilon;
    float* mu;
    float* beta;
    float* feedbackTapGains;

    
    float totalInputEnergy;
    float correctInputEnergy;

    
    
}Gains;

#endif /* Gains_hpp */
