/// @file

#include <iostream>
#include <math.h>
#include <iomanip>
#include "pitch_analyzer.h"

using namespace std;

/// Name space of UPC
namespace upc
{
void PitchAnalyzer::autocorrelation(const vector<float> &x, vector<float> &r) const
{

  for (unsigned int l = 0; l < r.size(); ++l)
  {
    /// \DONE Compute the autocorrelation r[l]
    for (unsigned int n = 0; n < (x.size() - l); n++)
    {
      r[l] = r[l] + x[n] * x[n + l];
    }
    r[l] = (1.0F / x.size()) * r[l];
  }

  if (r[0] == 0.0F) //to avoid log() and divide zero
    r[0] = 1e-10;
}

void PitchAnalyzer::set_window(Window win_type)
{
  if (frameLen == 0)
    return;

  window.resize(frameLen);

  //HAMMING CONSTANTS
  const float a0 = 0.53836;
  const float a1 = 0.46164;

  switch (win_type)
  {
  case HAMMING:
    /// \DONE Implement the Hamming window
    for (unsigned int n = 0; n < frameLen; n++)
    {
      window[n] = a0 - a1 * cos((2 * M_PI * n) / (frameLen - 1));
    }
    break;
  case RECT:
  default:
    window.assign(frameLen, 1);
  }
}

void PitchAnalyzer::set_f0_range(float min_F0, float max_F0)
{
  npitch_min = (unsigned int)samplingFreq / max_F0;
  if (npitch_min < 2)
    npitch_min = 2; // samplingFreq/2

  npitch_max = 1 + (unsigned int)samplingFreq / min_F0;

  //frameLen should include at least 2*T0
  if (npitch_max > frameLen / 2)
    npitch_max = frameLen / 2;
}

bool PitchAnalyzer::unvoiced(float pot, float r1norm, float rmaxnorm) const
{
  /// \DONE Implement a rule to decide whether the sound is voiced or not.
  /// * You can use the standard features (pot, r1norm, rmaxnorm),
  ///   or compute and use other ones.

  //const float r1 = 0.70 (th1); 
  //const float rmax = 0.3 (th2);
  //const float pot = -50.5 (thPot);

  if (pot < thPot || r1norm < th1 || rmaxnorm < th2 || (r1norm < 0.935 && rmaxnorm < 0.4))
  {
    return true; //Unvoiced Sound
  }
  else
  {
    return false; //Voiced Sound
  }
}

float PitchAnalyzer::compute_pitch(vector<float> &x) const
{
  if (x.size() != frameLen)
    return -1.0F;

  //Window input frame
  for (unsigned int i = 0; i < x.size(); ++i)
    x[i] *= window[i];

  //Declaramos vector r de npitch_max posiciones
  vector<float> r(npitch_max);

  //Compute correlation
  autocorrelation(x, r);

  /// \DONE
  /// Find the lag of the maximum value of the autocorrelation away from the origin.<br>
  /// Choices to set the minimum value of the lag are:
  ///    - The first negative value of the autocorrelation.
  ///    - The lag corresponding to the maximum value of the pitch.
  ///	   .
  /// In either case, the lag should not exceed that of the minimum value of the pitch.


  vector<float>::const_iterator iR = r.begin(), iRMax = iR;
  //Creem dos iteradors auxiliars, que pendran els valors immediament inferior i posterior
  //al valor de la posició actual de l'iterador
  vector<float>::const_iterator iRanterior = r.begin();
  vector<float>::const_iterator iRposterior = r.begin();

  //Iniciem iRposterior
  iRposterior = iR + 1;

  /*Bucle que serveix per establir el miním valor coherent del segon pic de l'autocorrelació
    Utilitzem 3 condicions:
    - 1era condició: Ens assegura que arribem fins abaix del primer pic de l'autocorrelació
    - 2na condició: Ens assegura, en cas que el primer pic baixi molt ràpid, que superem el valor
      mínim establert 
    - 3era condició: Ens assegura que en cas de tenir un primer pic molt ample, avançem el suficient
      com per a no pendre com a segon pic un valor que encara sigui del primer pic
  */
  while (*iR > *iRposterior || iR < r.begin() + npitch_min || *iR > 0.0F)
  {
    ++iR;
    ++iRposterior;
  }

  iRMax = iR;

  /* Un cop hem establert el valor mínim coherent de la distancia al segon pic,
  comprovem fent ús de iRanterior i iRposterior que iRMax es trobi en un pic, comprovant
  que iR és a l'hora major que iRanterior i iRposterior
     - Establim com a condició que iR no pot superar el llindar npitch_max - 
  */
  while (iR < r.begin() + npitch_max)
  {
    if (*iR > *iRMax)
    {
      iRanterior = iR - 1;
      iRposterior = iR + 1;
      if (*iR > *iRanterior && *iR > *iRposterior)
        iRMax = iR;
    }
    ++iR;
  }

  unsigned int lag = iRMax - r.begin();

  float pot = 10 * log10(r[0]);

  //You can print these (and other) features, look at them using wavesurfer
  //Based on that, implement a rule for unvoiced
  //change to #if 1 and compile
#if 1
  //if (r[0] > 0.0F)
  cout << fixed << setprecision(3) << pot << '\t' << '\t' << r[1] / r[0] << '\t' << '\t' << r[lag] / r[0] << endl;
#endif

  if (unvoiced(pot, r[1] / r[0], r[lag] / r[0]))
    return 0;
  else
    return (float)samplingFreq / (float)lag;
}
} // namespace upc
