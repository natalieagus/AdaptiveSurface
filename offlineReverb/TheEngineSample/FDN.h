/*

FDN: a feedback delay network reverberator

*/

#include "Cuboid.hpp"
#include "Gains.hpp"
#include "Vector3D.hpp"
#include "BMMultiTapDelay.h"
#include "BMBiquadArray.h"

#include "MultiLevelBiQuadFilter.h"

#pragma once


//#define DELAYUNITSSMALL 3
#define DELAYSPERUNIT 64
#define DELAYUNITSSTD 1
//#define UNCIRCULATEDTAPSSMALL (2*DELAYUNITSSMALL*DELAYSPERUNIT)
//#define UNCIRCULATEDTAPSSTD 2*DELAYUNITSSTD*DELAYSPERUNIT
//#define UNCIRCULATEDTAPSSTD 0
//#define EXTRADELAYS 0 //<= DELAYUNITTSTD + FLOORUNITS
//#define FLOORUNITS 0 //4 * FLOORUNITS >= FLOORDELAYS
//#define DELAYUNITSSTD (10 +EXTRADELAYS + FLOORUNITS)
//#define NUMDELAYSSTD  (DELAYUNITSSTD * DELAYSPERUNIT)
//#define FLOORDELAYS 0//PUT min 4, 9, 16, can try more 25, 36, 64 HERE, ensure FLOORDELAYS < 0.5*NUMDELAYSSTD
//#define SMOOTHINGDELAYS ((EXTRADELAYS * DELAYSPERUNIT) + (FLOORUNITS * DELAYSPERUNIT) - FLOORDELAYS)
//#define WALLDELAYS (NUMDELAYSSTD - SMOOTHINGDELAYS - FLOORDELAYS)
//#define TOTALDELAYS (WALLDELAYS + SMOOTHINGDELAYS + FLOORDELAYS)
#define TOTALDELAYS DELAYSPERUNIT * DELAYUNITSSTD
#define AUDIOCHANNELS 2
#define SAMPLINGRATEF 44100.0f
#define SOUNDSPEED 340.29f
#define D_PER_SIDE 2
#define NUMDELAYSSTD TOTALDELAYS
#define SMOOTHDELAY 0

#define RADIALTRACE (TOTALDELAYS - 2*D_PER_SIDE*D_PER_SIDE)

//#define CENTIMETRESTOMETRES 0.01f
//#define CENTIMETRESTOMETRESSQ CENTIMETRESTOMETRES*CENTIMETRESTOMETRES
#define CHANNELS 12

using namespace std;

#import <Accelerate/Accelerate.h>
#import "FirstOrderFilter.h"
#import "SingleTapDelay.h"
#import "Vector3D.hpp"
#import "Parameter.hpp"
//#import "energyTerm.hpp"


class FDN
{
public:
    
    float directPortionOn = 1.0f;
    float reverbPortionOn = 1.0f;
    int roomType;
    
    // constructor
	FDN(void);
//    FDN(bool powerSaveMode); // call this with powerSaveMode set to true for a more efficient reverb with lower quality sound.
    ~FDN();
    
    // processes a buffer with numFrames samples.  mono in, stereo out.
    void processIFretlessBuffer(float* input, size_t numFrames, float* outputL, float* outputR );

    void setParameter(Parameter params);
    bool parameterNeedsUpdate;

    void impulseResponse(long numSamples, std::ofstream* outputFileLeft, std::ofstream* outputFileRight);
    void processReverb(float* pInput, float* pOutputL, float* pOutputR);
    
protected:

    BMBiquadArray lowShelfFilter;
    BMBiquadArray highShelfFilter;
    
    float convertAzimuthToLeftEar(float azimuth);
    float convertAzimuthToRightEar(float azimuth);

    
    MultiLevelBiQuadFilter LowPassLeft;
    MultiLevelBiQuadFilter LowPassRight;
    
    MultiLevelBiQuadFilter BellLeft;
    MultiLevelBiQuadFilter BellRight;
    
    float listenerOrientation;
    
    double time_elapsed_msecs = 0.0f;
    
    float gainEyring(int delayTime);
    
    // forward declarations
    void BMFastHadamard16(const float* input, float* output, float* temp16);
    bool BMPowerOfTwoQ (size_t x);
    inline void BMFastHadamardTransform(float* input,
                                        float* output,
                                        float* temp1,
                                        float* temp2,
                                        size_t length);
    float temp1[TOTALDELAYS];
    float temp2[TOTALDELAYS];
    float firstOrderReflectionAttenuation[TOTALDELAYS-SMOOTHDELAY];
    
    float totalEnergyAfterAttenuation;
//    energyTerm BRDFenergy;
    
    BMMultiTapDelay multiTapDelay;
    
    //Setup multiTapDelay values
    size_t indicesL[TOTALDELAYS/2];
    size_t indicesR[0];
    float gainsL[TOTALDELAYS/2];
    float multiTapGains[TOTALDELAYS/2];
    float gainsR[0];
    Vector3D multiDelayLinePoints[TOTALDELAYS/2];
    size_t multiDelayLinePointsChannel[TOTALDELAYS/2];
    int multiTapDelayTapsNumber = 0;
    float multiTapOutputs[TOTALDELAYS/2];
    bool excessEnergy = false;
    
