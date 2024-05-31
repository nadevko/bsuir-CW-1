#pragma once

#include "processor.hh"

namespace CW1 {

class HashFormatter : public Processor::Formatter {
 public:
  void format(std::ostream& os, const Processor::Table& data) const override;
};

}  // namespace CW1
