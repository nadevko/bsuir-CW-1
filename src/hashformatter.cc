#include "hashformatter.hh"

#include <iomanip>
#include <iostream>

namespace CW1 {

void HashFormatter::format(std::ostream& os,
                           const Processor::Table& data) const {
  size_t max_length = 0;
  for (const auto& pair : data) {
    max_length = std::max(max_length, pair.first.length());
  }

  for (const auto& pair : data) {
    os << std::setw(max_length) << pair.first << " | " << pair.second[0]
       << '\n';
  }
};

}  // namespace CW1
