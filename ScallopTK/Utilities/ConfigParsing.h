
#ifndef _SC_CONFIG_PARSING_
#define _SC_CONFIG_PARSING_

// Helper Functions for Parsing Config Files (csv, ini)

// Std Lib Include Files
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdio.h>

// Internal Include Files
#include "../Config/SimpleIni.h"
#include "../Common/definitions.h"

//File system
#ifdef WIN32
  #include "Common/filesystem_win32.h"
#else
  #include "Common/filesystem_unix.h"
#endif

// Namespaces
using namespace std;

// Remove any spaces from either end of the string
inline std::string RemoveSpaces( const std::string& str )
{
  int str_size = str.size();

  if( !str_size || (str[0] != ' ' && str[ str.size()-1 ] != ' ') )
    return str;
  
  int pre_count = 0;
  int post_count = 0;
  
  for( int i = 0; i < str_size; i++ )
  {
    if( str[ i ] != ' ' )
      break;
    pre_count++;
  }

  if( pre_count == str_size )
    return "";
  
  for( int i = str_size-1; i >= 0; i-- )
  {
    if( str[ i ] != ' ' )
      break;
    post_count++;
  }
  
  return str.substr( pre_count, str_size - pre_count - post_count );
}


inline void SplitPathAndFile( const string& input, string& path, string& file )
{
  if( input.size() == 0 )
  {
    file = "";
    path = "";
    return;
  }
  
  for( int i = input.size() - 1; i >= 0; i-- )
  {
    if( input[i] == '\\' || input[i] == '/' )
    {
      file = input.substr( i + 1, input.size() - i - 1 );
      path = input.substr( 0, i + 1 );
      return;
    }
  }
  
  file = input;
  path = "";
}

// Convert a csv style string into a vector of substrings
inline std::vector< std::string > ConvertCSVLine( string line, bool ignore_empty = false )
{
  std::vector< std::string > parsed;
    stringstream strm(line);
    string word;

    while(getline(strm,word,','))
    {
    word = RemoveSpaces(word);
    if( ignore_empty && word.size() == 0 )
      continue;
        parsed.push_back(word);
    }
    return parsed;
}

// Parse the contents of a process config file
inline bool ParseModeConfig( string key, SystemSettings& params )
{
  // Find config directory
  const std::string EXEPATH = GetExectuablePath();
  string ROOT, TMP;
  SplitPathAndFile( EXEPATH, ROOT, TMP );
  const std::string DIR1 = ROOT + "/data/";
  const std::string DIR2 = ROOT + "/../data/";
  const std::string DIR1DEFAULT = ROOT + "/data/ConfigFiles/SYSTEM_SETTINGS";
  const std::string DIR2DEFAULT = ROOT +"/../data/ConfigFiles/SYSTEM_SETTINGS";
  string CONFIG_DIR = "";
  string DATA_DIR = "";

  if( !DoesFileExist( DIR1DEFAULT ) )
  {
    if( DoesFileExist( DIR2DEFAULT ) )
    {
      CONFIG_DIR = DIR2 + "ConfigFiles/";
      DATA_DIR = DIR2;
    }
  }
  else
  {
    CONFIG_DIR = DIR1 + "ConfigFiles/";
    DATA_DIR = DIR1;
  }

  // Calculate location of config file
  string filename = CONFIG_DIR + key;

  try
  {
    // Open file
    CSimpleIniA rdr;
    rdr.SetUnicode();
    SI_Error rc = rdr.LoadFile(filename.c_str());
    if( rc < 0 )
    {
      cout << "CRITICAL ERROR: Could not load system config file - " << key << endl;
      return false;
    }

    // Read file contents
    params.IsTrainingMode = !strcmp( rdr.GetValue("options", "training_mode", NULL), "true" );
  
    if( params.IsTrainingMode )
    {
      params.UseMIPInput = !strcmp( rdr.GetValue("options", "load_mip_file", NULL), "true" );
      params.IsInputDirectory = !strcmp( rdr.GetValue("options", "is_list", NULL), "false" );
      params.IsMetadataInImage = !strcmp( rdr.GetValue("options", "is_metadata_in_image", NULL), "true" );
      params.EnableOutputDisplay = false;
      params.OutputList = true;
      params.OutputDuplicateClass = false;
      params.OutputDetectionImages = false;
      params.NumThreads = 1;
    }
    else
    {
      params.UseMIPInput = false;
      params.IsInputDirectory = !strcmp( rdr.GetValue("options", "is_list", NULL), "false" );
      params.IsMetadataInImage = !strcmp( rdr.GetValue("options", "is_metadata_in_image", NULL), "true" );
      params.EnableOutputDisplay = !strcmp( rdr.GetValue("options", "enable_display", NULL), "true" );
      params.OutputList = !strcmp( rdr.GetValue("options", "output_list", NULL), "true" );
      params.OutputDuplicateClass = !strcmp( rdr.GetValue("options", "output_duplicate_classifications", NULL), "true" );
      params.OutputDetectionImages = !strcmp( rdr.GetValue("options", "output_images", NULL), "true" );
      params.NumThreads = atoi( rdr.GetValue("options", "num_threads", "1" ) );
    }
  }
  catch(...)
  {
    cout << "CRITICAL ERROR: Could not load system config file - " << key << endl;
    return false;
  }

  return true;
}

