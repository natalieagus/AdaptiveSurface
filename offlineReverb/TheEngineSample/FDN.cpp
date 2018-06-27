#include "FDN.h"
#include <math.h>
#include <cstdlib>
#include <cstring>
#include <assert.h>
#include <random>
#include "Cuboid.hpp"
#include "CuboidGroup.hpp"
#include "Gains.hpp"
#include <stdlib.h>     /* srand, rand */
#include <random>
#include <stdio.h>
#include <iostream>
#include <fstream>

#include <time.h>       /* time */

FDN::~FDN(){
    if (delayBuffers) delete[] delayBuffers;
}

FDN::FDN(int roomType)
{
//    bool powerSaveByDefault = false;
    numDelays = TOTALDELAYS;
    delayUnits = DELAYUNITSSTD;
    delayBuffers = NULL;
    parametersFDN =  Parameter(roomType);
    this->roomType = parametersFDN.roomType;
    setParameterSafe(parametersFDN);
}



void FDN::impulseResponse(long numSamples, std::ofstream* outputFileLeft, std::ofstream* outputFileRight){

    
    printf("\n\n======IR BEGIN=======\n\n");
    clock_t begin = clock();
    
    float input;
    
    float zero = 0.0f;
    float one = 1.0f;
    float left = 0.0f;
    float right = 0.0f;
    
    
    /*
     *  Gain model on: ER and diffusion run in parallel, diffusion output to the
     *  FDN input.
     */

    numSamples = 44100*5.0;
//    numSamples = parametersFDN.RT60 * 2.0f * 44100.f;
    
    for (int i = 0; i < numSamples; i++){
        input = i==0 ? one : zero;
        processReverb(&input, &left, &right);
        *outputFileLeft << left << ",";
        *outputFileRight << right << ",";
        
    }
    
    clock_t end = clock();
    
    double elapsed_msecs = double(end - begin) / CLOCKS_PER_SEC * 1000;
    printf("Time elapsed miliseconds: %f\n", elapsed_msecs);
    
    printf("\n\n======IR End=======\n\n");
   
    *outputFileLeft << "\n";
    *outputFileRight << "\n";
}


