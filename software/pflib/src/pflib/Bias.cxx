#include "pflib/Bias.h"

#include <iostream>
#include <bitset>

namespace pflib {

// Board Commands
const uint8_t MAX5825::WDOG = 1 << 4;

// DAC Commands
//  add the DAC selection to these commands
//  to get the full command byte
const uint8_t MAX5825::RETURNn       = 7  << 4;
const uint8_t MAX5825::CODEn         = 8  << 4;
const uint8_t MAX5825::LOADn         = 9  << 4;
const uint8_t MAX5825::CODEn_LOADALL = 10 << 4;
const uint8_t MAX5825::CODEn_LOADn   = 11 << 4;

MAX5825::MAX5825(I2C& i2c, uint8_t addr, int bus) : i2c_{i2c}, our_addr_{addr}, bus_{bus} {}

std::vector<uint8_t> MAX5825::get(uint8_t cmd, int n_return_bytes) {
  i2c_.set_active_bus(bus_);
  i2c_.set_bus_speed(1300);
  
  return i2c_.general_write_read(our_addr_, {cmd}, n_return_bytes);
}

void MAX5825::set(uint8_t cmd, uint16_t data_bytes) {
  i2c_.set_active_bus(bus_);
  i2c_.set_bus_speed(1300);

  std::vector<unsigned char> instructions = { 
    cmd, // CODE for this dac
    data_bytes >> 8, // first data byte
    data_bytes & 0xFF, // second data byte
  };

  i2c_.general_write_read(our_addr_, instructions);

  return;
}

/**
 * get by DAC index
 */
std::vector<uint16_t> MAX5825::getByDAC(uint8_t i_dac, uint8_t cmd) {
  int num_dacs = 1;
  if (i_dac > 7) {
    i_dac = 8;
    num_dacs = 8;
  }
  auto bytes{get(cmd+i_dac, 2*num_dacs)};
  std::vector<uint16_t> vals(num_dacs);
  for (unsigned int j{0}; j < num_dacs; j++) {
    vals[j] = (bytes.at(2*j)<<4)+(bytes.at(2*j+1)>>4);
  }
  return vals;
}

/**
 * set by DAC index
 */
void MAX5825::setByDAC(uint8_t i_dac, uint8_t cmd, uint16_t data_bytes) {
  if (i_dac > 7) i_dac = 8;
  // for the MAX5825, the voltages are 12-bits,
  // so the 4 LSBs of the two data bytes will be ignored.
  data_bytes <<= 4;
  set(cmd+i_dac, data_bytes);
}

/// DAC chip addresses
const uint8_t Bias::ADDR_LED_0  = 0x18;
const uint8_t Bias::ADDR_LED_1  = 0x1A;
const uint8_t Bias::ADDR_SIPM_0 = 0x10;
const uint8_t Bias::ADDR_SIPM_1 = 0x12;

Bias::Bias(I2C& i2c, int bus) {
  led_.emplace_back(i2c, Bias::ADDR_LED_0, bus);
  led_.emplace_back(i2c, Bias::ADDR_LED_1 , bus);
  sipm_.emplace_back(i2c, Bias::ADDR_SIPM_0, bus);
  sipm_.emplace_back(i2c, Bias::ADDR_SIPM_1 , bus);
}

void Bias::codeLED(uint8_t i_led, uint16_t code) {
  int i_chip = (i_led > 7);
  led_.at(i_chip).codeDAC(i_led - i_chip*8, code);
}

void Bias::codeSiPM(uint8_t i_sipm, uint16_t code) {
  int i_chip = (i_sipm > 7);
  sipm_.at(i_chip).codeDAC(i_sipm - i_chip*8, code);
}

}
