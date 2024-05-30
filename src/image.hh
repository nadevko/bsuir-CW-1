#pragma once

#include <gtkmm.h>

#include <opencv2/opencv.hpp>
#include <vector>

#include "opencv2/core/mat.hpp"

namespace CW1 {
class Image : public Gtk::Image {
 public:
  Image(const Glib::RefPtr<Gdk::Pixbuf>& pixbuf);

  uint64_t get_rvhash(int numBins = 30, int numSectors = 180,
                      float sigma = 1.0f, float medianThreshold = 10.0f,
                      float stdDevThreshold = 10.0f) const;

 private:
  Glib::RefPtr<Gdk::Pixbuf> m_pixbuf;

  cv::Mat convertPixbufToMat() const;

  cv::Mat convertToGrayscale(const cv::Mat& image) const;

  cv::Mat gaussianBlur(const cv::Mat& image, float sigma) const;

  cv::Mat createGaussianKernel(int kernelSize, float sigma) const;

  cv::Mat filter(const cv::Mat& image, const cv::Mat& kernel) const;

  std::vector<std::vector<uchar>> divideImageIntoSectors(
      const cv::Mat& grayImage, int numSectors) const;

  void calculateSectorStatistics(
      const std::vector<std::vector<uchar>>& sectorPixels,
      std::vector<float>& sectorMedians,
      std::vector<float>& sectorStdDevs) const;

  float calculateMedian(const std::vector<uchar>& values) const;

  float calculateStandardDeviation(const std::vector<uchar>& values) const;

  uint64_t computeRVHash(const std::vector<float>& sectorMedians,
                         const std::vector<float>& sectorStdDevs,
                         int numSectors, float medianThreshold,
                         float stdDevThreshold) const;
};
}  // namespace CW1
