//------------------------------------------------------------------------------
// Title: definitions.h
// Author: Matthew Dawkins
// Description: This file contains declarations and constants used across 
// many operations
//------------------------------------------------------------------------------

#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

// C++ Includes
#include <string>
#include <vector>
#include <map>

// OpenCV Includes
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

//------------------------------------------------------------------------------
//                           Compile/Build Parameters
//------------------------------------------------------------------------------

// Define if compiling on a windows based platform
//  - this should already be defined by your compiler so it doesnt matter -
//#define WIN32 1

// Enables pthread support (linux only)
//#define USE_PTHREADS 1

// Benchmarks main function while processing (single thread only)
//#define BENCHMARK 1

// Enables automatic reading of metadata from image file
//  - For any HabCam data this should be on -
#define AUTO_READ_METADATA 1

//------------------------------------------------------------------------------
//                                 Constants
//------------------------------------------------------------------------------

// Max search depth for reading metadata in JPEG files
const int MAX_DEPTH_META = 10000;

// Mathematical Constants
const float PI = 3.14159265f;
const float INF = 1E+37f;

// Scallop Size Variables
// (Pre-adjusted for given tolerances in size calc stage)
const float minRadius = 0.0145f; //in meters
const float maxRadius = 0.1298f; //in meters

// Input image type definitions
const int UNKNOWN = 0x00; //.xxx
const int JPEG = 0x01; //.jpg || .JPG
const int RAW_TIF = 0x02; //.tif || .TIF
const int RAW_TIFF = 0x03; //.tiff
const int BMP = 0x04; //.bmp
const int PNG = 0x05; //.png

// Image Resize factors for different stages of our algorithm
// Controls the resolution of our detectors and feature extrc.
//   [units = meters / pixel]
const float MAX_PIXEL_SIZE_COLOR_CLASS = 0.0026f;
const float MAX_PIXEL_SIZE_COLOR_DOG = 0.0052f;
const float MAX_PIXEL_SIZE_TEMPLATE = MAX_PIXEL_SIZE_COLOR_CLASS;
const float MAX_PIXEL_SIZE_ADAPTIVE = MAX_PIXEL_SIZE_COLOR_CLASS;
const float MAX_PIXEL_SIZE_WATERSHED = MAX_PIXEL_SIZE_COLOR_CLASS;
const float MAX_PIXEL_SIZE_CLUST = MAX_PIXEL_SIZE_COLOR_CLASS;
const float MAX_PIXEL_SIZE_TEXTONS = MAX_PIXEL_SIZE_COLOR_CLASS;
const float MAX_PIXEL_SIZE_HOG = MAX_PIXEL_SIZE_COLOR_CLASS;

// Resize factos w.r.t. scallop radius per pixel
// ie, how many pixels we should resize the minRadius to for each stage
//  [units = # of pixels / min scanning radius ]
const float MIN_RAD_COLOR_CLASS = minRadius / MAX_PIXEL_SIZE_COLOR_CLASS;
const float MIN_RAD_COLOR_DOG = minRadius / MAX_PIXEL_SIZE_COLOR_DOG;
const float MIN_RAD_TEMPLATE = minRadius / MAX_PIXEL_SIZE_TEMPLATE;
const float MIN_RAD_ADAPTIVE = minRadius / MAX_PIXEL_SIZE_ADAPTIVE;
const float MIN_RAD_WATERSHED = minRadius / MAX_PIXEL_SIZE_WATERSHED;
const float MIN_RAD_CLUST = minRadius / MAX_PIXEL_SIZE_CLUST;
const float MIN_RAD_TEXTONS = minRadius / MAX_PIXEL_SIZE_TEXTONS;
const float MIN_RAD_HOG = minRadius / MAX_PIXEL_SIZE_HOG;

// Minimum template radius to scan for
const float minTemplateRadius = 0.020f; //in meters

// A downsizing image resize factor must be lower than 95%
// to validate and actually perform the resize (otherwise
// it doesn't pay)
const float RESIZE_FACTOR_REQUIRED = 0.95f;

