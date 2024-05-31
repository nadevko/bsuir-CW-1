#include "hashformatter.hh"

#include <iomanip>
#include <iostream>

namespace CW1 {

void HashFormatter::format(std::ostream& os,
                           const Processor::Table& data) const {
  // Determine the length of the longest key
  size_t max_length = 0;
  for (const auto& pair : data) {
    max_length = std::max(max_length, pair.first.length());
  }

  // Format and write each entry to the stream
  for (const auto& pair : data) {
    os << std::setw(max_length) << pair.first << " | " << pair.second[0]
       << '\n';
  }
};

}  // namespace CW1
