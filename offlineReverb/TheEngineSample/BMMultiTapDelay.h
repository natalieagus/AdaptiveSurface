
#include <Accelerate/Accelerate.h>

#ifdef __cplusplus
extern "C" {

#endif
    
#ifndef BMMultiTapDelay_h
#define BMMultiTapDelay_h
    
//#include <Accelerate/Accelerate.h>
#include <stdio.h>
#include "TPCircularBuffer.h"
    
#endif /* BMMultiTapDelay_h */
    
    typedef struct BMMultiTapDelay {
        TPCircularBuffer cBufferL, cBufferR;
        float *bufferL, *bufferR;
        float *startL, *startR, *endL, *endR;
        float *readPtrL, *readPtrR, *writePtrL, *writePtrR;
        float *collectionBuffer;
        float *gainsL, *gainsR;
        size_t *indicesL, *indicesR;
        size_t numTapsL, numTapsR, bufferLengthSamples, maxDelayTime;
        bool stereo;
    } BMMultiTapDelay;
    
    
    
    /*!
     * Initialise the multi-tap delay
     *
     * @param This      a pointer to an uninitialised struct
     * @param stereo    is it stereo (true) or mono (false). Mono uses Left only
     * @param indicesL  array of delay tap times in samples, left channel / mono
     * @param indicesR  array of delay tap times in samples, right channel
     * @param gainsL    array of output tap gains for left channel or mono
     * @param gainsR    array of output tap gains for right channel
     * @param numTapsL  length of gainsL and indicesL
     * @param numTapsR  length of gainsR and indicesR
     */
    void BMMultiTapDelay_init(BMMultiTapDelay* This,
                              bool stereo,
                              size_t* indicesL,
                              size_t* indicesR,
                              float* gainsL,
                              float* gainsR,
                              size_t numTapsL,
                              size_t numTapsR);
    
    
    
    
    
    /*!
     * Free the memory used by a multi-tap delay struct
     *
     * @param This      a pointer to an initialised struct
     */
    void BMMultiTapDelay_destroy(BMMultiTapDelay* This);
    
    
    
    
    /*!
     * Process one sample of stereo input and output
     */
    static __inline__ __attribute__((always_inline)) void
    BMMultiTapDelay_processStereo(BMMultiTapDelay* This,
                                  float inL, float inR,
                                  float* outL, float* outR){
        /*
         * read to left channel output
         */
        // read left channel into collection buffer
        vDSP_vgathr(This->readPtrL,
                    This->indicesL, 1,
                    This->collectionBuffer, 1,
                    This->numTapsL);
        // apply the gain and sum to output
        vDSP_dotpr(This->collectionBuffer, 1,
                   This->gainsL, 1,
                   outL,
                   This->numTapsL);
        
        /*
         * read to right channel output
         */
        // read right channel into collection buffer
        vDSP_vgathr(This->readPtrR,
                    This->indicesR, 1,
                    This->collectionBuffer, 1,
                    This->numTapsR);
        // apply the gain and sum to output
        vDSP_dotpr(This->collectionBuffer, 1,
                   This->gainsR, 1,
                   outL,
                   This->numTapsR);
        
        // write
        *(This->writePtrL) = inL;
        *(This->writePtrR) = inR;
        
        // increment
        This->writePtrL++;
        This->writePtrR++;
        This->readPtrL++;
        This->readPtrR++;
        
        /*
         * wrap if necesary
         * Note that we only test one condition and then wrap everything.
         * This is possible because we are using circular buffers, which
         * allow us to write and read off the end of the actual memory
         * space up to a full extra buffer length.
         */
        if(This->readPtrL >= This->endL){
            This->readPtrL  = This->startL;
            This->readPtrR  = This->startR;
            This->writePtrL = This->startL + This->maxDelayTime;
            This->writePtrR = This->startR + This->maxDelayTime;
        }
    }
    
    
    
    
    
    /*!
     * Process one sample of mono input and output
     */
    static __inline__ __attribute__((always_inline)) void
    BMMultiTapDelay_processMono(BMMultiTapDelay* This,
                                float inL,
                                float* outL){
        /*
         * read to left channel output
         */
        // read left channel into collection buffer
        vDSP_vgathr(This->readPtrL,
                    This->indicesL, 1,
                    This->collectionBuffer, 1,
                    This->numTapsL);
        // apply the gain and sum to output
        vDSP_dotpr(This->collectionBuffer, 1,
                   This->gainsL, 1,
                   outL,
                   This->numTapsL);
        
        // write
        *(This->writePtrL) = inL;
        
        // increment
        This->writePtrL++;
        This->readPtrL++;
        
        
        /*
         * wrap if necesary
         *
         * note that we only test one condition and then wrap everything.
         * This is possible because we are using circular buffers, which
         * allow us to write and read off the end of the actual memory
         * space up to a full extra buffer length.
         */
        if(This->readPtrL >= This->endL){
            This->readPtrL  = This->startL;
            This->writePtrL = This->startL + This->maxDelayTime;
        }
    }
    
    /*!
     * Process one sample of mono input
     * store output into output, one element per tap
     *
     * @param input one sample of input
     * @param output array of length >= numTaps
     */
    static __inline__ __attribute__((always_inline)) void
    BMMultiTapDelay_processMultiChannelOut(BMMultiTapDelay* This,
                                           float input,
                                           float* output){
        /*
         * read to left channel output
         */
        // read left channel into collection buffer
        for(size_t i=0; i<This->numTapsL; i++){
            // read
            output[i] = This->readPtrL[This->indicesL[i]];
            // apply gain
            output[i] *= This->gainsL[i];
        }
        
        
        // write
        *(This->writePtrL) = input;
        
        // increment
        This->writePtrL++;
        This->readPtrL++;
        
        
        /*
         * wrap if necesary
         *
         * note that we only test one condition and then wrap everything.
         * This is possible because we are using circular buffers, which
         * allow us to write and read off the end of the actual memory
         * space up to a full extra buffer length.
         */
        if(This->readPtrL >= This->endL){
            This->readPtrL  = This->startL;
            This->writePtrL = This->startL + This->maxDelayTime;
        }
    }
    
#ifdef __cplusplus
}
#endif