#pragma once

#include <giomm/file.h>

#include "rvhash.hh"

namespace CW1 {

// Abstract base class Processor
class Processor {
 public:
  virtual ~Processor() = default;

  // Define Entry type
  using Entry = std::pair<std::string, RVHash::hash>;

  // Define Table type
  using Table = std::map<std::string, std::vector<RVHash::hash>>;

  // Abstract base class Formatter
  class Formatter {
   public:
    virtual ~Formatter() = default;
    virtual void format(std::ostream& os, const Table& data) const = 0;
  };

  // Pure virtual function to process file paths
  virtual Table process_filepaths(
      const std::vector<std::string>& filepaths) const = 0;

  // Pure virtual function to process a single file
  virtual void process_file(const std::string& filepath,
                            Table& result) const = 0;

  // Setters for parameters
  virtual void set_size(size_t value) { size = value; }
  virtual void set_bins(size_t value) { bins = value; }
  virtual void set_sectors(size_t value) { sectors = value; }
  virtual void set_sigma(float value) { sigma = value; }
  virtual void set_stdDeviationThreshold(float value) {
    stdDeviationThreshold = value;
  }
  virtual void set_medianThreshold(float value) { medianThreshold = value; }
  virtual void set_recursive(bool value) { recursive = value; }
  virtual void set_rvhash(const RVHash& value) { rvhash = value; }
  virtual void set_formatter(std::shared_ptr<const Formatter> value) {
    formatter = value;
  }

  // Convert Table to string using formatter
  std::string to_string(const Table& data) const {
    std::stringstream ss;
    format(ss, data);
    return ss.str();
  }

  // Format Table using formatter
  void format(std::ostream& os, const Table& data) const {
    if (!formatter) {
      throw std::runtime_error("Formatter is not set");
    }
    formatter->format(os, data);
  }

 protected:
  RVHash rvhash;  // RVHash object

 protected:
  // Protected fields for parameter values
  size_t size = 8;
  size_t bins = 30;
  size_t sectors = 180;
  float sigma = 1.0;
  float stdDeviationThreshold = 10.0;
  float medianThreshold = 10.0;
  bool recursive = false;  // Recursive flag, default is false

 protected:
  std::shared_ptr<const Formatter> formatter;  // Formatter object

 protected:
  // Method to check if a file is an image
  bool is_image_file(const std::string& filepath) const {
    try {
      auto pixbuf = Gdk::Pixbuf::create_from_file(filepath);
      return true;
    } catch (const Gdk::PixbufError& ex) {
      // Failed to create Pixbuf from file, file format may not be supported
      return false;
    } catch (const Glib::Error& ex) {
      // Other Glib errors may occur
      return false;
    }
  }
};

}  // namespace CW1