// Histogram Update Propterties for Fast Read/Merge
// -Approximate # of pixels to scan
const float MAX_ESTIM_TO_SCAN = 60000;
// -Estimated ratio in # of environment/object pixels
const float OBJ_ENVI_RATIO = 0.1f;
// -Default # of pixels to skip after hitting an env pixel
const int DEFAULT_ENVI_SKIP = 5;
// -Default # of pixels to skip after hitting an obj pixel
const int DEFAULT_OBJ_SKIP = 2;
// -Default merge ratio for syncing old w/ new histogram
const float DEFAULT_MERGE_RATIO = 0.08f;

// Properties for gradient calculations
const float LAB_GRAD_SIGMA = 1.35f;
const float ENV_GRAD_SIGMA = 1.85f;

// Special type definitions
const std::string BROWN_SCALLOP = "BROWN";
const std::string WHITE_SCALLOP = "WHITE";
const std::string BURIED_SCALLOP = "BURIED";
const std::string ALL_SCALLOP = "ALL";
const std::string SAND_DOLLAR = "DOLLARS";

// Suppression type definitions for config file
const std::string WORLD_VS_OBJ_STR = "WVO";
const std::string DESIRED_VS_OBJ_STR = "OVO";
const std::string MIXED_CLASS_STR = "MIX";

// Classifier types
const unsigned int MAIN_CLASS = 0x00;
const unsigned int WORLD_VS_OBJ = 0x01;
const unsigned int DESIRED_VS_OBJ = 0x02;
const unsigned int MIXED_CLASS = 0x03;

// Internal tag designations for objects in mask
//  - Why isn't this an enum? -
typedef unsigned int tag;
const tag UNCLASSIFIED = 0x00;
const tag ENVIRONMENT = 0x01;
const tag SCALLOP_WHITE = 0x02;
const tag SCALLOP_BROWN = 0x03;
const tag SCALLOP_BURIED = 0x04;
const tag DOLLAR = 0x05;
const tag CLAM = 0x06;
const tag URCHIN = 0x07;
const tag SAC = 0x08;
const tag ROCK = 0x09;
const tag OTHER = 0x10;
const tag TOTAL_DESIG = 0x11;

// Detection methods for prioritzation
const unsigned int TEMPLATE = 0;
const unsigned int DOG = 1;
const unsigned int ADAPTIVE = 2;
const unsigned int CANNY = 3;
const unsigned int MULTIPLE1 = 4;
const unsigned int MULTIPLE2 = 5;
const unsigned int MULTIPLE3 = 6;
const unsigned int TOTAL_DM = 7;

// Feature constants
const unsigned int COLOR_BINS = 32;
const unsigned int COLOR_FEATURES = 122;
const unsigned int GABOR_FEATURES = 36;
const unsigned int SIZE_FEATURES = 9;
const unsigned int EDGE_FEATURES = 137;
const unsigned int NUM_HOG = 2;

// Sand Dollar Suppression Sys value
const double SDSS_DIFFERENCE_FACTOR = 2.0;

// Maximum number of classifiers in a single classification system
const int MAX_CLASSIFIERS = 30;

//------------------------------------------------------------------------------
//                      System Parameters - Set at Run-Time
//------------------------------------------------------------------------------

struct SystemSettings
{
  // Input Directory
  std::string InputDirectory;
  
  // Input Filename if we're processing a list in the above folder
  std::string InputFilename;
  
  // Output Directory
  std::string OutputDirectory;
  
  // Output Filename for list or training data in the above folder
  std::string OutputFilename;
  
  // Root directory for color filters
  std::string RootColorDIR;
  
  // Root directory for classifiers
  std::string RootClassifierDIR;
  
  // Is metadata stored in the image or the file list?
  bool IsMetadataInImage;

  // Are we in training mode or process mode?
  bool IsTrainingMode;
  
  // If we are in training mode, simple-gui or mips?
  bool UseMIPInput;

  // If training from MIPs files, the percentage of false points outside
  // of annotations to report
  float MIPTrainingPercentKeep;

  // Should we look at border points
  bool LookAtBorderPoints;

  // Is the input a directory or a list
  bool IsInputDirectory;

  // If the input is a directory, the classifier system to use
  std::string ClassifierToUse;
  
  // Enable output display?
  bool EnableOutputDisplay;
  
  // Output detection list?
  bool OutputList;
  
