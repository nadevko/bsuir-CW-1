#pragma once

#include <gdkmm/pixbuf.h>

namespace CW1 {

class RVHash {
 private:
  using buffer = const Glib::RefPtr<Gdk::Pixbuf>;
  using sector = std::vector<char>;
  using stats = std::vector<float>;
  using kernel = std::vector<std::vector<float>>;
  int bins = 30;
  int sectors = 180;
  float sigma = 1.0;
  float stdDeviationThreshold = 10.0;
  float medianThreshold = 10.0;

 public:
  RVHash();

  uint64_t compute(const buffer& pixbuf) const;
  void set_bins(int bins);
  void set_sectors(int sectors);
  void set_sigma(float sigma);
  void set_stdDeviationThreshold(float stdDeviationThreshold);
  void set_medianThreshold(float medianThreshold);

 private:
  buffer get_grayscale(const buffer& pixbuf) const;
  std::vector<sector> get_sectors(const buffer& pixbuf) const;
  void get_sector_stats(const std::vector<sector>& sectors, stats& medians,
                        stats& stddeviations) const;
  float get_median(const sector& sector) const;
  float get_stddeviation(const sector& sector) const;
  uint64_t compute(const stats& medians, const stats& stddeviations) const;
  buffer get_gaussian_blur(const buffer& pixbuf) const;
  kernel get_gaussian_kernel(const int& size) const;
  buffer get_filtered(const buffer& pixbuf, const kernel& kernel) const;
};

}  // namespace CW1