void FDN::setParameterSafe(Parameter params)
{

    printf("\n\n======Setting Begin=======\n\n");
    clock_t begin = clock();
    
    ///************************************************////
    ///*********Setting Room Parameters****************////
    ///************************************************////
    parametersFDN = params;
    listenerOrientation = parametersFDN.orientation;
    reverbOn = parametersFDN.roomWidth > 0.05;
    
    printf("RT60: %f, room width : %f, room length : %f roomHeight: %f\n", parametersFDN.RT60, parametersFDN.roomWidth, parametersFDN.roomHeight, parametersFDN.roomCeiling);
    printf("Listenerloc : %f %f ssloc : %f %f \n", parametersFDN.listenerLoc.x, parametersFDN.listenerLoc.y, parametersFDN.soundSourceLoc.x, parametersFDN.soundSourceLoc.y);
    
    ///************************************************////
    ///*********Reset Input and Output Gains, and delay lengths to zero****************////
    ///************************************************////
    memset(inputGains, 0, TOTALDELAYS * sizeof(float));
    memset(outputGains, 0, TOTALDELAYS * sizeof(float));
    memset(delayTimes, 0, TOTALDELAYS * sizeof(int));
    memset(delayTimesChannel, 0,  TOTALDELAYS * sizeof(size_t));
    
    ///************************************************////
    ///*********Setting Room Segmentations*************////
    ///************************************************////
#ifdef NORMAL_METHOD
    Room = Cuboid(parametersFDN.roomWidth, parametersFDN.roomHeight, parametersFDN.roomCeiling);
    
//    Room.segmentCubeBasedOnProjectedArea(TOTALDELAYS-SMOOTHDELAY, parametersFDN.soundSourceLoc, parametersFDN.listenerLoc, WALLS_PER_SIDE_PROJ_AREA);
//    Room.sliceCube(TOTALDELAYS-SMOOTHDELAY);
    Room.sliceCube(ACTUALDELAY);
#endif
    
#ifdef LATERAL_METHOD
    printf("\nlateral method initiated \n\n");
    Room = Cuboid(parametersFDN.roomWidth, parametersFDN.roomHeight, parametersFDN.roomCeiling);
    Room.sliceCubeLateral(CUBE_DIVISION, TOTALDELAYS-SMOOTHDELAY, LATERAL_DIVISION, parametersFDN.listenerLoc);
#endif
    
    //For 5B
#ifdef SPHERICAL_METHOD
    printf("\n spherical method initiated \n\n");
    RoomGroup = CuboidGroup(parametersFDN.roomWidth, parametersFDN.roomHeight, parametersFDN.roomCeiling, WALLS_PER_SIDE_INIT * WALLS_PER_SIDE_INIT);
    RoomGroup.assign_and_group_SurfacesBasedOnNearestNeighbour_inRoom(parametersFDN.listenerLoc, ACTUALDELAY);
#endif

    
    ///************************************************////
    ///*********Setting Delay Lines*************////
    ///************************************************////
#ifdef NORMAL_METHOD
    mean_spherical_length = Room.getDelayValues(delayTimes, parametersFDN.listenerLocLeftEar, parametersFDN.listenerLocRightEar, parametersFDN.soundSourceLoc, SAMPLE_RATE_F);
    createNoInputNoOutputDelayLines(ACTUALDELAY, GHOSTDELAY);
    printf("Room elements %d \n", Room.elements);
#endif
    
#ifdef LATERAL_METHOD
    Room.getDelayValues(delayTimes, parametersFDN.listenerLocLeftEar, parametersFDN.listenerLocRightEar, parametersFDN.soundSourceLoc, SAMPLE_RATE_F);
    printf("Room elements %d \n", Room.elements);
#endif

#ifdef SPHERICAL_METHOD
    mean_spherical_length = RoomGroup.getDelayValues(delayTimes, parametersFDN.listenerLocLeftEar, parametersFDN.listenerLocRightEar, parametersFDN.soundSourceLoc, SAMPLE_RATE_F);
    createNoInputNoOutputDelayLines(RoomGroup.total_number_of_surface_groups_in_the_room, RoomGroup.rays_without_patches+GHOSTDELAY);
#endif
    
    ///************************************************////
    ///*********Setting FDN Parameters*****************////
    ///************************************************////
    // this is required to keep the mixing matrices unitary
    matrixAttenuation = 1.0/sqrt((float)DELAYSPERUNIT);
    resetTapAttenuation(parametersFDN.RT60);
    
    
    ///************************************************////
    ///*********Setting Input Output Gains*************////
    ///************************************************////
#ifdef NORMAL_METHOD
    GainValues = Gains(DMIN, Room.elements, SMOOTHDELAY, Room.area, feedbackTapGains, parametersFDN.RT60, Room.volume, parametersFDN.energyReceived);
    float insufficiency = GainValues.calculateGains(Room.segmentedSides, parametersFDN.listenerLoc, parametersFDN.soundSourceLoc);
#endif
    
#ifdef LATERAL_METHOD
    GainValues = Gains(DMIN, Room.elements, SMOOTHDELAY, Room.area, feedbackTapGains, parametersFDN.RT60, Room.volume, parametersFDN.energyReceived);
    float insufficiency = GainValues.calculateGains(Room.segmentedSides, parametersFDN.listenerLoc, parametersFDN.soundSourceLoc);
#endif
    
#ifdef SPHERICAL_METHOD
    //For 5B
    GainValues = Gains(DMIN, feedbackTapGains, parametersFDN.RT60, parametersFDN.energyReceived, &RoomGroup);
    float insufficiency = GainValues.calculateGainsGroup(&RoomGroup, parametersFDN.listenerLoc, parametersFDN.soundSourceLoc);
#endif
    
    ///************************************************////
    ///*********Getting Input Output Gains*************////
    ///************************************************////
    
#ifdef NORMAL_METHOD
    GainValues.getGains(inputGains, outputGains);
    if (GHOSTDELAY > 0){
        //get the last index of nonzero gain
        int index_of_injection_gain = ACTUALDELAY;
        float energy_to_add_per_delay_line = (GainValues.totalInputEnergy * ((float)TOTALDELAYS / (float)ACTUALDELAY)  - GainValues.totalInputEnergy) / ((float) GHOSTDELAY);
        printf("energy to be added %f \n", energy_to_add_per_delay_line );
        for (int i = 0; i<GHOSTDELAY; i++){
            //spread energy among delays with no output tap
            inputGains[i+index_of_injection_gain] = sqrtf(energy_to_add_per_delay_line);
        }
    }

#endif
#ifdef SPHERICAL_METHOD
    assert(RoomGroup.total_number_of_surface_groups_in_the_room + GHOSTDELAY + RoomGroup.rays_without_patches == TOTALDELAYS);
    GainValues.getGains(inputGains, outputGains);
    //handle energy injection
    if (GHOSTDELAY > 0 || RoomGroup.rays_without_patches > 0){
        //get the last index of nonzero gain
        int index_of_injection_gain = RoomGroup.total_number_of_surface_groups_in_the_room;
        float energy_to_add_per_delay_line = (GainValues.totalInputEnergy * ((float) TOTALDELAYS / (float)RoomGroup.total_number_of_surface_groups_in_the_room) - GainValues. totalInputEnergy ) / ((float)GHOSTDELAY + (float)RoomGroup.rays_without_patches);
        printf("energy to be added %f \n", energy_to_add_per_delay_line );
        //spread energy among delays with no output tap
        for (int i = 0; i<GHOSTDELAY+RoomGroup.rays_without_patches; i++){
            inputGains[i+index_of_injection_gain] = sqrtf(energy_to_add_per_delay_line);
        }
    }
#endif
    
    totalEnergyAfterAttenuation = GainValues.totalInputEnergy;
    insufficiency = GainValues.correctInputEnergy - totalEnergyAfterAttenuation;
    printf("New input energy after attenuation before adjustment, %f \n", totalEnergyAfterAttenuation);
    
    
    ///************************************************////
    ///*********Setting Delay Angles ******************////
    ///************************************************////
    
#ifdef NORMAL_METHOD
    setDelayOutputChannels();
#endif
    
#ifdef LATERAL_METHOD
    setDelayOutputChannels();
#endif
    
#ifdef SPHERICAL_METHOD
    //for 5B
    setDelayOutputChannelsPatchGroup();
#endif


    ///************************************************////
    ///*********Energy Balancing Section****************////
    ///************************************************////
#ifdef ENERGY_BALANCE
    
    printf("insufficiency : %f \n", insufficiency);
    
    //This is the case where there's too little energy
    if (insufficiency > 0.0f){
        //set the mu of the last delay (delay no output) to top up to the energy
        float inputEnergyNoOutputDelayLine = insufficiency + ((float)TOTALDELAYS/float(TOTALDELAYS-SMOOTHDELAY) * GainValues.correctInputEnergy) - GainValues.correctInputEnergy;
        printf("Input energy to top up : %f \n", inputEnergyNoOutputDelayLine);
        float muOnNoOutputDelayLine = sqrtf(inputEnergyNoOutputDelayLine/SMOOTHDELAY);
        
        for (int l = TOTALDELAYS-SMOOTHDELAY; l<TOTALDELAYS; l++){
        inputGains[l] = muOnNoOutputDelayLine;
        }

        
    }
    //This is the case where enough energy
    else if (insufficiency == 0.0f){
        float inputEnergyOnThisDelayLine = ((float)TOTALDELAYS / float(TOTALDELAYS-SMOOTHDELAY) * GainValues.correctInputEnergy)  - GainValues.correctInputEnergy;
        float muOnThisDelayLine = sqrtf(inputEnergyOnThisDelayLine);
        inputGains[TOTALDELAYS-SMOOTHDELAY] = muOnThisDelayLine;
    }
    //This is the case where there's too much energy
    else if (insufficiency < 0.0f){
        excessEnergy = true;
        float currentInputEnergy = totalEnergyAfterAttenuation;
        
        int multiTapIndex = 0;
        
        while (currentInputEnergy > GainValues.correctInputEnergy){
            
            float maxInGain = 0.0f;
            float maxInGainSq = 0.0f;
            int indexMaxInGain = 0;
            
            for (int i = 0; i<TOTALDELAYS-SMOOTHDELAY; i++){
                if (inputGains[i] * inputGains[i] > maxInGainSq){
                    maxInGain = inputGains[i];
                    maxInGainSq = inputGains[i] * inputGains[i];
                    indexMaxInGain = i;
                }
            }
            
            printf("Index : %d caused excess, delay length: %d, removing this\n", indexMaxInGain, delayTimes[indexMaxInGain]);
            
            size_t dVal = delayTimes[indexMaxInGain];
            indicesL[multiTapIndex] = dVal;
            float betaVal = inputGains[indexMaxInGain] * outputGains[indexMaxInGain];
            gainsL[multiTapIndex] = betaVal;
            
#ifdef      NORMAL_METHOD
            multiDelayLinePoints[multiTapIndex] = Room.segmentedSides[indexMaxInGain].getMidpoint();
#endif
            
#ifdef      LATERAL_METHOD
            multiDelayLinePoints[multiTapIndex] = Room.segmentedSides[indexMaxInGain].getMidpoint();
#endif
            
#ifdef      SPHERICAL_METHOD
            multiDelayLinePoints[multiTapIndex] = RoomGroup.intersectionPointsInRoom_singleArray[indexMaxInGain];
#endif
            multiTapGains[multiTapIndex] =  gain(parametersFDN.RT60, delayTimes[indexMaxInGain]);
            
            //transform this into no inputGain
            inputGains[indexMaxInGain] = 0.0f;
            
            
            //recalculate total energy
            float newInputEnergy = 0.0f;
            for (int i = 0; i< TOTALDELAYS-SMOOTHDELAY; i++){
                newInputEnergy += inputGains[i] * inputGains[i];
            }
            
            

            currentInputEnergy = newInputEnergy;
            multiTapIndex++;

        }
        
        multiTapDelayTapsNumber = multiTapIndex;
        
        //recalculate total energy
        float newInputEnergy = 0.0f;
        for (int i = 0; i< TOTALDELAYS; i++){
            newInputEnergy += inputGains[i] * inputGains[i];
        }
        
        printf("New input Energy after take off the excess: %f \n" , newInputEnergy);
        //adjust the value of noOutputGain
        float insufficientEnergyDueToRemoval = (GainValues.correctInputEnergy - currentInputEnergy);
        float inputEnergyNoOutputDelayLine = insufficientEnergyDueToRemoval + (((float)(TOTALDELAYS)/(float)(TOTALDELAYS-SMOOTHDELAY)) * GainValues.correctInputEnergy) - GainValues.correctInputEnergy;
        
        
        float muOnNoOutputDelayLine = sqrtf(inputEnergyNoOutputDelayLine/SMOOTHDELAY);
        for (int l = TOTALDELAYS-SMOOTHDELAY; l<TOTALDELAYS; l++){
            inputGains[l] = muOnNoOutputDelayLine;
        }
    }
    
    //recalculate total energy
    float newTotalInputEnergy = 0.0f;
    
    for (int i = 0; i< TOTALDELAYS; i++){
        newTotalInputEnergy += inputGains[i] * inputGains[i];
    }
    
    printf("Previous totalEnergy: %f, New total input energy: %f, correct energy: %f, effectiveEnergyOut without the nogain delay line: %f \n", GainValues.totalInputEnergy, newTotalInputEnergy, GainValues.correctInputEnergy, newTotalInputEnergy * (float)(TOTALDELAYS-SMOOTHDELAY)/(TOTALDELAYS));
    
    //Init Multi-tap Delay line when there's excess energy
    if (excessEnergy){
        BMMultiTapDelay_init(&multiTapDelay, false, indicesL, indicesR, gainsL, gainsR, multiTapDelayTapsNumber,0);
        
        setMultiTapDelayChannels();
    }
#endif
    
    
    ///************************************************////
    ///**Setting FDN Delay length without output********////
    ///************************************************////
    setDelayNoOutputLength();

    
    ///************************************************////
    ///**************Setting Direct Ray Line***********////
    ///************************************************////
    setDirectDelayTimes();
    setDirectGains();
    setDirectRayAngles();
    setDirectSingleTapDelay();

    
    ///************************************************////
    ///**************Setting ITD Banks*****************////
    ///************************************************////
    setTempPoints();
    calculateAdditionalDelays();
    
    
    ///************************************************////
    ///**************Setting HRTF Filters**************////
    ///************************************************////
    setFilters();
    

    
    ///************************************************////
    ///**************Preparing the buffers**************////
    ///************************************************////
    printf("\n{");
    int totalDelayTime = 0;
    for(int i = 0; i < numDelays-1; i++){
//        printf("%i,", delayTimes[i]);
        totalDelayTime += delayTimes[i];
        delayTimesSecond[i] = (float) delayTimes[i] / SAMPLE_RATE_F;
        
    };
//    printf("%i", delayTimes[numDelays-1]);
//    printf("};\n");
    
    //comment this later after thesis is done
//    exit(1);
    
    printf("totaldelay  %d \n ", totalDelayTime);
    resetDelay(totalDelayTime);
    resetReadIndices();
    
    ///************************************************////
    ///**************Set HSF Filters *****************////
    ///************************************************////
    
    //Highshelf, for lesser decay time on higher frequencies
    BMBiquadArray_FDNHighShelfInit(&highShelfFilter, delayTimesSecond, parametersFDN.hsffreq, parametersFDN.RT60, parametersFDN.hsfRT60, TOTALDELAYS, SAMPLE_RATE_F);
    
    //Lowshelf, for lesser decay time on lower frequencies
    BMBiquadArray_FDNLowShelfInit(&lowShelfFilter, delayTimesSecond, parametersFDN.lsffreq, parametersFDN.RT60, parametersFDN.lsfRT60, TOTALDELAYS, SAMPLE_RATE_F);
    

    //Lowpass, dampen high frequencies
    LowPassLeft = MultiLevelBiQuadFilter();
    LowPassLeft.setLowPass(parametersFDN.lowpass);
    
    LowPassRight = MultiLevelBiQuadFilter();
    LowPassRight.setLowPass(parametersFDN.lowpass);
    
    
    //Bell, boost between 2-7kHz, peak at 3.5khz to add more realistic effect on head shadowing
    BellLeft = MultiLevelBiQuadFilter();
    BellLeft.setBell(parametersFDN.bellPeak, parametersFDN.bellBW, parametersFDN.bellGain, 1);
    
    BellRight = MultiLevelBiQuadFilter();
    BellRight.setBell(parametersFDN.bellPeak, parametersFDN.bellBW, parametersFDN.bellGain, 1);
    



    //    printf("Printing Parameters: \n");
    //
        for (int i = 0 ; i < numDelays ; i++){
            printf("Index : %d, delay : %d, inputGain : %f, outputGain: %f, feedbacktapgain: %f \n",i, delayTimes[i], inputGains[i], outputGains[i], feedbackTapGains[i]);
    
    
    
        }
    
    
    clock_t end = clock();
    
    double elapsed_msecs = double(end - begin) / CLOCKS_PER_SEC * 1000;
    printf("Time elapsed miliseconds for setup and init: %f\n", elapsed_msecs);

    printf("\n\n======Setting End=======\n\n");
}



