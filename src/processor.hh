#pragma once

#include <giomm/file.h>

#include "rvhash.hh"

namespace CW1 {

class Processor {
 public:
  virtual ~Processor() = default;

  using Entry = std::pair<std::string, RVHash::hash>;

  using Table = std::map<std::string, std::vector<RVHash::hash>>;

  class Formatter {
   public:
    virtual ~Formatter() = default;
    virtual void format(std::ostream& os, const Table& data) const = 0;
  };

  virtual Table process_filepaths(
      const std::vector<std::string>& filepaths) const = 0;

  virtual void process_file(const std::string& filepath,
                            Table& result) const = 0;

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

  std::string to_string(const Table& data) const {
    std::stringstream ss;
    format(ss, data);
    return ss.str();
  }

  void format(std::ostream& os, const Table& data) const {
    if (!formatter) {
      throw std::runtime_error("Formatter is not set");
    }
    formatter->format(os, data);
  }

 protected:
  RVHash rvhash;

 protected:
  size_t size = 8;
  size_t bins = 30;
  size_t sectors = 180;
  float sigma = 1.0;
  float stdDeviationThreshold = 10.0;
  float medianThreshold = 10.0;
  bool recursive = false;

 protected:
  std::shared_ptr<const Formatter> formatter;

 protected:
  bool is_image_file(const std::string& filepath) const {
    try {
      auto pixbuf = Gdk::Pixbuf::create_from_file(filepath);
      return true;
    } catch (const Gdk::PixbufError& ex) {
      return false;
    } catch (const Glib::Error& ex) {
      return false;
    }
  }
};

}  // namespace CW1
