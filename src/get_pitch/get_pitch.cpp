/// @file

#include <iostream>
#include <fstream>
#include <string.h>
#include <errno.h>

#include "wavfile_mono.h"
#include "pitch_analyzer.h"

#include "docopt.h"

#define FRAME_LEN 0.030   /* 30 ms. */
#define FRAME_SHIFT 0.015 /* 15 ms. */

using namespace std;
using namespace upc;

static const char USAGE[] = R"(
get_pitch - Pitch Detector 

Usage:
    get_pitch [options] <input-wav> <output-txt> [<th1>] [<th2>] [<thPot>]
    get_pitch (-h | --help)
    get_pitch --version

Options:
    -h, --help  Show this screen
    --version   Show the version of the project

Arguments:
    input-wav   Wave file with the audio signal
    output-txt  Output file: ASCII file with the result of the detection:
                    - One line per frame with the estimated f0
                    - If considered unvoiced, f0 must be set to f0 = 0
)";

int main(int argc, const char *argv[])
{
  /// \DONE
  ///  Modify the program syntax and the call to **docopt()** in order to
  ///  add options and arguments to the program.
  std::map<std::string, docopt::value> args = docopt::docopt(USAGE,
                                                             {argv + 1, argv + argc}, // array of arguments, without the program name
                                                             true,                    // show help if requested
                                                             "2.0");                  // version string

  std::string input_wav = args["<input-wav>"].asString();
  std::string output_txt = args["<output-txt>"].asString();

  int numArgs = argc - 3;
  float th1;
  float th2;
  float thPot;

  if (numArgs > 0)
  {
    th1 = stof(args["<th1>"].asString());
    numArgs--;
  }
  else
  {
    th1 = 0.7;
  }

  if (numArgs > 0)
  {
    th2 = stof(args["<th2>"].asString());
    numArgs--;
  }
  else
  {
    th2 = 0.3;
  }

  if (numArgs > 0)
  {
    thPot = -1.0 * stof(args["<thPot>"].asString());
    numArgs--;
  }
  else
  {
    thPot = -50.5;
  }

  // Read input sound file
  unsigned int rate;
  vector<float> x;
  if (readwav_mono(input_wav, rate, x) != 0)
  {
    cerr << "Error reading input file " << input_wav << " (" << strerror(errno) << ")\n";
    return -2;
  }

  int n_len = rate * FRAME_LEN;
  int n_shift = rate * FRAME_SHIFT;

  // Define analyzer
  PitchAnalyzer analyzer(n_len, rate, th1, th2, thPot, PitchAnalyzer::HAMMING, 50, 500);

  /// \DONE
  /// Preprocess the input signal in order to ease pitch estimation. For instance,
  /// central-clipping or low pass filtering may be used.

  //  Cálculo del Threshold CL para el center-clipping
  float CL = 0.0;
  float potencia = 0.0;

  for (unsigned int i = 0; i < x.size(); i++)
  {
    potencia += abs(x[i]) * abs(x[i]);
  }
  potencia = (1.0F / x.size()) * potencia;

  CL = 0.8 * potencia;

  //  Aplicamos el Center Clipping
  for (unsigned int i = 0; i < x.size(); i++)
  {
    if (x[i] >= CL)
    {
      x[i] -= CL;
    }
    else if (abs(x[i]) < CL)
    {
      x[i] = 0;
    }
    else if (x[i] <= -CL)
    {
      x[i] += CL;
    }
  }

  // Iterate for each frame and save values in f0 vector
  vector<float>::iterator iX;
  vector<float> f0;
  for (iX = x.begin(); iX + n_len < x.end(); iX = iX + n_shift)
  {
    float f = analyzer(iX, iX + n_len);
    f0.push_back(f);
  }

  /// \DONE
  /// Postprocess the estimation in order to supress errors. For instance, a median filter
  /// or time-warping may be used.

  unsigned int mida_finestra = 3;
  float finestra[mida_finestra];
  float aux_ord;

  for (unsigned int i = 1; i < f0.size() - 1; ++i)
  { //recorrem els elements respectant els limits de la finestra
    for (unsigned int p = 0; p < mida_finestra; ++p)
    { //guardem els elements de la finestra (-1 0 1 )
      finestra[p] = f0[i - 1 + p];
    }
    for (unsigned int p = 0; p < mida_finestra; ++p)
    { //algoritme d'ordenacio
      int min = p;
      for (unsigned int k = p + 1; k < mida_finestra; k++)
      {
        if (finestra[min] > finestra[k])
        {
          min = k;
        }
      }
      aux_ord = finestra[p];
      finestra[p] = finestra[min];
      finestra[min] = aux_ord;
    }

    f0[i] = finestra[1]; //la mostra que ha quedat al mig serà la bona
    //apliquem el filtre a partir de la mostra 1 fins N-1 per evitar problemes amb marges
  }

  // Write f0 contour into the output file
  ofstream os(output_txt);
  if (!os.good())
  {
    cerr << "Error reading output file " << output_txt << " (" << strerror(errno) << ")\n";
    return -3;
  }

  os << 0 << '\n'; //pitch at t=0
  for (iX = f0.begin(); iX != f0.end(); ++iX)
    os << *iX << '\n';
  os << 0 << '\n'; //pitch at t=Dur

  return 0;
}