inline void FDN::processReverb(float* pInput, float* pOutputL, float* pOutputR)
{
    

    // copy the filtered input so that we can process it without affecting the original value
    float xn = *pInput;
    float fdnTankOutsNew[CHANNELS] = {0};
    
    
    ///************************************************////
    ///**************Direct Rays***********************////
    ///************************************************////
    float directRaysOutput[2] = { xn * directAttenuation, xn * directAttenuation };
    processDirectRaysDelay(directRaysOutput, directRaysOutput); //delays the rays
    
    // copy output taps to pre-filtered output buffer
    //rwIndices are output taps pointer
    
    vDSP_vgathra (const_cast<const float**>(rwIndices), 1, outputsPF, 1, numDelays);
    

    
    
    
    ///************************************************////
    ///************Apply Shelf Filters*************////
    ///************************************************////
    // apply a first order high-shelf filter to the feedback path
    //
    // This filter structure is direct form 2 from figure 14 in section 1.1.6
    // of Digital Filters for Everyone by Rusty Alred, second ed.
    //
    // t = outputsPF + (a1 * z1);
//
//    vDSP_vma (a1, 1, z1, 1, outputsPF , 1, t, 1, numDelays);
//    // outputsAF = b0*t + b1*z1;
//    vDSP_vmma(b0, 1, t, 1, b1, 1, z1, 1, outputsAHSF , 1, numDelays);
//
//    // z1 = t;
//    memcpy(z1, t, numDelays * sizeof(float));
    BMBiquadArray_processSample(&highShelfFilter, outputsPF, outputsAHSF, TOTALDELAYS);
    
    BMBiquadArray_processSample(&lowShelfFilter, outputsAHSF, outputsAF, TOTALDELAYS);
    
    //Processing outputs
    vDSP_vmul(outputGains, 1, outputsAF, 1, outputsBinaural, 1, numDelays);
    
    ///************************************************////
    ///************Handle Excess Energy*****************////
    ///************************************************////
    if (excessEnergy){
        //get output from multiTaps
        BMMultiTapDelay_processMultiChannelOut(&multiTapDelay, xn, multiTapOutputs);
        
        vDSP_vmul(multiTapGains, 1, multiTapOutputs, 1, multiTapOutputs, 1, multiTapDelayTapsNumber);
        
        //add the output from multiTaps to tankout
        for (int i = 0; i<multiTapDelayTapsNumber; i++){
            size_t channel = multiDelayLinePointsChannel[i];
            fdnTankOutsNew[channel] += multiTapOutputs[i];
        }
    }
    
    //Multiplexer
    processTankOut(fdnTankOutsNew);
    
    //HRTF (Reverb and Direct)
    float fdnTankOutLeft[CHANNELS] = {};
    float fdnTankOutRight[CHANNELS] = {};
    filterChannelsHRTF(fdnTankOutsNew, directRaysOutput, fdnTankOutLeft, fdnTankOutRight); //HRTF
    
    //ITD
    addReverbDelay(fdnTankOutLeft, fdnTankOutRight); //Temp point delays
    
    //Sum Reverb output
    float reverbOut[2] = {0.0f, 0.0f};
    vDSP_sve(fdnTankOutLeft, 1, &reverbOut[0], CHANNELS);
    vDSP_sve(fdnTankOutRight, 1, &reverbOut[1], CHANNELS);
    
    float outputLeft = directRaysOutput[0] + reverbOut[0];
    float outputRight = directRaysOutput[1] + reverbOut[1];
//    printf("direct R delay setup %f \n", directRays[1].time);
//    printf("%f DRI %f DRO \n", directRaysInput[1], directRaysOutput[1]);
//
    float outputL_lowpass = 0.f;
    float outputR_lowpass = 0.f;
    
    LowPassLeft.processBuffer(&outputLeft, &outputL_lowpass, 1);
    LowPassRight.processBuffer(&outputRight, &outputR_lowpass, 1);

//    LowPassLeftTwo.processBuffer(&outputL_lowpass, &outputLeft, 1);
//    LowPassRightTwo.processBuffer(&outputR_lowpass, &outputRight, 1);
//
//    LowPassLeftThree.processBuffer(&outputLeft, &outputL_lowpass, 1);
//    LowPassRightThree.processBuffer(&outputRight, &outputR_lowpass, 1);
    
    BellLeft.processBuffer(&outputL_lowpass, pOutputL, 1);
    BellRight.processBuffer(&outputR_lowpass, pOutputR, 1);
    
    //outputs
//    *pOutputL = outputLeft;
//    *pOutputR = outputRight;
    
//    printf("%f, ", LowPass.process(outputLeft));
    
//    //Reverb
//    *pOutputL += reverbOut[0];
//    *pOutputR += reverbOut[1];
    
    
    ///************************************************////
    ///************Continue Reverb Process*************////
    ///************************************************////
    
    // scale the output taps according to the feedBackTapGains
    vDSP_vmul(feedbackTapGains, 1, outputsAF, 1, outputsAF, 1, numDelays);
    
    
    // apply the matrix attenuation before mixing
    vDSP_vsmul (outputsAF, 1, &matrixAttenuation, outputsAF, 1, numDelays);
    
    BMFastHadamardTransform(outputsAF, inputs, temp1, temp2, TOTALDELAYS);
    
    // scale and mix new input together with feedback input
    vDSP_vsmul(inputGains, 1, &xn, scaledNewInput, 1, numDelays);
    
    //Add the scaledInput with the feedback input (called inputs)
    vDSP_vadd(inputs, 1, scaledNewInput, 1, inputs, 1, numDelays);
    
    // feedback the mixed audio into the tank, shifting the feedback path over to the next unit
    size_t j;//,k;
    for (j = DELAYSPERUNIT; j < numDelays; j++) *(rwIndices[j]) = inputs[j-DELAYSPERUNIT];
    for (j = 0;j<DELAYSPERUNIT; j++) *(rwIndices[j]) = inputs[j+numDelays-DELAYSPERUNIT];
    
    incrementIndices();
}


