//
//  Constants.h
//  ViolinModel
//
//  Created by Hans on 5/7/15.
//  Copyright (c) 2015 A Tasty Pixel. All rights reserved.
//

#ifndef ViolinModel_Constants_h
#define ViolinModel_Constants_h

#define SAMPLE_RATE_INT 44100
#define SAMPLE_RATE_D 44100.0
#define SAMPLE_RATE_F 44100.0f
#define MIN_SAMPLE_DIFFERENCE 0.0000305185 // smallest float representable by 16 bit audio
#define MAX_BUFFER_LENGTH 2048
#define MAX_DELAY_LINE SAMPLE_RATE_INT * 2 // 2 seconds of delay

#define DEBUG 1 // comment this out to disable assert statements

#include "mactypes.h"
#include <math.h>

//#define MIN(a,b) (((a)<(b))?(a):(b))
//#define MAX(a,b) (((a)>(b))?(a):(b))


#endif
