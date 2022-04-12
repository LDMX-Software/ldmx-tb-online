#ifndef TRIGSCINT_SIMQIE_H
#define TRIGSCINT_SIMQIE_H

/**
 * @class SimQIE
 * @brief class for simulating QIE chip output
 * @note This should be initialized only once per simulation
 */
class SimQIE {
 public:
  /**
   * Defaut constructor
   * @note if used, noise & pedestal are not simulated
   */
  SimQIE();

  /**
   * Quantization error
   */
  float QErr(float Q);

  /**
   * Converting ADC back to charge
   * @param ADC = ADC count
   */
  float ADC2Q(int ADC);

 private:
  /// Indices of first bin of each subrange
  int nbins_[5] = {0, 16, 36, 57, 64};
  /// Charge lower limit of all the 16 subranges
  float edges_[17] = {-16,   34,    158,    419,    517,   915,
                      1910,  3990,  4780,   7960,   15900, 32600,
                      38900, 64300, 128000, 261000, 350000};
  /// sensitivity of the subranges (Total charge/no. of bins)
  float sense_[16] = {3.1,   6.2,   12.4,  24.8, 24.8, 49.6, 99.2, 198.4,
                      198.4, 396.8, 793.6, 1587, 1587, 3174, 6349, 12700};

};

#endif
