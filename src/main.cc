#include <gtkmm.h>

#include <iostream>
#include <numeric>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

class ImageWithHash : public Gtk::Image {
 public:
  ImageWithHash(const Glib::RefPtr<Gdk::Pixbuf>& pixbuf)
      : Gtk::Image(), m_pixbuf(pixbuf) {}

  uint64_t rvhash(int numBins = 256, int numSectors = 36, float sigma = 1.0f,
                  float medianThreshold = 10.0f,
                  float stdDevThreshold = 10.0f) const {
    cv::Mat image = convertPixbufToMat();
    cv::Mat grayImage = convertToGrayscale(image);
    cv::GaussianBlur(grayImage, grayImage, cv::Size(0, 0), sigma);
    std::vector<std::vector<uchar>> sectorPixels =
        divideImageIntoSectors(grayImage, numSectors);
    std::vector<float> sectorMedians, sectorStdDevs;
    calculateSectorStatistics(sectorPixels, sectorMedians, sectorStdDevs);
    return computeHash(sectorMedians, sectorStdDevs, numSectors,
                       medianThreshold, stdDevThreshold);
  }

 private:
  Glib::RefPtr<Gdk::Pixbuf> m_pixbuf;

  cv::Mat convertPixbufToMat() const {
    return cv::Mat(cv::Size(m_pixbuf->get_width(), m_pixbuf->get_height()),
                   CV_8UC3, const_cast<guchar*>(m_pixbuf->get_pixels()),
                   m_pixbuf->get_rowstride());
  }

  cv::Mat convertToGrayscale(const cv::Mat& image) const {
    cv::Mat grayImage;
    cv::cvtColor(image, grayImage, cv::COLOR_RGB2GRAY);
    return grayImage;
  }

  std::vector<std::vector<uchar>> divideImageIntoSectors(
      const cv::Mat& grayImage, int numSectors) const {
    cv::Point center(grayImage.cols / 2, grayImage.rows / 2);
    float sectorAngle = 360.0f / numSectors;
    std::vector<std::vector<uchar>> sectorPixels(numSectors);

    for (int i = 0; i < grayImage.rows; ++i) {
      for (int j = 0; j < grayImage.cols; ++j) {
        float angle = atan2(i - center.y, j - center.x) * 180.0f / CV_PI;
        if (angle < 0) angle += 360.0f;
        int sectorIdx = static_cast<int>(angle / sectorAngle);
        sectorPixels[sectorIdx].push_back(grayImage.at<uchar>(i, j));
      }
    }

    return sectorPixels;
  }

  void calculateSectorStatistics(
      const std::vector<std::vector<uchar>>& sectorPixels,
      std::vector<float>& sectorMedians,
      std::vector<float>& sectorStdDevs) const {
    for (const auto& pixels : sectorPixels) {
      if (!pixels.empty()) {
        float median = calculateMedian(pixels);
        float stdDev = calculateStandardDeviation(pixels);
        sectorMedians.push_back(median);
        sectorStdDevs.push_back(stdDev);
      } else {
        sectorMedians.push_back(0.0f);
        sectorStdDevs.push_back(0.0f);
      }
    }
  }

  float calculateMedian(const std::vector<uchar>& values) const {
    std::vector<uchar> sortedValues = values;
    std::sort(sortedValues.begin(), sortedValues.end());
    int mid = sortedValues.size() / 2;
    return (sortedValues.size() % 2 == 0)
               ? (sortedValues[mid - 1] + sortedValues[mid]) / 2.0f
               : sortedValues[mid];
  }

  float calculateStandardDeviation(const std::vector<uchar>& values) const {
    float mean =
        std::accumulate(values.begin(), values.end(), 0.0f) / values.size();
    float sqSum =
        std::inner_product(values.begin(), values.end(), values.begin(), 0.0f);
    return sqrt(sqSum / values.size() - mean * mean);
  }

  uint64_t computeHash(const std::vector<float>& sectorMedians,
                       const std::vector<float>& sectorStdDevs, int numSectors,
                       float medianThreshold, float stdDevThreshold) const {
    uint64_t hash = 0;
    for (int i = 0; i < numSectors; ++i) {
      int nextIdx = (i + 1) % numSectors;
      hash |= (std::abs(sectorMedians[i] - sectorMedians[nextIdx]) >
                   medianThreshold ||
               std::abs(sectorStdDevs[i] - sectorStdDevs[nextIdx]) >
                   stdDevThreshold)
                  ? (1ULL << i)
                  : 0;
    }
    return hash;
  }
};

int main(int argc, char* argv[]) {
  auto app = Gtk::Application::create();

  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <image_path1> [<image_path2> ...]\n";
    return 1;
  }

  size_t maxPathLength = 0;
  std::vector<std::string> images;
  for (int i = 1; i < argc; ++i) {
    images.push_back(argv[i]);
    maxPathLength =
        (maxPathLength >= images.back().size()) ?: images.back().size();
  }

  for (const auto& path : images) {
    try {
      auto pixbuf = Gdk::Pixbuf::create_from_file(path);
      ImageWithHash image(pixbuf);
      uint64_t hash = image.rvhash();
      std::cout << std::setw(maxPathLength) << std::left << path << " | "
                << hash << '\n';
    } catch (const Glib::FileError& ex) {
      std::cerr << "Error loading image " << path << ": " << ex.what() << '\n';
    }
  }

  return 0;
}
