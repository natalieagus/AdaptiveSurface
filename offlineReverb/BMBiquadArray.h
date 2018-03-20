#ifndef BMBiquadArray_h
#define BMBiquadArray_h

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef struct BMBiquadArray {
        float *x1, *x2, *y1, *y2;
        float *a1, *a2, *b0, *b1, *b2;
        float sampleRate;
        size_t numChannels;
    } BMBiquadArray;
    
    
    
    
    
    /*
     * Initialize a filter array for use in controlling high frequency decay time in a feedback Delay network
     *
     * @param delayTimes          list of delay times in seconds for the FDN
     * @param fc                  cutoff frequency of the shelf filters (Hz)
     * @param unfilteredRT60      the decay time below fc
     * @param filteredRT60        the decay time above fc
     * @param numChannels         size of FDN, length of delayTimes
     * @param sampleRate          sample rate in Hz
     */
    void BMBiquadArray_FDNHighShelfInit(BMBiquadArray* This, float* delayTimes, float fc, float unfilteredRT60, float filteredRT60, size_t numChannels, float sampleRate);
    
    
    
    
    
    
    /*
     * Initialize a filter array for use in controlling low frequency decay time in a feedback Delay network
     *
     * @param delayTimes          list of delay times in seconds for the FDN
     * @param fc                  cutoff frequency of the shelf filters (Hz)
     * @param unfilteredRT60      the decay time above fc
     * @param filteredRT60        the decay time below fc
     * @param numChannels         size of FDN, length of delayTimes
     * @param sampleRate          sample rate in Hz
     */
    void BMBiquadArray_FDNLowShelfInit(BMBiquadArray* This, float* delayTimes, float fc, float unfilteredRT60, float filteredRT60, size_t numChannels, float sampleRate);
    
    
    
    
    
    /*
     * process one sample of audio on the input array of numChannels channels
     *
     * length of input == length of output == numChannels
     * (works in place)
     */
    void BMBiquadArray_processSample(BMBiquadArray* This, float* input, float* output, size_t numChannels);
    
    
    
    
    
    
    /*
     * Free memory and clear
     */
    void BMBiquadArray_free(BMBiquadArray* This);
    
    
    
    /*
     * Print the impulse response of the first filter in the array
     */
    void BMBiquadArray_impulseResponse(BMBiquadArray* This);
    
    
#ifdef __cplusplus
}
#endif

#endif /* BMBiquadArray_h */
