//------------------------------------------------------------------------------
// Title: Scallop Detector
// Author: Matthew Dawkins
// Description: Run the core detection pipeline in an executable
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                               Include Files
//------------------------------------------------------------------------------

// Standard C/C++
#include <iostream>
#include <string>

// Scallop Includes
#include "ScallopTK/Pipelines/CoreDetector.h"
#include "ScallopTK/Utilities/ConfigParsing.h"
#include "ScallopTK/Utilities/Threads.h"

//------------------------------------------------------------------------------
//                               Configurations
//------------------------------------------------------------------------------

// Namespaces
using namespace std;
using namespace ScallopTK;

//------------------------------------------------------------------------------
//                                Main Function
//------------------------------------------------------------------------------

int main( int argc, char** argv )
{
  // Variables as defined in definitions.h
  SystemParameters settings;
  string mode, input, output, config, classifier;

  // Output Welcome Message
  // ASCII art from:
  // http://www.chris.com/ascii/index.php?art=sports%20and%20activities%2Fscuba
  cout << endl;
  cout << "~~~~~~~~~~~SCALLOP DETECTOR~~~~~~~~~~~~" << endl;
  cout << endl;
  cout << "           _.-''|''-._ " << endl;
  cout << "        .-'     |     `-.       __\\" << endl;
  cout << "      .'\\       |       /`. _  (..)\\" << endl;
  cout << "    .'   \\      |      /   `.\\ \\ ,_/ " << endl;
  cout << "    \\     \\     |     /     / \\ )(_ " << endl;
  cout << "     `\\    \\    |    /    /'   `|| \\" << endl;
  cout << "       `\\   \\   |   /   /'      || /" << endl;
  cout << "         `\\  \\  |  /  /'        |\\ `" << endl;
  cout << "        _.-`\\ \\ | / /'-._       | \\" << endl;
  cout << "       {_____`\\\\|//'_____}  ____| /" << endl;
  cout << "               `-'          \\_.-' \\`." << endl;
  cout << "                                   ~~~ " << endl;
  cout << " Software Author: MDD " << endl;
  cout << " ASCII Art Author: JGS " << endl;
  cout << endl;

  // If we're running via standard command line usage...
  if( argc == 4 || argc == 5 ) {

    mode = argv[1];
    input = argv[2];
    output = argv[3];

    if( !ParseSystemConfig( settings ) )
    {
      return false;
    }
    if( !ParseModeConfig( mode, settings ) )
    {
      return false;
    }
    if( settings.IsInputDirectory && !(settings.IsTrainingMode && settings.UseFileForTraining) )
    {
      settings.InputDirectory = input;
      settings.InputFilename = "";

      if( input.size() > 0 && input[ input.size()-1 ] != '\\' && input[ input.size()-1 ] != '/' )
      {
        settings.InputDirectory = settings.InputDirectory + "/";
      }
      SplitPathAndFile( output, settings.OutputDirectory, settings.OutputFilename );
    }
    else
    {
      SplitPathAndFile( input, settings.InputDirectory, settings.InputFilename );
      SplitPathAndFile( output, settings.OutputDirectory, settings.OutputFilename );
    }

    if( argc == 5 )
    {
      settings.ClassifierToUse = argv[4];
    }
    else
    {
      settings.ClassifierToUse = "DEFAULT";
    }
  }
  // If we haven't specified any arguments...
  else if( argc == 1 ) 
  {
    cout << "Enter input foldername: ";
    cin >> input;
    cout << "Enter output list filename: ";
    cin >> output;
    cout << "Enter classifier to use (enter DEFAULT for default): ";
    cin >> classifier;

    if( !ParseSystemConfig( settings ) )
    {
      return false;
    }
    if( !ParseModeConfig( "PROCESS_DIR", settings ) )
    {
      return false;
    }
    
    settings.ClassifierToUse = classifier;

    SplitPathAndFile( output, settings.OutputDirectory, settings.OutputFilename );

    settings.InputDirectory = input;
    settings.InputFilename = "";
  }
  // If we specified too many arguments...
  else
  {
    cout << endl;
    cout << "Scallop Detector Usage: " << endl;
    cout << "./ScallopDetector [MODE] [INPUT_LIST/DIR] [OUTPUT_LIST/DATA]" << endl;
    cout << endl;
    return 0;
  }

  // Validate number of specified threads
  if( settings.NumThreads < 1 || settings.NumThreads > MAX_THREADS ) {
    cerr << "WARNING: Invalid number of threads. Defaulting to 1." << endl;
    settings.NumThreads = 1;
  }

#ifdef WIN32
  // If on windows, default to 1 thread
  if( settings.NumThreads != 1 ) {
    cerr << endl;
    cerr << "WARNING: Windows version does not support threading at this time. ";
    cerr << "WARNING: Defaulting to 1 thread." << endl;
    settings.NumThreads = 1;
  }
#endif

  // If in training mode...
  if( settings.IsTrainingMode ) 
  {
    cout << endl << "TRAINING MODE INITIALIZING" << endl;

    if( settings.NumThreads != 1 ) {
      cerr << endl;
      cerr << "WARNING: Threading serves no purpose in training mode. ";
      cerr << "WARNING: Defaulting to 1 thread." << endl;
      settings.NumThreads = 1;
    }
  }

  // Count scallops!
  int count = runCoreDetector( settings );

  // Output completion statement
  cout << endl << "Processing Complete." << endl;
  return true;
}