// Parse the contents of a process config file
inline bool ParseSystemConfig( SystemSettings& params )
{
  // Find config directory
  const std::string EXEPATH = GetExectuablePath();
  string ROOT, TMP;
  SplitPathAndFile( EXEPATH, ROOT, TMP );
  const std::string DIR1 = ROOT + "/data/";
  const std::string DIR2 = ROOT + "/../data/";
  const std::string DIR1DEFAULT = ROOT + "/data/ConfigFiles/SYSTEM_SETTINGS";
  const std::string DIR2DEFAULT = ROOT +"/../data/ConfigFiles/SYSTEM_SETTINGS";
  string CONFIG_DIR = "";
  string DATA_DIR = "";

  if( !DoesFileExist( DIR1DEFAULT ) )
  {
    if( DoesFileExist( DIR2DEFAULT ) )
    {
      CONFIG_DIR = DIR2 + "ConfigFiles/";
      DATA_DIR = DIR2;
    }
  }
  else
  {
    CONFIG_DIR = DIR1 + "ConfigFiles/";
    DATA_DIR = DIR1;
  }

  // Calculate location of config file
  string filename = CONFIG_DIR + "SYSTEM_SETTINGS";

  try
  {
    // Open file
    CSimpleIniA rdr;
    rdr.SetUnicode();
    SI_Error rc = rdr.LoadFile(filename.c_str());
    if( rc < 0 )
    {
      cout << "CRITICAL ERROR: Could not load system config file - SYSTEM_CONFIG" << endl;
      return false;
    }

    // Read file contents
    params.RootColorDIR = rdr.GetValue("options", "root_histogram_dir", NULL);
    params.RootClassifierDIR = rdr.GetValue("options", "root_classifier_dir", NULL);
    params.FocalLength = atof( rdr.GetValue("options", "focal_length", NULL) );
    params.MIPTrainingPercentKeep = atof( rdr.GetValue("options", "training_false_keep_percentage", NULL) );
    params.LookAtBorderPoints = !strcmp( rdr.GetValue("options", "look_at_border_points", NULL), "true" );

    if( params.RootClassifierDIR == "[DEFAULT]" )
    {
      params.RootClassifierDIR = DATA_DIR + "Classifiers/";
    }
    if( params.RootColorDIR == "[DEFAULT]" )
    {
      params.RootColorDIR = DATA_DIR + "ColorFilterBanks/";
    }
  }
  catch(...)
  {
    cout << "CRITICAL ERROR: Could not load system config file - SYSTEM_CONFIG" << endl;
    return false;
  }

  return true;
}

