#ifndef SCALLOP_TK_STEMID_H_
#define SCALLOP_TK_STEMID_H_

//------------------------------------------------------------------------------
//                               Include Files
//------------------------------------------------------------------------------

//Standard C/C++
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <cmath>

//Opencv
#include <cv.h>
#include <cxcore.h>

//Scallop Includes
#include "ScallopTK/Utilities/Definitions.h"
#include "ScallopTK/Utilities/HelperFunctions.h"

//------------------------------------------------------------------------------
//                             Function Prototypes
//------------------------------------------------------------------------------

namespace ScallopTK
{

void stemDetection( Candidate *cd, IplImage* unused );

}

#endif