  // If an IP falls into more than one category, output it multiple times?
  bool OutputDuplicateClass;
  
  // Output images with detections in output directory?
  bool OutputDetectionImages;

  // Camera focal length
  float FocalLength;

  // Number of worker threads to allocate
  int NumThreads;
};


//------------------------------------------------------------------------------
//           Classifier Parameters - As Read From some Config File
//------------------------------------------------------------------------------

struct ClassifierConfigParameters
{
  // The subdirectory for all classifier files within the root folder
  std::string ClassifierSubdir;
  
  // Information for the primary classifiers
  std::vector< std::string > L1Keys;
  std::vector< std::string > L1Files;
  std::vector< std::string > L1SpecTypes;
  
  // Information for the secondary classifiers
  std::vector< std::string > L2Keys;
  std::vector< std::string > L2Files;
  std::vector< std::string > L2SpecTypes;
  std::vector< std::string > L2SuppTypes;
  
  // Is sand dollar suppression turned on?
  bool EnabledSDSS;

  // Classifier threshold
  double Threshold;
};


//------------------------------------------------------------------------------
//              Required Parameters read from some MIP file
//------------------------------------------------------------------------------

struct MIPParameters
{
  // Image name where object is
  std::string Name;
  
  // ID code of object
  int ID;
  
  // X1, Y1
  double X1, Y1;
  
  // X2, Y2
  double X2, Y2;
};

typedef std::vector< MIPParameters > MIPParameterVector;

//------------------------------------------------------------------------------
//                         Simple Contour Definition
//------------------------------------------------------------------------------

struct scanPoint {
  scanPoint( const int& _r, const int& _c ) : r(_r), c(_c) {}
  int r, c;
};

struct contour {
  float mag; // Contour Magnitude
  int label; // Contour Label in Binary Image
  bool covers_oct[8]; // Contour octant coverage around an IP center
  std::vector<scanPoint> pts; // Vector of points comprising contour
};

//------------------------------------------------------------------------------
//                          Interest Point Definition
//------------------------------------------------------------------------------

// Candidate Point
// Stores location, stats for classification, and classification results         
struct candidate
{

  // Initial ellipse Location
  // (Detected with low-resolution IP detectors)
  double r;
  double c;
  double major;
  double minor;
  double angle;

  // Revised ellipse location
  // (Calculted from edge feature cost function)
  double nr;
  double nc;
  double nmajor;
  double nminor;
  double nangle;

  // IP Detection Method and Magnitude of said Method
  unsigned int method;
  double magnitude;
  unsigned int method_rank;

  // Is the candidate a corner entry
  bool is_corner; //is the candidate on an image boundary
  bool is_side_border[8]; // which octants are outside the image

  // Features for classification  
  bool is_active;
  double ColorFeatures[COLOR_FEATURES];
  double GaborFeatures[GABOR_FEATURES];
  double SizeFeatures[SIZE_FEATURES];
  CvMat *HoGResult[NUM_HOG];
  double major_meters;

  // Used for color detectors
  IplImage *SummaryImage;
  IplImage *ColorQuadrants;
  int ColorQR, ColorQC;
  int ColorBinCount[COLOR_BINS];

  // Edge Based Features
  bool has_edge_features;
  double EdgeFeatures[EDGE_FEATURES];

  // Expensive edge search results
  float innerColorAvg[3];
  float outerColorAvg[3];
  contour *best_cntr;
  contour *full_cntr;

  // User entered designation, candidate filename if in training mode
  int designation;
  std::string filename;

  // Stats for final classification
  unsigned int classification;
  double class_magnitudes[MAX_CLASSIFIERS];
};

// An actual detection according to our algorithm, used just for post processing
struct detection
{
  // Image name
  std::string img;
  
  // Object Location
  double r;
  double c;
  double major;
  double minor;
  double angle;

  // Object Contour (if it exists)
  contour *cntr;

  // Possible Object IDs and classification detection values
  std::vector< std::string > IDs;
  std::vector< double > ClassificationValues;
  
  // Does the best match fall into any of these categories?
  bool IsBrownScallop;
  bool IsWhiteScallop;
  bool IsBuriedScallop;
  bool IsDollar;
};

#endif