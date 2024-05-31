#include "hashprocessor.hh"

#include <filesystem>

namespace CW1 {

Processor::Table HashProcessor::process_filepaths(
    const std::vector<std::string>& filepaths) const {
  Table result;
  for (const auto& filepath : filepaths) {
    if (std::filesystem::is_regular_file(filepath)) {
      process_file(filepath, result);
    } else if (std::filesystem::is_directory(filepath)) {
      process_directory(filepath, result);
    }
  }
  return result;
}

void HashProcessor::process_file(const std::string& filepath,
                                 Table& result) const {
  if (!is_image_file(filepath)) {
    return;
  }
  auto pixbuf = Gdk::Pixbuf::create_from_file(filepath);
  auto pattern = rvhash.compute(pixbuf);
  result[filepath].push_back(pattern);
}

void HashProcessor::process_directory(const std::string& directory,
                                      Table& result) const {
  for (const auto& entry : std::filesystem::directory_iterator(directory)) {
    if (recursive && std::filesystem::is_directory(entry.path())) {
      process_directory(entry.path().string(), result);
    } else if (std::filesystem::is_regular_file(entry.path())) {
      process_file(entry.path().string(), result);
    }
  }
}

}  // namespace CW1
