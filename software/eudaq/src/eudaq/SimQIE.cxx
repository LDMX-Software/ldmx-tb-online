#include "eudaq/SimQIE.h"
#include <iostream>

SimQIE::SimQIE() {}
  
// Function to convert ADCs back to charge
// The method checks to which QIE subrange does the ADC correspnd to
// and returns the mean charge of the correspnding bin in the subrange
float SimQIE::ADC2Q(int ADC) {
  if (ADC <= 0) return -16;
  if (ADC >= 255) return 350000;

  int rr = ADC / 64;  // range
  int v1 = ADC % 64;  // temp. var
  int ss = 0;         // sub range

  for (int i = 1; i < 4; i++) {  // to get the subrange
    if (v1 > nbins_[i]) ss++;
  }
  int cc = 64 * rr + nbins_[ss];
  float temp = edges_[4 * rr + ss] + (v1 - nbins_[ss]) * sense_[4 * rr + ss] +
               sense_[4 * rr + ss] / 2;
  return temp ;
}

// Function to return the quantization error for given input charge
float SimQIE::QErr(float Q) {
  if (Q <= edges_[0]) return 0;
  if (Q >= edges_[16]) return 0;

  int ID = 8;
  int a = 0;
  int b = 16;
  while (b - a != 1) {
    if (Q > edges_[(a + b) / 2])
      a = (a + b) / 2;
    else
      b = (a + b) / 2;
  }
  return ( sense_[a] / ( 3.4641 * Q ) ); // sqrt(12) = 3.4641
}