///************************************************////
///*****Reverb Processing Functions*****************////
///************************************************////


/*!
 * Fast Hadamard Transform
 *   works in place
 *
 * @param input   an array of floats with length = length
 * @param output  an array of floats with length = length
 * @param temp1   an array of floats with length = length (temp buffer)
 * @param temp2   an array of floats with length = length (temp buffer)
 * @param length  must be a power of 2, >= 16
 */
inline void FDN::BMFastHadamardTransform(float* input,
                                    float* output,
                                    float* temp1,
                                    float* temp2,
                                    size_t length){
    // length must be a power of 2
    assert(BMPowerOfTwoQ(length));
    // length must be at least 16 because we hard-code the
    // base case at order 16.
    assert(length >= 16);
    
    size_t partitionSize = length;
    float* tmpIn = input;
    float* tmpOut = temp1;
    bool tmpState = true;
    
    // iteratively calculated recursion
    while (partitionSize > 16) {
        size_t halfPartitionSize = partitionSize >> 1;
        for (size_t i = 0; i < length; i += partitionSize){
            // copy all the lower terms into place
            memcpy(tmpOut+i,tmpIn+i,sizeof(float)*halfPartitionSize);
            memcpy(tmpOut+i+halfPartitionSize,tmpIn+i,sizeof(float)*halfPartitionSize);
            // sum all the higher terms into place
            for (size_t j=i; j<halfPartitionSize+i; j++) {
                size_t idx2 = j+halfPartitionSize;
                tmpOut[j] += tmpIn[idx2];
                tmpOut[idx2] -= tmpIn[idx2];
            }
        }
        
        // swap temp buffers to avoid using the same memory for reading and writing.
        tmpIn = tmpOut;
        tmpState = !tmpState;
        if (tmpState) tmpOut = temp1;
        else tmpOut = temp2;
        partitionSize >>= 1;
    }
    
    // base case
    for (size_t i = 0; i < length; i += 16)
        BMFastHadamard16(tmpIn + i, output + i, tmpOut);
}




/*
 * Fast Hadamard transform of length 16
 *   works in place.
 *
 * @param input   an array of 16 floats
 * @param output  an array of 16 floats
 * @param temp16  an array of 16 floats that will be over-written
 *
 */