// Parse the contents of a classifier config file
inline bool ParseClassifierConfig( string key, const SystemSettings& settings, ClassifierConfigParameters& params )
{
  // Find config directory
  const std::string EXEPATH = GetExectuablePath();
  string ROOT, TMP;
  SplitPathAndFile( EXEPATH, ROOT, TMP );
  const std::string DIR1 = ROOT + "/data/ConfigFiles/";
  const std::string DIR2 = ROOT + "/../data/ConfigFiles/";
  const std::string DIR1DEFAULT = ROOT + "/data/ConfigFiles/" + key;
  const std::string DIR2DEFAULT = ROOT +"/../data/ConfigFiles/" + key;
  string CONFIG_DIR = "";

  if( !DoesFileExist( DIR1DEFAULT ) )
  {
    if( DoesFileExist( DIR2DEFAULT ) )
    {
      CONFIG_DIR = DIR2;
    }
  }
  else
  {
    CONFIG_DIR = DIR1;
  }

  // Calculate location of config file
  string filename = CONFIG_DIR + key;

  try
  {
    // Open file
    CSimpleIniA rdr;
    rdr.SetUnicode();
    SI_Error rc = rdr.LoadFile(filename.c_str());
    if( rc < 0 )
    {
      cout << endl << endl;
      cout << "CRITICAL ERROR: Could not load classifier config file - " << key << endl;
      return false;
    }

    // Read file contents
    params.ClassifierSubdir = rdr.GetValue("classifiers", "CLASSIFIER_SUBDIR", NULL);
    params.L1Keys = ConvertCSVLine( rdr.GetValue("classifiers", "C1IDS", NULL), true );
    params.L1Files = ConvertCSVLine( rdr.GetValue("classifiers", "C1FILES", NULL), true );
    params.L1SpecTypes = ConvertCSVLine( rdr.GetValue("classifiers", "C1CATEGORY", NULL), true );
    params.L2Keys = ConvertCSVLine( rdr.GetValue("classifiers", "C2IDS", NULL), true );
    params.L2SpecTypes = ConvertCSVLine( rdr.GetValue("classifiers", "C2CATEGORY", NULL), true );
    params.L2SuppTypes = ConvertCSVLine( rdr.GetValue("classifiers", "C2CLFSTYLE", NULL), true );
    params.L2Files = ConvertCSVLine( rdr.GetValue("classifiers", "C2FILES", NULL), true );
    params.EnabledSDSS = !strcmp( rdr.GetValue("classifiers", "ENABLE_SAND_DOLLAR_SUPPRESSION_SYS", NULL), "true" );
    params.Threshold = atof( rdr.GetValue("classifiers", "THRESHOLD", "0.0") );

    // Check vector sizes
    if( params.L1Keys.size() != params.L1Files.size() || 
      params.L1Keys.size() != params.L1SpecTypes.size() ||
      params.L2Files.size()!= params.L2SpecTypes.size() ||
        params.L2Keys.size() != params.L2SuppTypes.size() || 
        params.L2Keys.size() != params.L2Files.size() )
    {
      cout << "CRITICAL ERROR: Classifier lists in config file " << key << " are not the same length!" << endl;
      return false;
    }
  }
  catch(...)
  {
    cout << "CRITICAL ERROR: Could not load classifier config file - " << key << endl;
    return false;
  }
  
  // Append paths to files
  vector< string >& L1Files = params.L1Files;
  vector< string >& L2Files = params.L2Files;
  for( unsigned i = 0; i < L1Files.size(); i++ )
  {
    if( params.ClassifierSubdir != "." && params.ClassifierSubdir != "" && params.ClassifierSubdir != " " )
      L1Files[i] = settings.RootClassifierDIR + params.ClassifierSubdir + L1Files[i];
    else
      L1Files[i] = settings.RootClassifierDIR + L1Files[i];
  }
  for( unsigned i = 0; i < L2Files.size(); i++ )
  {
    if( params.ClassifierSubdir != "." && params.ClassifierSubdir != "" && params.ClassifierSubdir != " " )
      L2Files[i] = settings.RootClassifierDIR + params.ClassifierSubdir + L2Files[i];
    else
      L2Files[i] = settings.RootClassifierDIR + L2Files[i];
  } 

  return true;
}

// Parse the contents of a MIP input training file
inline bool ParseMIPFile( string filename, MIPParameterVector& output )
{
  // Clear vector
  output.clear();
  
  // Open file
  ifstream input( filename.c_str() );
  
  if( !input ) {
    cout << "ERROR: Unable to load MIP file!" << endl;
    return false;
  }
  
  // Loop through file line by line
  string line;
  while(getline(input, line))
  {
    // Ignore header
    if( line.substr(0,12) == "imagename,x1" )
      continue;
    
    // Parse line
    vector< string > words = ConvertCSVLine( line );
    
    // Confirm line length
    if( words.size() != 12 ) {
      cout << "WARNING: Invalid MIP line or end of file, ignoring" << endl;
      continue;
    }
    
    // Retrieve info from parsed line
    string filename, dir;
    SplitPathAndFile( words[0], dir, filename );
    MIPParameters MIP;
    MIP.Name = filename;
    MIP.X1 = atof( words[ 1 ].c_str() );
    MIP.Y1 = atof( words[ 2 ].c_str() );
    MIP.X2 = atof( words[ 3 ].c_str() );
    MIP.Y2 = atof( words[ 4 ].c_str() );
    MIP.ID = atoi( words[ 7 ].c_str() );
    output.push_back( MIP );
  }
  return true;
}

