#pragma once

#include "processor.hh"

namespace CW1 {

class HashProcessor : public Processor {
 public:
  Table process_filepaths(
      const std::vector<std::string>& filepaths) const override;

  void process_file(const std::string& filepath, Table& result) const override;

 protected:
  void process_directory(const std::string& directory, Table& result) const;
};

}  // namespace CW1
