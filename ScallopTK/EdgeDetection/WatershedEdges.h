#ifndef SCALLOP_TK_WATERSHED_EDGES_H_
#define SCALLOP_TK_WATERSHED_EDGES_H_

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
#include "ScallopTK/EdgeDetection/GaussianEdges.h"

//------------------------------------------------------------------------------
//                             Function Prototypes
//------------------------------------------------------------------------------

namespace ScallopTK
{

void extractWatershedEdge( IplImage *img, Candidate* kp );
void extractScallopContours( GradientChain& Gradients, CandidatePtrVector cds );

}

#endif
