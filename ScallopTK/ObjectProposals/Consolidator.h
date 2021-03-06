#ifndef SCALLOP_TK_CONSOLIDATOR_H_
#define SCALLOP_TK_CONSOLIDATOR_H_

//------------------------------------------------------------------------------
//                               Include Files
//------------------------------------------------------------------------------

//Standard C/C++
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <math.h>
#include <fstream>
#include <queue>

//Opencv
#include <cv.h>
#include <cxcore.h>

//Scallop Includes
#include "ScallopTK/TPL/KDTree/kdtree.h"
#include "ScallopTK/Utilities/Definitions.h"
#include "ScallopTK/Utilities/HelperFunctions.h"
#include "ScallopTK/ObjectProposals/PriorStatistics.h"

namespace ScallopTK
{

//------------------------------------------------------------------------------
//                                Definitions
//------------------------------------------------------------------------------

class CompareCandidates {
public:
  bool operator() ( const Candidate* cd1, const Candidate* cd2 ) {
    if( cd1->method < cd2->method )
      return true;
    if( cd1->method == cd2->method && cd1->magnitude > cd2->magnitude )
      return true;
    return false;
  }
};

class CompareRank {
public:
  bool operator() ( const Candidate* cd1, const Candidate* cd2 ) {
    if( cd1->methodRank > cd2->methodRank )
      return true;
    return false;
  }
};

typedef priority_queue<Candidate*, CandidatePtrVector, CompareRank> CandidateQueue;

//------------------------------------------------------------------------------
//                             Function Prototypes
//------------------------------------------------------------------------------

void prioritizeCandidates( CandidatePtrVector& Blob, CandidatePtrVector& Adaptive,
  CandidatePtrVector& Template, CandidatePtrVector& Canny, CandidatePtrVector& Unordered,
  CandidateQueue& Ordered, ThreadStatistics *GS );
  
}

#endif
