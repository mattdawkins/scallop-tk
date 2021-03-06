//------------------------------------------------------------------------------
// Title: DoG.h
// Description: Difference of Gaussian Candidate Detection
//------------------------------------------------------------------------------

#ifndef SCALLOP_TK_DOG_H_
#define SCALLOP_TK_DOG_H_

//------------------------------------------------------------------------------
//                               Include Files
//------------------------------------------------------------------------------

//Standard C/C++
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>

//Opencv
#include <cv.h>
#include <cxcore.h>

//Scallop Includes
#include "ScallopTK/Utilities/Definitions.h"
#include "ScallopTK/Utilities/HelperFunctions.h"

//Visual Debugger
#ifdef ENABLE_VISUAL_DEBUGGER
  #include "ScallopTK/Utilities/VisualDebugger.h"
#endif

//------------------------------------------------------------------------------
//                               Configurations
//------------------------------------------------------------------------------

//Namespaces
using namespace std;
using namespace cv;

namespace ScallopTK
{

//------------------------------------------------------------------------------
//                                 Constants
//------------------------------------------------------------------------------

//Upscale the original image if our Scallop min size (pixels) is less than this
const float DOG_UPSCALE_THRESHOLD = 1.0f;

//Smoothing interval for sequential levels
const float DOG_SIGMA = 1.6f;

//Initial sigma of base level
const float DOG_INIT_SIGMA = 0.5f;

//Number of intervals per octave
const int DOG_INTERVALS_PER_OCT = 6;

//How many rows / colors near the image edge we should ignore
const int DOG_SCAN_START = 2;

//Max #steps to infer extrema
const int DOG_MAX_INTERP_STEPS = 5;

//Scale factor to compensate for size underestimation
const float DOG_COMPENSATION = 1.55f;

//Extrema Detection modes for DoG Detection
const int DOG_MIN = 0x00;
const int DOG_MAX = 0x01;
const int DOG_ALL = 0x02;

//------------------------------------------------------------------------------
//                             Function Prototypes
//------------------------------------------------------------------------------

bool findDoGCandidates( IplImage* input, CandidatePtrVector& kps,
  float minRad, float maxRad, int mode = DOG_ALL );

//------------------------------------------------------------------------------
//                             Required Structures
//------------------------------------------------------------------------------

struct DoG_Candidate {
  int r, c;
  int octv, intvl;
  float x, y;
  float subintvl;
};

}

#endif