inline void FDN::BMFastHadamard16(const float* input, float* output, float* temp16){
    // level 1
    // +
    temp16[0] = input[0] + input[8];
    temp16[1] = input[1] + input[9];
    temp16[2] = input[2] + input[10];
    temp16[3] = input[3] + input[11];
    temp16[4] = input[4] + input[12];
    temp16[5] = input[5] + input[13];
    temp16[6] = input[6] + input[14];
    temp16[7] = input[7] + input[15];
    // -
    temp16[8] = input[0] - input[8];
    temp16[9] = input[1] - input[9];
    temp16[10] = input[2] - input[10];
    temp16[11] = input[3] - input[11];
    temp16[12] = input[4] - input[12];
    temp16[13] = input[5] - input[13];
    temp16[14] = input[6] - input[14];
    temp16[15] = input[7] - input[15];
    
    
    // level 2
    //+
    output[0] = temp16[0] + temp16[4];
    output[1] = temp16[1] + temp16[5];
    output[2] = temp16[2] + temp16[6];
    output[3] = temp16[3] + temp16[7];
    //-
    output[4] = temp16[0] - temp16[4];
    output[5] = temp16[1] - temp16[5];
    output[6] = temp16[2] - temp16[6];
    output[7] = temp16[3] - temp16[7];
    
    //+
    output[8] = temp16[8] + temp16[12];
    output[9] = temp16[9] + temp16[13];
    output[10] = temp16[10] + temp16[14];
    output[11] = temp16[11] + temp16[15];
    //-
    output[12] = temp16[8] - temp16[12];
    output[13] = temp16[9] - temp16[13];
    output[14] = temp16[10] - temp16[14];
    output[15] = temp16[11] - temp16[15];
    
    
    // level 3
    // +
    temp16[0] = output[0] + output[2];
    temp16[1] = output[1] + output[3];
    // -
    temp16[2] = output[0] - output[2];
    temp16[3] = output[1] - output[3];
    
    // +
    temp16[4] = output[4] + output[6];
    temp16[5] = output[5] + output[7];
    // -
    temp16[6] = output[4] - output[6];
    temp16[7] = output[5] - output[7];
    
    // +
    temp16[8] = output[8] + output[10];
    temp16[9] = output[9] + output[11];
    // -
    temp16[10] = output[8] - output[10];
    temp16[11] = output[9] - output[11];
    
    // +
    temp16[12] = output[12] + output[14];
    temp16[13] = output[13] + output[15];
    // -
    temp16[14] = output[12] - output[14];
    temp16[15] = output[13] - output[15];
    
    
    // level 4
    output[0] = temp16[0] + temp16[1];
    output[1] = temp16[0] - temp16[1];
    output[2] = temp16[2] + temp16[3];
    output[3] = temp16[2] - temp16[3];
    
    output[4] = temp16[4] + temp16[5];
    output[5] = temp16[4] - temp16[5];
    output[6] = temp16[6] + temp16[7];
    output[7] = temp16[6] - temp16[7];
    
    output[8] = temp16[8] + temp16[9];
    output[9] = temp16[8] - temp16[9];
    output[10] = temp16[10] + temp16[11];
    output[11] = temp16[10] - temp16[11];
    
    output[12] = temp16[12] + temp16[13];
    output[13] = temp16[12] - temp16[13];
    output[14] = temp16[14] + temp16[15];
    output[15] = temp16[14] - temp16[15];
}



// is x a power of 2?
// reference: http://www.exploringbinary.com/ten-ways-to-check-if-an-integer-is-a-power-of-two-in-c/
inline bool FDN::BMPowerOfTwoQ (size_t x)
{
    return ((x != 0) && ((x & (~x + 1)) == x));
}



//ITD for reverb
inline void FDN::addReverbDelay(float* fdnLeft, float*fdnRight){
    for(int i = 0; i < CHANNELS/2; i++){
        //left ear, channel 0123 needs to be delayed by left-right time ITD
        fdnLeft[i] = reverbDelays[i].process(fdnLeft[i]); //Left ear channel 0123 gets ITD from channel 0123
    }
    for (int i = CHANNELS/2 ; i < CHANNELS; i++){
        //right ear, channel 4567 needs to be delayed by right-left time ITD
        fdnRight[i] = reverbDelays[i].process(fdnRight[i]);//Right ear channel 0123 gets ITD from channel 4567
    }
}

//Delays for direct rays (including ITD)
inline void FDN::processDirectRaysDelay(float* input, float* directRaysOutput){
    directRaysOutput[0] = directRays[0].process(input[0]);
    directRaysOutput[1] = directRays[1].process(input[1]);
}

//Multiplexer
inline void FDN::processTankOut(float fdnTankOut[CHANNELS]){
    for (size_t i = 0; i < numDelays; i++){
        size_t channel = delayTimesChannel[i];
        fdnTankOut[channel] += outputsBinaural[i];
    }
}

//HRTF
inline void FDN::filterChannelsHRTF(float fdnTankOut[CHANNELS], float directRay[2], float fdnTankOutLeft[CHANNELS], float fdnTankOutRight[CHANNELS]){
    //Filter left & right ear
    for (size_t i = 0; i < CHANNELS; i++){
        fdnTankOutLeft[i] = leftEarFilter[i].process(fdnTankOut[i]);
        fdnTankOutRight[i] = rightEarFilter[i].process(fdnTankOut[i]);
    }
    
    //Filter direct rays
    directRay[0] = directRayFilter[0].process(directRay[0]);
    directRay[1] = directRayFilter[1].process(directRay[1]);
}



///************************************************////
///*********Reset Functions************************////
///************************************************////
void FDN::resetReadIndices(){
    // set start, end indices for the first delay in the feedback tank
    rwIndices[0] = startIndices[0] = (float*)delayBuffers;
    endIndices[0] = startIndices[0] + delayTimes[0];
    
    //print delay times
    for (int i = 0; i < numDelays;i++){
        if (delayTimes[i] < 1){
            delayTimes[i] = 10;
        }
    };
    
    // set start / end indices for the second feedback delay tap onwards
    for (int i = 0 + 1; i < numDelays; i++){
        rwIndices[i] = startIndices[i] = startIndices[i-1] + delayTimes[i-1];
        endIndices[i] = startIndices[i] + delayTimes[i];
    }
    
    samplesUntilNextWrap = 0; // Initially, we trigger an unncecssary wrap operation to calculate the number of samples until the next actual wrap
}

void FDN::resetTapAttenuation(float rt60){
    for (int i = 0; i < numDelays; i++){
        feedbackTapGains[i] = gain(rt60, delayTimes[i]);// * powf(-1, (float)( rand() % 10));
//        printf("i %d Delay time: %d, gain %f \n", i, delayTimes[i], feedbackTapGains[i]);
    }
}


void FDN::resetDelay(int totalDelayTime)
{
    if (delayBuffers) delete[] delayBuffers;
    delayBuffers = new float[totalDelayTime];
    this->totalDelayTime = totalDelayTime;
    
    // clear the buffers
    memset(delayBuffers, 0, sizeof(float) * totalDelayTime);
    memset(scaledNewInput, 0, sizeof(float) * numDelays);
    memset(inputs, 0, sizeof(float) * numDelays);
    memset(outputsPF, 0, sizeof(float) * numDelays);
    memset(outputsAF, 0, sizeof(float) * numDelays);
    memset(outputsAHSF, 0, sizeof(float) * numDelays);
    memset(outputsBinaural, 0, sizeof(float) * numDelays);
    
    memset(z1, 0, sizeof(float) * numDelays); //DF2
    memset(z1a, 0, sizeof(float) * numDelays); //DF1
    memset(z1b, 0, sizeof(float) * numDelays); //DF1
    
//    randomSeed = 0;
}

