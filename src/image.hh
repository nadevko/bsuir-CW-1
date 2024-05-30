#pragma once

#include <gdkmm/pixbuf.h>
#include <gtkmm/image.h>

#include <vector>

namespace CW1 {

class Image : public Gtk::Image {
 public:
  Image(const Glib::RefPtr<Gdk::Pixbuf>& pixbuf);

  uint64_t get_rvhash(int numBins = 30, int numSectors = 180,
                      float sigma = 1.0f, float medianThreshold = 10.0f,
                      float stdDevThreshold = 10.0f) const;

 private:
  Glib::RefPtr<Gdk::Pixbuf> convertToGrayscale(
      const Glib::RefPtr<Gdk::Pixbuf>& pixbuf) const;
  std::vector<std::vector<char>> divideImageIntoSectors(
      const Glib::RefPtr<Gdk::Pixbuf>& pixbuf, int numSectors) const;
  void calculateSectorStatistics(
      const std::vector<std::vector<char>>& sectorPixels,
      std::vector<float>& sectorMedians,
      std::vector<float>& sectorStdDevs) const;
  float calculateMedian(const std::vector<char>& values) const;
  float calculateStandardDeviation(const std::vector<char>& values) const;
  uint64_t computeRVHash(const std::vector<float>& sectorMedians,
                         const std::vector<float>& sectorStdDevs,
                         int numSectors, float medianThreshold,
                         float stdDevThreshold) const;
  Glib::RefPtr<Gdk::Pixbuf> gaussianBlur(
      const Glib::RefPtr<Gdk::Pixbuf>& pixbuf, float sigma) const;
  std::vector<std::vector<float>> createGaussianKernel(int size,
                                                       float sigma) const;
  Glib::RefPtr<Gdk::Pixbuf> filter(
      const Glib::RefPtr<Gdk::Pixbuf>& image,
      const std::vector<std::vector<float>>& kernel) const;
  Glib::RefPtr<Gdk::Pixbuf> m_pixbuf;
};

}  // namespace CW1
