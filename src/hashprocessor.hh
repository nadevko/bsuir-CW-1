#pragma once

#include "processor.hh"

namespace CW1 {

class HashProcessor : public Processor {
 public:
  // Override process_filepaths method
  Table process_filepaths(
      const std::vector<std::string>& filepaths) const override;

  // Override process_file method
  void process_file(const std::string& filepath, Table& result) const override;

 protected:
  // Process directory recursively
  void process_directory(const std::string& directory, Table& result) const;
};

}  // namespace CW1