inline void FDN::incrementIndices(){
    for (int i = 0; i < numDelays; i++) {
        float** rwIndex = rwIndices + i;
        // increment
        (*rwIndex)++;
        // wrap around back to the beginning if we're off the end
        //if ((*rwIndex) >= endIndices[i]) (*rwIndex) = startIndices[i];
    }
    samplesUntilNextWrap--;
    
    // if any pointer is at the end of the buffer, check all pointers and wrap back to the beginning where necessary
    if (samplesUntilNextWrap <= 0) {
        samplesUntilNextWrap = LONG_MAX;
        for (long i = 0; i< numDelays; i++){
            float** rwIndex = rwIndices + i;
            float** endIndex = endIndices + i;
            
            // wrap all pointers that need to be wrapped
            if ((*rwIndex) >= (*endIndex)) (*rwIndex) = startIndices[i];
            
            // find the distance until the next pointer needs to wrap
            long iThDistanceToEnd = (*endIndex) - (*rwIndex);
            if (iThDistanceToEnd < samplesUntilNextWrap) samplesUntilNextWrap = iThDistanceToEnd;
        }
    }
}


///************************************************////
///********High Shelf Filter***********************////
///************************************************////
// Multiplies the decay rate for high Frequencies
// hfMult >= 1
// 0 <= fc < ~18000
//
// 1 means HF decays at the same rate as mid frequencies, 2 means HF decay twice as fast,
// 3 is three times as fast, etc.
//
//
// the formulae for calculating the filter coefficients are from Digital Filters for Everyone,
// second ed., by Rusty Alred.  Section 2.3.10: Shelf Filters

void FDN::setHFDecayMultiplier(float fc, float hfMult, float rt60){
    double g[numDelays];
    double D[numDelays];
    double gamma;
    
    gamma = tan((M_PI * fc) / double(SAMPLINGRATEF));
    
    for (int i = 0; i < numDelays; i++){
        // set the filter gains
        //
        // here we find the gain already applied to each delay line
        double broadbandGain = gain(rt60, delayTimes[i]);
        double desiredHFGain = gain(rt60 / hfMult, delayTimes[i]);
        // and now find how much additional filter gain to apply for the given increase in HF decay speed
        g[i] = desiredHFGain / broadbandGain;
        assert(g[i]<= 1.0);
//        printf("Gains HSF i %f \n", g[i]);
        // just a temp variable
        D[i] = 1.0 / ((g[i] * gamma) + 1.0);
        
        // set the filter coefficients
        b0[i] = g[i] * (gamma + 1.0) * D[i];
        b1[i] = g[i] * (gamma - 1.0) * D[i];
        // Rusty Allred omits the negative sign in the next line.  We use it to avoid a subtraction in the optimized filter code.
        a1[i] = -1.0 * ((g[i] * gamma) - 1.0) * D[i];
        
//        printf("b0, b1, a1, %f %f %f \n", b0[i], b1[i], a1[i]);
    }
}



///************************************************////
///*********FDN Feedback Gains**********************////
///************************************************////
// computes the appropriate feedback gain attenuation
// to get a decay envelope with the specified RT60 time (in seconds)
// from a delay line of the specified length.
//
// This formula comes from solving EQ 11.33 in DESIGNING AUDIO EFFECT PLUG-INS IN C++ by Will Pirkle
// which is attributed to Jot, originally.
double FDN::gain(double rt60, double delayLengthInSamples) {
 //  printf("Gain : %f \n", pow(10.f, (-3.0 *  avgDelay) / (rt60 * SAMPLINGRATEF) ));
   //   printf("Gain 2 : %f \n", pow(M_E, (-3.0 *  avgDelay) / (rt60 * SAMPLINGRATEF) ));
    return pow(10.f, (-3.f *  delayLengthInSamples) / (rt60 * SAMPLINGRATEF) );
    
   // return  pow (0.1 , delayLengthInSamples/SAMPLINGRATEF);
}

void FDN::setParameter(Parameter params){
//    newParametersFDN = params;
//    parameterNeedsUpdate = true;
}






///************************************************////
///*********Direct Ray Functions*******************////
///************************************************////
inline void FDN::setDirectGains(){
    float SL = parametersFDN.soundSourceLoc.distance(parametersFDN.listenerLoc);
    directAttenuation = DMIN/(SL) * exp(-0.0056*SL) ;
}


//Setting direct delay value for two direct rays
inline void FDN::setDirectDelayTimes()
{
    
    //Calculate delay from source to receiver
    float directDelayLeft = parametersFDN.soundSourceLoc.distance(parametersFDN.listenerLocLeftEar) / SOUNDSPEED;
    float directDelayRight = parametersFDN.soundSourceLoc.distance(parametersFDN.listenerLocRightEar) / SOUNDSPEED;
    directDelayTimes[0] = directDelayLeft;
    printf("delay left %f \n", directDelayTimes[0]);

    directDelayTimes[1] = directDelayRight;
        printf("delay right %f \n", directDelayTimes[1]);
}

//Setting direct ray angle with respect to listener location
void FDN::setDirectRayAngles()
{
    float yDiff, xDiff;
    yDiff = parametersFDN.soundSourceLoc.y - parametersFDN.listenerLocLeftEar.y;
    xDiff = parametersFDN.soundSourceLoc.x - parametersFDN.listenerLocLeftEar.x;
    float angleInDegrees = atan2(xDiff, yDiff) * 180.0f / M_PI - parametersFDN.orientation;

   
    //switch to 360.f
    
    //Make it between 0 to 360
    if (fabs(angleInDegrees - 0.0f)<0.0001){
        angleInDegrees = 0.0f;
    }
    
    if (angleInDegrees < 0.0f ){
        angleInDegrees = 360.f + angleInDegrees;
    }
    
    directRayAngles[0] = angleInDegrees;
    
    float yDiff2, xDiff2;
    yDiff2 = parametersFDN.soundSourceLoc.y - parametersFDN.listenerLocRightEar.y;
    xDiff2 = parametersFDN.soundSourceLoc.x - parametersFDN.listenerLocRightEar.x;
    float angleInDegrees2 = atan2(xDiff2, yDiff2) * 180.0f / M_PI - parametersFDN.orientation;
    
    //Make it between 0 to 360
    if (fabs(angleInDegrees2 - 0.0f)<0.0001){
        angleInDegrees2 = 0.0f;
    }
    
    if (angleInDegrees2 < 0.0f ){
        angleInDegrees2 = 360.f + angleInDegrees2;
    }
    
    directRayAngles[1] = angleInDegrees2;
    
    printf("directRayAngle L %f R %f \n", directRayAngles[0], directRayAngles[1]);
}

