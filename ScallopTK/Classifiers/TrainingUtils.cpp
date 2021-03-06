
#include "TrainingUtils.h"

namespace ScallopTK
{

void printCandidateInfo( int desig, Candidate *cd ) {

  // Print desig
  data_file << desig << " ";

  // Print Size features
  for( int i=0; i<SIZE_FEATURES; i++ ) {
    data_file << cd->sizeFeatures[i] << " ";
  }

  // Print color features
  for( int i=0; i<COLOR_FEATURES; i++ )
    data_file << cd->colorFeatures[i] << " ";

  // Print edge features
  for( int i=0; i<EDGE_FEATURES; i++ )
    data_file << cd->edgeFeatures[i] << " ";

  // Print HoG1
  CvMat* mat = cd->hogResults[0];
  for( int i=0; i<1764; i++ ) {
    float value = ((float*)(mat->data.ptr))[i];
    data_file << value << " " ;
  }

  // Print HoG2
  mat = cd->hogResults[1];
  for( int i=0; i<1764; i++ ) {
    float value = ((float*)(mat->data.ptr))[i];
    data_file << value << " " ;
  }

  // Print Gabor
  for( int i=0; i<GABOR_FEATURES; i++ )
    data_file << cd->gaborFeatures[i] << " ";

  // End line
  data_file << "\n";
}

bool initializeTrainingMode( const std::string& folder, const std::string& file ) {

  std::string data_output_fn = folder + file;

#ifdef SAVE_TRAINING_INSTRUCTIONS
  std::string inst_output_fn = folder + "instructions";
  instruction_file.open(inst_output_fn);
  if( !instruction_file.is_open() ) {
    cerr << "WHAT ARE YOU DOING?\n";
    return false;
  }
#endif

  data_file.open(data_output_fn.c_str());
  if( !data_file.is_open() ) {
    cerr << "WHAT ARE YOU DOING?\n";
    return false;
  }
  cvNamedWindow( "output", CV_WINDOW_AUTOSIZE );
  //cvNamedWindow( "output2", CV_WINDOW_AUTOSIZE );
  return true;
}

void exitTrainingMode() {
  cvDestroyWindow( "output" );
  //cvDestroyWindow( "output2" );
  
#ifdef SAVE_TRAINING_INSTRUCTIONS
  instruction_file.close();
#endif

  data_file.close();
}

bool getDesignationsFromUser( CandidatePtrVector& UnorderedCandidates,
  IplImage *display_img, IplImage *mask, int *Detections, float minRad,
  float maxRad, string img_name ) {
  
  // Variables                
  string input;
  std::cout << "\n";

  // Options
  bool skipsmall = false;
  bool skiplarge = false;
  bool skipcustom = false;
  bool allzero = false;
  float lowp, highp;

  // Cycle through all Candidates
  for( int i=0; i<UnorderedCandidates.size(); i++ ) {

    Candidate *cd = UnorderedCandidates[i];

    if( !cd->isActive )
      continue;

    //Skip if small
    if( skipsmall ) {
      float minsize = (maxRad - minRad)/2.7 + minRad;
      if( cd->major < minsize )
        continue;
    }
    if( skiplarge ) {
      float minsize = (maxRad - minRad)/1.5 + minRad;
      if( cd->major > minsize )
        continue;
    }
    if( skipcustom ) {
      float min = (maxRad-minRad)*lowp + minRad;
      float max = (maxRad-minRad)*highp + minRad;
      if( cd->major < min || cd->major > max )
        continue;
    }

    //Show interest point
    showIPNW( display_img, cd->colorQuadrants, cd );

    //Get User input
    std::cout << "INFO: " << i << " of " << UnorderedCandidates.size() << " ";
    std::cout << cd->method << " " << cd->magnitude << "\n";
    std::cout << "ENTER DESIGNATION: ";
    if( allzero )
      input = "0";
    else
      cin >> input;

    //Print intruction to file    
#ifdef SAVE_TRAINING_INSTRUCTIONS
    instruction_file << input << endl;
#endif

    //Check instruction for special cases
    if( input == "S" || input == "SKIP" ) 
      break;
    if( input == "EXIT" ) {
      trainingExitFlag = true;
      return false;
    }
    if( input == "SKIPSMALL" ) {
      skipsmall = true;
      continue;
    }
    if( input == "SKIPLARGE" ) {
      skiplarge = true;
      continue;
    }

    if( input == "SKIPCUSTOM" ) {
      std::cout << " LOWER: ";
      cin >> lowp;
      std::cout << " UPPER: ";
      cin >> highp;
      
#ifdef SAVE_TRAINING_INSTRUCTIONS
      instruction_file << lowp << endl;
      instruction_file << highp << endl;
#endif
      skipcustom = true;
      continue;
    }

    if( input == "SKIPN" ) {
      std::cout << " N: ";
      int N;
      cin >> N;
      i = i + N;

#ifdef SAVE_TRAINING_INSTRUCTIONS
      instruction_file << N << endl;
#endif
      continue;
    }

    if( input == "ALLZERO" ) {
      allzero = true;
      input = "0";
    }

    int in_num = atoi( input.c_str() );
#ifdef TRAINING_MODE
    cd->designation = in_num;
#endif

    //Print features to file
    printCandidateInfo( in_num, cd );

    //Update detect map
    if( in_num == 1 || in_num == 2 || in_num == 4 ) {
      Detections[SCALLOP_BROWN]++;
      updateMask( mask, cd->r, cd->c, cd->angle, cd->major, cd->minor, SCALLOP_BROWN );
    } else if( in_num == 8 ) {
      Detections[SCALLOP_WHITE]++;
      updateMask( mask, cd->r, cd->c, cd->angle, cd->major, cd->minor, SCALLOP_WHITE );
    } else if( in_num == 3 ) {
      Detections[SCALLOP_BROWN]++;
      updateMaskRing( mask, cd->r, cd->c, cd->angle, cd->major*0.63, cd->minor*0.63, cd->major, SCALLOP_BROWN );
    } else if( in_num == 10 ) {
      Detections[DOLLAR]++;
      updateMask( mask, cd->r, cd->c, cd->angle, cd->major, cd->minor, DOLLAR );
    }
  }

#ifdef SAVE_INTEREST_POINTS
  ofstream ip_out( ip_file_out.c_str(), ios::app );
  for( unsigned int i = 0; i < UnorderedCandidates.size(); i++ ) {
    if( !UnorderedCandidates[i]->isActive )
      continue;
    ip_out << img_name << " ";
    ip_out << UnorderedCandidates[i]->designation << " ";
    ip_out << UnorderedCandidates[i]->method << " ";
    ip_out << UnorderedCandidates[i]->magnitude << " ";
    ip_out << UnorderedCandidates[i]->r << " ";
    ip_out << UnorderedCandidates[i]->c << " ";
    ip_out << UnorderedCandidates[i]->major << " ";
    ip_out << UnorderedCandidates[i]->minor << " ";
    ip_out << UnorderedCandidates[i]->angle << " ";
    ip_out << endl;
  }
  ip_out.close();
#endif
  return true;
}

bool getDesignationsFromUser(CandidateQueue& OrderedCandidates,
  IplImage *display_img, IplImage *mask, int *Detections,
  float minRad, float maxRad, string img_name) {

  // Variables                
  string input;
  std::cout << "\n";

  // Options
  bool skipsmall = false;
  bool skiplarge = false;
  bool skipcustom = false;
  bool skiparea = false;
  bool skipmethod = false;
  bool allzero = false;
  bool skipcorner = false;
  float lowp, highp;
  int method;
  int lc, lr, uc, ur;

  // Cycle through all Candidates
  int cand_size = OrderedCandidates.size(); 
  for( int i=0; i<cand_size; i++ ) {

    Candidate *cd = OrderedCandidates.top();
    OrderedCandidates.pop();

    if( !cd->isActive )
      continue;

    //Skip if small
    if( skipsmall ) {
      float minsize = (maxRad - minRad)/2.7 + minRad;
      if( cd->major < minsize )
        continue;
    }
    if( skiplarge ) {
      float minsize = (maxRad - minRad)/1.5 + minRad;
      if( cd->major > minsize )
        continue;
    }
    if( skipcustom ) {
      float min = (maxRad-minRad)*lowp + minRad;
      float max = (maxRad-minRad)*highp + minRad;
      if( cd->major < min || cd->major > max )
        continue;
    }
    if( skipmethod ) {
      if( cd->method == method )
        continue;
    }
    if( skiparea ) {
      if( cd->r < lr || cd->r > ur )
        continue;
      if( cd->c < lc || cd->c > uc )
        continue;
    }
    if( skipcorner ) {
      if( cd->isCorner )
        continue;
    }

    //Show interest point
    showIPNW( display_img, cd );

#ifndef SCALLOP_TK_TRAINING_MODE_READ_IP_FROM_FILE
    //Get User input
    std::cout << "INFO: " << i << " of " << cand_size << " ";
    std::cout << cd->method << " " << cd->magnitude << "\n";
    std::cout << "ENTER CMD OR DESIGNATION: ";
    if( allzero )
      input = "0";
    else
      cin >> input;

    //Print intruction to file
#ifdef SAVE_TRAINING_INSTRUCTIONS
    instruction_file << input << endl;
#endif

    //Check instruction for special cases
    if( input == "SKIP" ) 
      break;
    if( input == "EXIT" ) {
      trainingExitFlag = true;
      return false;
    }
    if( input == "SKIPSMALL" ) {
      skipsmall = true;
      continue;
    }
    if( input == "SKIPLARGE" ) {
      skiplarge = true;
      continue;
    }

    if( input == "SKIPCUSTOM" ) {
      std::cout << " LOWER: ";
      cin >> lowp;
      std::cout << " UPPER: ";
      cin >> highp;
#ifdef SAVE_TRAINING_INSTRUCTIONS
      instruction_file << lowp << endl;
      instruction_file << highp << endl;
#endif
      skipcustom = true;
      continue;
    }

    if( input == "SKIPN" ) {
      std::cout << " N: ";
      int N;
      cin >> N;
      N = N - 1;
      for( int j = 0; j < N; j++ )
        OrderedCandidates.pop();
      i = i + N;
#ifdef SAVE_TRAINING_INSTRUCTIONS
      instruction_file << N << endl;
#endif
      continue;
    }

    if( input == "ALLZERO" ) {
      allzero = true;
      input = "0";
    }

    if( input == "SKIPAREA" ) {
      float inrat;
      std::cout << " LOWER_R_%: ";
      cin >> inrat;
      lr = mask->height * inrat;
#ifdef SAVE_TRAINING_INSTRUCTIONS
      instruction_file << inrat << endl;
#endif
      std::cout << " UPPER_R_%: ";
      cin >> inrat;
      ur = mask->height * inrat;
#ifdef SAVE_TRAINING_INSTRUCTIONS
      instruction_file << inrat << endl;
#endif
      std::cout << " LOWER_C_%: ";
      cin >> inrat;
      lc = mask->width * inrat;
#ifdef SAVE_TRAINING_INSTRUCTIONS
      instruction_file << inrat << endl;
#endif
      std::cout << " UPPER_C_%: ";
      cin >> inrat;
      uc = mask->width * inrat;
#ifdef SAVE_TRAINING_INSTRUCTIONS
      instruction_file << inrat << endl;
#endif
      skiparea = true;
      continue;
    }

    if( input == "SKIPMETHOD" ) {
      std::cout << " METHOD: ";
      cin >> method;
#ifdef SAVE_TRAINING_INSTRUCTIONS
      instruction_file << method << endl;
#endif
      skipmethod = true;
      continue;
    }

    if( input == "SKIPCORNER" ) {
      skipcorner = true;
      continue;
    }

    int in_num = atoi( input.c_str() );

#ifdef TRAINING_MODE
    cd->designation = in_num;
#endif

#else
    // (If IP read from file enabled, we already have stored desig)
    int in_num = cd->designation;
#endif

    //Print features to file
    printCandidateInfo( in_num, cd );

    //Update detect map
    if( in_num == 1 || in_num == 2 || in_num == 11 || in_num == 12 ) {
      Detections[SCALLOP_BROWN]++;
      updateMask( mask, cd->r, cd->c, cd->angle, cd->major, cd->minor, SCALLOP_BROWN );
    } else if( in_num == 31 || in_num == 32 ) {
      Detections[SCALLOP_WHITE]++;
      updateMask( mask, cd->r, cd->c, cd->angle, cd->major, cd->minor, SCALLOP_WHITE );
    } else if( in_num == 21 || in_num == 22 ) {
      Detections[SCALLOP_BROWN]++;
      updateMaskRing( mask, cd->r, cd->c, cd->angle, cd->major*0.75, cd->minor*0.75, cd->major, SCALLOP_BROWN );
    } else if( in_num == 51 || in_num == 52 || in_num == 61 || in_num == 62 ) {
      Detections[DOLLAR]++;
      updateMask( mask, cd->r, cd->c, cd->angle, cd->major, cd->minor, DOLLAR );
    }
#ifdef SAVE_INTEREST_POINTS
    ofstream ip_out( ip_file_out.c_str(), ios::app );

    ip_out << img_name << " ";
    ip_out << cd->designation << " ";
    ip_out << cd->method << " ";
    ip_out << cd->magnitude << " ";
    ip_out << cd->r << " ";
    ip_out << cd->c << " ";
    ip_out << cd->major << " ";
    ip_out << cd->minor << " ";
    ip_out << cd->angle << " ";
    ip_out << endl;

    ip_out.close();
#endif
  }
  return true;
}

void dumpCandidateFeatures( string file_name, CandidatePtrVector& cd )
{
  // Open stream
  ofstream ip_out( file_name.c_str(), ios::app );
  
  for( int c = 0; c < cd.size(); c++ ) 
  {
    // Check to make sure not inactive
    if( cd[c]->isActive == false )
      continue;
    
    // Print desig
    ip_out << cd[c]->classification << " ";
  
    // Print Size features
    for( int i=0; i<SIZE_FEATURES; i++ ) {
      ip_out << cd[c]->sizeFeatures[i] << " ";
    }
  
    // Print color features
    for( int i=0; i<COLOR_FEATURES; i++ )
      ip_out << cd[c]->colorFeatures[i] << " ";
  
    // Print edge features
    for( int i=0; i<EDGE_FEATURES; i++ )
      ip_out << cd[c]->edgeFeatures[i] << " ";
  
    // Print HoG1
    CvMat* mat = (cd[c])->hogResults[0];
    for( int i=0; i<1764; i++ ) {
      float value = ((float*)(mat->data.ptr))[i];
      ip_out << value << " " ;
    }
  
    // Print HoG2
    mat = cd[c]->hogResults[1];
    for( int i=0; i<1764; i++ ) {
      float value = ((float*)(mat->data.ptr))[i];
      ip_out << value << " " ;
    }
  
    // Print Gabor
    for( int i=0; i<GABOR_FEATURES; i++ )
      ip_out << cd[c]->gaborFeatures[i] << " ";
  
    // End line
    ip_out << "\n";
  }
  
  ip_out.close();
}

}