    void  setMultiTapDelayChannels();
    Cuboid Room;
    CuboidGroup RoomGroup;
    Gains GainValues;


    
    float directAttenuation;
    
    double time[30] = {0};
    int timeindex = 0;
    
    float inputGains[TOTALDELAYS];
    float outputGains[TOTALDELAYS];
    
    void configureRoomRayModel();
    
    Parameter parametersFDN;
//    Parameter newParametersFDN;
    void setParameterSafe(Parameter params);
    
    void setDelayNoOutputLength();
    
    float directDelayTimes[2]; //unit = FREQ * seconds
    Vector3D roomBouncePoints[TOTALDELAYS];

    size_t delayTimesChannel[TOTALDELAYS];
    
    void setTempPoints();
    Vector3D tempPoints[CHANNELS];
    void calculateAdditionalDelays();
    float additionalDelays[CHANNELS];
    SingleTapDelay reverbDelays[CHANNELS];
    void addReverbDelay(float* fdnLeft, float*fdnRight);
//    void setDelayTimes();

    
    float scaledNewInput[TOTALDELAYS];
    
    //To handle direct Rays
    SingleTapDelay directRays[2];
    void setDirectSingleTapDelay();
    void setDirectGains();
    void setDirectDelayTimes();
    void processDirectRaysDelay(float* input, float* directRaysOutput);
    
    //To do channel angle calculations
    void setDelayOutputChannels();
    void setDirectRayAngles();
    size_t determineChannel(float x, float y, float orientation);
    size_t angleToChannel(float angleInDegrees);
//    float channelToAngleHRTF(size_t channel);
    
    
    float channeltoangleNormal(size_t channel);
    //setting tankout of 8 channels
    void processTankOut(float fdnTankOut[CHANNELS]);

    //Binaural filters
    FirstOrderFilter leftEarFilter[CHANNELS];
    FirstOrderFilter rightEarFilter[CHANNELS];
    FirstOrderFilter directRayFilter[2];
    void setFilters();
    //Direct ray is inclusive in one of the channels, so there's no need to have another channel angle for this
    float directRayAngles[2];
    //process the fdntankout[channels]
    void filterChannelsHRTF(float fdnTankOut[CHANNELS], float directRay[2], float fdnTankOutLeft[CHANNELS], float fdnTankOutRight[CHANNELS]);
    
 
    // delay buffers
	//float delayBuffers [MAXBUFFERSIZE];
    float* delayBuffers;
    int delayTimes [TOTALDELAYS];
    float delayTimesSecond[TOTALDELAYS];
    int totalDelayTime;
    int numDelays, delayUnits;
	//float feedbackAttenuation[NUMDELAYSSTANDARD];
    //int numDelays, delayUnits, numIndices;

	// read / write indices
    // we put all the indices in one array, then access them via pointers.
    // Keeping all indices in one array allows us to increment all of them with a single
    // optomised vector-scalar addition.
    float* rwIndices[TOTALDELAYS];
    float* startIndices[TOTALDELAYS];
    float* endIndices[TOTALDELAYS];
    long samplesUntilNextWrap;
    float avgDelay;
    
    double gain(double rt60, double delayLengthInSamples);

    // variables for generating random numbers
	int  rands,randomSeed;

	// we keep buffer one sample of input in an array so that we can keep a pointer to each input for
	// fast summing of feedback output.  This avoids calculating the array index of the 
	// write pointer for every source.  We'll sum the inputs here and then copy only once from here
	// to the write pointer location.
	float inputs[TOTALDELAYS];
	float outputsPF[TOTALDELAYS];
    float outputsAF[TOTALDELAYS];
    float outputsAHSF[TOTALDELAYS];
    float outputsBinaural[TOTALDELAYS];
    
    float outTapSigns[TOTALDELAYS];
	float feedbackTapGains[TOTALDELAYS];

    
//    // temp variables for mixing feedback
    float ppxxV [DELAYUNITSSTD];
    float xxppV [DELAYUNITSSTD];
    float pnxxV [DELAYUNITSSTD];
    float xxpnV [DELAYUNITSSTD];
    
    // filter coefficients
    //
    // high-shelf filters for HF attenuation inside the tank
    float b0[NUMDELAYSSTD];
    float b1[NUMDELAYSSTD];
    float a1[NUMDELAYSSTD];
    float z1[NUMDELAYSSTD]; //DF2
    float t[NUMDELAYSSTD];
    float z1a[NUMDELAYSSTD]; //DF1
    float z1b[NUMDELAYSSTD]; //DF1
    float one_i = 1.f;
    float zero_f = 0.0f;
    
    float inputAttenuation;
	float matrixAttenuation;
    
    void updateRand();
	void resetTapAttenuation(float rt60);
	void resetReadIndices();
    void setHFDecayMultiplier(float fc, float hfMult, float rt60);
    void incrementIndices();

    void resetDelay(int totalDelayTime);

    void initialise(bool powerSaveMode);
    
    bool reverbOn;



    
};

double gain(double rt60, double delayLengthInSamples);