void FDN::setDirectSingleTapDelay(){
    directRays[0].setTimeSafe(directDelayTimes[0]);
    directRays[1].setTimeSafe(directDelayTimes[1]);
    printf("\n\ndirect delay times left %f direct delay times right %f \n", directDelayTimes[0], directDelayTimes[1]);
}



///************************************************////
///*********Reverb ITD Functions******************////
///************************************************////
//Set points for additional delay to enter the ears, 8 points, 1 per channel
void FDN::setTempPoints(){
    
    float yBot = 0.0f-parametersFDN.listenerLoc.y;
    float yTop = parametersFDN.roomHeight - parametersFDN.listenerLoc.y;
    float xLeft = 0.0f - parametersFDN.listenerLoc.x;
    float xRight = parametersFDN.roomWidth - parametersFDN.listenerLoc.x;
    
    float w =parametersFDN.listenerLoc.x;
    float h = parametersFDN.listenerLoc.y;
    
    for (int i = 0; i < CHANNELS/2; i++){
        float angle = channeltoangleNormal(i) ;
//        printf("Angle %f \n", angle);
        float m = 1.0f/tan(angle * M_PI / 180.f);
        //y = mx + 0
        Vector3D pointArray[4] = {Vector3D(yBot/m, yBot),
            Vector3D(yTop/m, yTop),
            Vector3D(xLeft, m*xLeft),
            Vector3D(xRight, m*xRight)};
        
        for (int j = 0; j< 4; j++){
            float xO = pointArray[j].x + parametersFDN.listenerLoc.x;
            if (xO > parametersFDN.roomWidth or xO < 0.0f){
                pointArray[j].mark = false;
                continue;
            }
            float yO = pointArray[j].y + parametersFDN.listenerLoc.y;
            if (yO > parametersFDN.roomHeight or yO < 0.0f){
                pointArray[j].mark = false;
                continue;
            }
            if (pointArray[j].mark == true){
                //check for x value
                if (pointArray[j].x >= 0){
                    tempPoints[i].x = pointArray[j].x + w;
                    tempPoints[i].y = pointArray[j].y + h;
                    tempPoints[i].z = parametersFDN.listenerLoc.z;
                }
                else{
                    tempPoints[i+CHANNELS/2].x = pointArray[j].x + w;
                    tempPoints[i+CHANNELS/2].y = pointArray[j].y + h;
                    tempPoints[i+CHANNELS/2].z = parametersFDN.listenerLoc.z;
                }
            }
        }
    }
    
//    for (int i = 0; i<CHANNELS; i++){
////        tempPoints[i].z = lLoc.z; //set to be leveled
//        printf("{%f, %f, %f},", tempPoints[i].x, tempPoints[i].y, tempPoints[i].z);
//    }
//    
    
    
}

void FDN::calculateAdditionalDelays(){
    
    for (int i = 0; i < CHANNELS; i++)
    {
//        printf("Tempoints i %d : %f %f %f \n", i, tempPoints[i].x, tempPoints[i].y, tempPoints[i].z);
        if (i < CHANNELS/2){

            //near to right use temp0,leftear- temp0,rightear, 0 to 3, FOR LEFT EAR
            float d = (tempPoints[i].distance(parametersFDN.listenerLocLeftEar) - tempPoints[i].distance(parametersFDN.listenerLocRightEar))  / SOUNDSPEED;
            
//            printf("d channel 0123 %f \n", d);
            if(d<0.00000001){
                printf("FALSE...\n");
                d = 0.0001;
            }
            additionalDelays[i] = d;
        }
        
        else{
            //near to left use temp4,rightear - temp4,leftear, 4 to 7, FOR RIGHT EAR
            float d = (tempPoints[i].distance(parametersFDN.listenerLocRightEar) - tempPoints[i].distance(parametersFDN.listenerLocLeftEar))  / SOUNDSPEED;
//                printf("d channel 4567 %f \n", d);
            if(d<0.00000001){
                printf("FALSE...\n");
                d = 0.0001;
            }
            additionalDelays[i] = d;
        }
//         printf("DELAY ADDITIONAL : %f samples \n", additionalDelays[i]*44100);
        reverbDelays[i].setTimeSafe(additionalDelays[i]);
    }
    
}

float FDN::channeltoangleNormal(size_t channel){
    float channelWidth = 360.0f / (float)CHANNELS;
    return (float)channel * channelWidth  + (channelWidth / 2.0f);
}


///************************************************////
///********Binaural Functions************************////
///************************************************////
void FDN::setDelayOutputChannels(){
    for (size_t i = 0; i < ACTUALDELAY ; i++){
        delayTimesChannel[i] = determineChannel(Room.segmentedSides[i].getMidpoint().x, Room.segmentedSides[i].getMidpoint().y, listenerOrientation);
    }
#ifdef ENERGY_BALANCE
    delayTimes[TOTALDELAYS-SMOOTHDELAY] = 0;
#endif
}


void FDN::setDelayOutputChannelsPatchGroup(){
    
    int counter_index = 0;
    for (size_t i = 0; i < 6; i++){
        for (int j = 0; j< RoomGroup.numOfSurfaceGroupsInEachWall[i]; j++){
            Vector3D p = RoomGroup.surfaceGroups[i][j].midPoint;
            delayTimesChannel[counter_index] = determineChannel(p.x, p.y, listenerOrientation);
            counter_index ++;
        }
    }
    
#ifdef ENERGY_BALANCE
    delayTimes[TOTALDELAYS-SMOOTHDELAY] = 0;
#endif
}



void  FDN::setMultiTapDelayChannels(){
    for (int i = 0; i<multiTapDelayTapsNumber; i++){
        multiDelayLinePointsChannel[i] = determineChannel(multiDelayLinePoints[i].x, multiDelayLinePoints[i].y, listenerOrientation);
    }
}


void FDN::setFilters(){
    //Right is negative, p.151 DAFX
    for (size_t i = 0; i < CHANNELS; i++){
//        float angle = channelToAngle(i);
//        printf("i %d Channel to Angle : %f \n", i,angle);
        
        leftEarFilter[i].setAngle(convertAzimuthToLeftEar(channeltoangleNormal(i)), SAMPLINGRATEF, false);
        rightEarFilter[i].setAngle(convertAzimuthToRightEar(channeltoangleNormal(i)), SAMPLINGRATEF, true);
    }
    
    directRayFilter[0].setAngle(convertAzimuthToLeftEar(directRayAngles[0]), SAMPLINGRATEF,false);
    directRayFilter[1].setAngle(convertAzimuthToRightEar(directRayAngles[1]), SAMPLINGRATEF, true);
    
}


