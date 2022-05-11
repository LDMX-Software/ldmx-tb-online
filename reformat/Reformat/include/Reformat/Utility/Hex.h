#ifndef REFORMAT_UTILITY_HEX_H_
#define REFORMAT_UTILITY_HEX_H_

#include <iomanip>

namespace reformat::utility {

/**
 * @struct hex
 * A very simple wrapper enabling us to more easily
 * tell the output stream to style the input word
 * in hexidecimal format.
 *
 * @tparam[in] WordType type of word for styling
 */
template <typename WordType>
struct hex {
  static const std::size_t width_{2 * sizeof(WordType)};
  const WordType& word_;
  hex(const WordType& w) : word_{w} {}
  friend inline std::ostream& operator<<(
      std::ostream& os, const reformat::utility::hex<WordType>& h) {
    os << "0x" << std::setfill('0') << std::setw(h.width_) << std::hex
       << h.word_ << std::dec;
    return os;
  }
};

}  // namespace reformat::utility

#endif  // REFORMAT_UTILITY_HEX_H_
