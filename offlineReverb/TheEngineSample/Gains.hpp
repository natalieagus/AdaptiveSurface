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
#include "CuboidGroup.hpp"
#include <math.h>
#include <stdio.h>

//#import "energyTerm.hpp"



//change them to make the first order reflections lose energy if you have to.
#define NUM_MONTECARLO 100
#define KD 1.0f
#define KS 0.0f
#define ENERGYINITIAL 1.0f
#define ALPHA 0.0056f

typedef struct Gains{
    

    Gains(){
    
    }
    
    /*Constructor with arguments for CuboidGroup
     *
     *
     *@param    dmin                minimum distance of S and L to any geometry in the room
     *@param    feedbackTapGains    computed using Jot's formula
     *@param    RT60                reverberation time in seconds
     *@param    energyReceived      the amound of energy "visible" to listener
     *@param    Room                the room object consisting of groups of patches
     *
     */
    Gains(float dmin, float* feedbackTapGains, float RT60, float energyReceived, CuboidGroup* Room){
        
        this->dmin = dmin ;
        this->numberDelays = Room->total_number_of_surface_groups_in_the_room;
        this->totalSurfaceArea = Room->cube.area;
        this->totalInputEnergy = 0.f;
        this->correctInputEnergy = 4.f * M_PI * powf(dmin, 2) * ENERGYINITIAL * (float) Room->numOfBauerRays / (float) Room->total_number_of_surface_groups_in_the_room;
        this->feedbackTapGains = feedbackTapGains;
        this->energyReceived = energyReceived;
        
        //sabine's equation
        this->absorptionCoefficient = 0.1611f * Room->cube.volume / (RT60 * this->totalSurfaceArea);
        
        //print all the variables
        printf("Gain class init, dmin %f, numDelays %i, totalSurfaceArea %f, totalInputEnergy %f, correctInputEnergy %f, energyReceived %f, absCoeff %f \n", dmin, numberDelays, totalSurfaceArea, totalInputEnergy, correctInputEnergy, energyReceived, absorptionCoefficient);
        
        
        
    }
    
    /*Constructor with arguments for Cuboid
     *
     *
     *@param    dmin                minimum distance of S and L to any geometry in the room
     *@param    numberDelays        the number of delay lines that represent patch in a room
     *@param    totalSurfaceArea    total surface area of the room
     *@param    feedbackTapGains    computed using Jot's formula
     *@param    RT60                reverberation time in seconds
     *@param    totalVolume         total volume of the room
     *@param    energyReceived      the amound of energy "visible" to listener
     *
     */
    Gains(float dmin, int numberDelays, int smoothingDelay, float totalSurfaceArea, float* feedbackTapGains, float RT60, float totalVolume, float energyReceived){
        
        this->dmin = dmin ;
        this->numberDelays = numberDelays;
        this->totalSurfaceArea = totalSurfaceArea;
        this->totalInputEnergy = 0.f;
        this->correctInputEnergy = 4.f * M_PI * powf(dmin, 2) * ENERGYINITIAL * (float) (numberDelays+smoothingDelay)/(float) numberDelays;
        this->feedbackTapGains = feedbackTapGains;
        this->energyReceived = energyReceived;
        
        //sabine's equation
        this->absorptionCoefficient = 0.1611f * totalVolume / (RT60 * this->totalSurfaceArea);
    };
    
    
    
    //methods for paper 2
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
    
    //methods for paper 5
    float calculateGainsGroup(CuboidGroup* Room, Vector3D L, Vector3D S);
    float monteCarloBeta_Squared_Group(Vector3D *points, Vector3D L, Vector3D S, Vector3D N);
    float monteCarloUpsilon_Squared_Group(Vector3D *points, Vector3D L, Vector3D S, Vector3D N);

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