inline void InitializeDefault( SystemSettings& settings )
{
  settings.RootColorDIR = "data/ColorFilterBanks/";
  settings.RootClassifierDIR = "data/Classifiers/";
  settings.IsTrainingMode = false;
  settings.UseMIPInput = false;
  settings.IsInputDirectory = true;
  settings.EnableOutputDisplay = true;
  settings.OutputList = true;
  settings.OutputDuplicateClass = true;
  settings.OutputDetectionImages = false;
  settings.NumThreads = 1;
}


inline candidate* ConvertMIPToCandidate( MIPParameters& Pt, float DownsizeFactor )
{
  // Convert a MIP point to a candidate with some random flux
  candidate* output = new candidate;

  // Calculate random adjustment factors
  double RAND_R = ( ((double)rand()/(double)RAND_MAX) - 0.5 ) / 5;
  double RAND_C = ( ((double)rand()/(double)RAND_MAX) - 0.5 ) / 5;
  double RAND_ANGLE = 3.14159 * ((double)rand()/(double)RAND_MAX);
  double RAND_MAJOR = 1.0 + ( ((double)rand()/(double)RAND_MAX) - 0.5 ) / 5;
  double RAND_MINOR = 1.0 + ( ((double)rand()/(double)RAND_MAX) - 0.5 ) / 5;
  if( ((double)rand()/(double)RAND_MAX) < 0.3 )
    RAND_ANGLE = 0.0;

  // Calculate MIP location in native
  double R = ( Pt.Y1 + Pt.Y2 ) / 2.0;
  double C = ( Pt.X1 + Pt.X2 ) / 2.0;
  double x_sqr = Pt.X2 - Pt.X1;
  x_sqr = x_sqr * x_sqr;
  double y_sqr = Pt.Y2 - Pt.Y1;
  y_sqr = y_sqr * y_sqr;
  double DIST = sqrt( x_sqr + y_sqr );

  // Adjust location for rescale factor
  R *= DownsizeFactor;
  C *= DownsizeFactor;
  DIST *= DownsizeFactor;

  // Set candidate vals
  output->r = R + DIST * RAND_R;
  output->c = C + DIST * RAND_C;
  output->major = RAND_MAJOR * DIST / 2.0;
  output->minor = RAND_MINOR * DIST / 2.0;
  output->angle = RAND_ANGLE;

  // Set classification value
  output->classification = Pt.ID;

  return output;
}

// Move this function to somplace better
inline void RemoveOverlapAndMerge( vector<candidate*>& Base, vector<candidate*>& Truth, double percentage_keep = 0.10 )
{
  // Top down greedy search - very slow but who cares its for training only
  for( int j = Base.size() - 1; j >= 0; j-- )
  {
    // Determine if we should kill this candidate (too close to GT)
    bool RemoveCandidate = false;

    for( int i = 0; i < Truth.size(); i++ )
    {
      double MAJOR = ( Base[j]->major < Truth[i]->major ? Truth[i]->major : Base[j]->major );
      double COMP1 = Base[j]->r - Truth[i]->r;
      double COMP2 = Base[j]->c - Truth[i]->c;
      COMP1 *= COMP1;
      COMP2 *= COMP2;
      double DIST = sqrt( COMP1 + COMP2 );
      if( DIST < 1.8 * MAJOR )
      {
        RemoveCandidate = true;
        break;
      }
    }

    if( RemoveCandidate || ((double)rand()/(double)RAND_MAX) > percentage_keep )
    {
      delete Base[j];
      Base.erase(Base.begin()+j);
    }
    else
    {
      Base[j]->classification = 0;
    }
  }

  // Merge results
  for( int j = 0; j < Truth.size(); j++ )
  {
    Base.push_back( Truth[j] );
  }
}

#endif