#ifdef __cplusplus
extern "C" {
#endif
    
#include <Accelerate/Accelerate.h>
#include <string.h>
#include "BMMultiTapDelay.h"
    
    
    void BMMultiTapDelay_init(BMMultiTapDelay* This,
                              bool stereo,
                              size_t* delayTimesL,
                              size_t* delayTimesR,
                              float* gainsL,
                              float* gainsR,
                              size_t numTapsL,
                              size_t numTapsR)
    {
        printf("Initializing multi tap buffer...\n");
        
        /*
         * Before initialising, we must find the required length of the buffer
         */
        // get the max delay time
        size_t maxDelayTime = 0;
        printf("numTapsL : %zu \n", numTapsL);
        for(size_t i=0; i < numTapsL; i++){
//            printf("delayTimesL[i] %zu \n", delayTimesL[i]);
            if(delayTimesL[i] > maxDelayTime){
            
                maxDelayTime = delayTimesL[i];

            }
        }
        if(stereo)
            for(size_t i=0; i < numTapsR; i++)
                if(delayTimesR[i] > maxDelayTime)
                    maxDelayTime = delayTimesR[i];
        This->maxDelayTime = maxDelayTime;
        
        
        // allocate memory for the buffers
        TPCircularBufferInit(&This->cBufferL,
                             sizeof(float)*(int)maxDelayTime);
        if(stereo){
            // do a second one if we need two channels
            TPCircularBufferInit(&This->cBufferR,
                                 sizeof(float)*(int)maxDelayTime );
            // now left and right buffers should have the same length
            assert(This->cBufferL.length == This->cBufferR.length);
        }
        
        // clear the buffers
        memset(This->cBufferL.buffer, 0, This->cBufferL.length);
        if(stereo)memset(This->cBufferR.buffer, 0, This->cBufferR.length);
        
        // make sure the length of the buffer is right for storing floats
        assert(This->cBufferL.length % sizeof(float) == 0);
        
        // remember the length of the buffer
        This->bufferLengthSamples = This->cBufferL.length / sizeof(float);
        
        // set the float pointers to the memory inside the circular buffers
        This->bufferL = (float*)This->cBufferL.buffer;
        if(stereo)This->bufferR = (float*)This->cBufferR.buffer;
        
        // set the start and end pointers
        This->startL = This->bufferL;
        if(stereo) This->startR = This->bufferR;
        This->endL = This->startL + This->bufferLengthSamples;
        if(stereo) This->endR = This->startR + This->bufferLengthSamples;
        
        // allocate memory for the array indices (these are relative indices)
        This->indicesL = malloc(sizeof(size_t)*numTapsL);
        if(stereo)This->indicesR = malloc(sizeof(size_t)*numTapsR);
        
        // convert delay times to indices
        for(size_t i=0; i<numTapsL; i++)
            This->indicesL[i] = maxDelayTime - delayTimesL[i];
        if(stereo)
            for(size_t i=0; i<numTapsR; i++)
                This->indicesR[i] = maxDelayTime - delayTimesR[i];
        
        // set the read pointers
        // this is where the delay of max length will read. all others will read
        // at a later location indicated by the indices
        This->readPtrL = This->startL;
        if(stereo) This->readPtrR = This->startR;
        
        // set the write pointers
        This->writePtrL = This->startL + maxDelayTime;
        if(stereo) This->writePtrR = This->startR + maxDelayTime;
        
        // allocate memory for the out tap gains
        This->gainsL = malloc(sizeof(float)*numTapsL);
        if(stereo)This->gainsR = malloc(sizeof(float)*numTapsR);
        
        // copy the gains
        memcpy(This->gainsL,gainsL,sizeof(float)*numTapsL);
        if(stereo)memcpy(This->gainsR,gainsR,sizeof(float)*numTapsR);
        
        // allocate memory for a temp buffer for summing the output
        size_t maxLength = numTapsL > numTapsR ? numTapsL : numTapsR;
        This->collectionBuffer = malloc(sizeof(float)*maxLength);
        
        // remember if it's stereo
        This->stereo = stereo;
        
        // remember the number of taps in each channel
        This->numTapsL = numTapsL;
        This->numTapsR = numTapsR;
    }
    
    
    
    
    void BMMultiTapDelay_destroy(BMMultiTapDelay* This){
        // left channel
        TPCircularBufferCleanup(&This->cBufferL);
        free(This->indicesL);
        free(This->gainsL);
        
        // right channel
        if(This->stereo){
            TPCircularBufferCleanup(&This->cBufferR);
            free(This->indicesR);
            free(This->gainsR);
        }
        
        // collection buffer
        free(This->collectionBuffer);
    }
    
#ifdef __cplusplus
}
#endif