//float FDN::angleLeftEarAllAzimuth(float azimuth){
//    
////    //azimuth between 0 to 360
////    float azimuth = channeltoangleNormal(channel);
////    
//    //this one will be 0 to 90
//    if(270.f<= azimuth and azimuth <360.f){
//        printf("left ear all az %f \n", 360.f - azimuth);
//        return 360.f - azimuth;
//    }
//    
//    printf("left ear all az %f \n", azimuth + 90.f);
//    return azimuth + 90.f;
//    
//    
//}
//
//
//float FDN::angleRightEarAllAzimuth(float azimuth){
//    
////    //azimuth between 0 to 360
////    float azimuth = channeltoangleNormal(channel);
////    
//    //this one will be 270 to 360
//    if(0<= azimuth and azimuth < 90.f){
//        printf("right ear all az %f\n", 360.f - azimuth);
//        return 360.f - azimuth;
//    }
//    
//    printf("right ear all az %f \n", azimuth - 90.f);
//    return azimuth - 90.f;
//    
//}


float FDN::convertAzimuthToLeftEar(float azimuth){
    
//    //azimuth between 0 to 360
//    float azimuth = channeltoangleNormal(channel);
//    
    //change azimuth to be between 0 to 180 and 0 to -180
    if (azimuth >= 360.f){
        azimuth -= 360.f;
    }
    
    if (azimuth > 180.f){
        azimuth = azimuth - 360.f;
    }
    
    if(azimuth > 90.f and azimuth <= 180.f){
        float leftEarAngle = azimuth + 90.f - 360.f;
        return leftEarAngle;
    }
    
    return azimuth + 90.f;
    
    
}

float FDN::convertAzimuthToRightEar(float azimuth){
    
//    //azimuth between 0 to 360
//    float azimuth = channeltoangleNormal(channel);
//    
    //change azimuth to be between 0 to 180 and 0 to -180
    if (azimuth >= 360.f){
        azimuth -= 360.f;
    }
    

    if (azimuth > 180.f){
        azimuth = azimuth - 360.f;
    }
    
    if(azimuth >= -180.f and azimuth < -90.f){
        float rightEarAngle = azimuth - 90.f + 360.f;
        return rightEarAngle;
    }
    
    return azimuth - 90.f;
    
}


//Input is angleInDegrees in Azimuth form
size_t FDN::angleToChannel(float angleInDegrees){
    
    //Make it between 0 to 360
    if (fabs(angleInDegrees - 0.0f)<0.0001){
        angleInDegrees = 0.0f;
    }
    
    if (angleInDegrees < 0.0f ){
        angleInDegrees = 360.f + angleInDegrees;
    }
    float channelWidth = 360.0f / CHANNELS;
    
    //Channel 0 to 7 from 12 o'clock, clockwise according from DFX
    size_t channel =(size_t)floorf(angleInDegrees / channelWidth);
//    printf("From angle: %f to channel %zu \n", angleInDegrees, channel);
    return channel;
}


//azimuth is clockwise 0 to 180 degree right, 0 to -180 left, p. 149 DAFX
size_t FDN::determineChannel(float x,float y, float orientation){
    float xL = parametersFDN.listenerLoc.x;
    float yL = parametersFDN.listenerLoc.y;
    
    float xDistance = x - xL;
    float yDistance = y - yL;
    
    //Special cases
    if (fabs(yDistance - 0.00001) < 0.0f){
        // zero y, can be at 90 or -90
        if (xDistance >= 0){
            return angleToChannel(90.f);
        }
        else{
            return angleToChannel(-90.f);
        }
    }
    
    if (fabs(xDistance - 0.00001) < 0.0f){
        //zero x, can be at 0 or 180;
        if (yDistance >= 0){
            return angleToChannel(0.f);
        }
        else{
            return angleToChannel(180.f);
        }
    }
    
    //Normal case
    //x over y because we want azimuth
    float angle2 = atan2f(xDistance, yDistance) * 180.0f / M_PI;

    //printf("Angle %f \n", angle2);

    return angleToChannel(angle2);
    
}


///************************************************////
///*******Energy Balancing FDN Delay*****************////
///************************************************////

void FDN::setDelayNoOutputLength(){
    
    //Set delay length for delay-without-output
    std::random_device rd;     // only used once to initialise (seed) engine
    std::mt19937 rng(10);    // random-number engine used (Mersenne-Twister in this case)
    std::uniform_int_distribution<int> uni(50, parametersFDN.roomCeiling/SOUNDSPEED * SAMPLE_RATE_F); // guaranteed unbiased
    
    
    
    for (int i = 0; i<SMOOTHDELAY; i++){
    int random_integer = uni(rng);
        printf("Randomdelay length: %d \n", random_integer);
    int delayTimeNoOutput = random_integer;
    delayTimes[TOTALDELAYS-SMOOTHDELAY+i] = delayTimeNoOutput;
    }

    
}

float FDN::gainEyring(int delayTime){
    
    float power = -1.0f * SOUNDSPEED * delayTime/SAMPLE_RATE_F * 0.0405 / parametersFDN.RT60;
    
    return expf(power);
    
}

///************************************************////
///*******Spherical Projection Grouping Extra Delay (5B)*********////
///************************************************////
void FDN::createNoInputNoOutputDelayLines(int current_delays, int remainder_delays){
    
    printf("begin setting leftover delay lines \n");
    
    assert(current_delays + remainder_delays == (TOTALDELAYS - SMOOTHDELAY));
    
    //Set delay length for delays without output and input tap
    std::random_device rd;     // only used once to initialise (seed) engine
    std::mt19937 rng(10);    // random-number engine used (Mersenne-Twister in this case)
    std::uniform_int_distribution<int> uni(0, 100); // guaranteed unbiased

    float mean_free_path = mean_spherical_length;
    
    for (int i = 0; i<remainder_delays; i++){
        int random_jitter = uni(rng);
//        printf("random_jitter %i \n", random_jitter);
        
        if (random_jitter % 2 == 0)
            delayTimes[current_delays + i] = floor(random_jitter + mean_free_path );
        else
            delayTimes[current_delays + i] = floor(mean_free_path - random_jitter);
        
//        if ( i%3 == 0 ){
//            if (random_jitter % 2 == 0)
//            delayTimes[current_delays + i] = random_jitter + parametersFDN.roomWidth / SOUNDSPEED * SAMPLE_RATE_F;
//            else
//                delayTimes[current_delays + i] = parametersFDN.roomWidth / SOUNDSPEED * SAMPLE_RATE_F - random_jitter;
//        }
//        else if (i%3 == 1){
//            if (random_jitter % 2 == 0)
//            delayTimes[current_delays + i] = random_jitter + parametersFDN.roomHeight / SOUNDSPEED * SAMPLE_RATE_F;
//            else
//                delayTimes[current_delays + i] = parametersFDN.roomHeight / SOUNDSPEED * SAMPLE_RATE_F - random_jitter;
//        }
//        else if (i%3 == 2){
//            if (random_jitter % 2 == 0)
//            delayTimes[current_delays + i] = random_jitter + parametersFDN.roomCeiling / SOUNDSPEED * SAMPLE_RATE_F;
//            else
//                delayTimes[current_delays + i] = parametersFDN.roomCeiling / SOUNDSPEED * SAMPLE_RATE_F - random_jitter;
//        }
        
//        printf("delay time %i is %i \n", i, delayTimes[current_delays + i]);
        assert(delayTimes[current_delays + i] > 0);
    }
    
}
