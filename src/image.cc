#include "image.hh"

#include <cmath>
#include <numeric>
#include <opencv2/imgproc/imgproc.hpp>

namespace CW1 {

Image::Image(const Glib::RefPtr<Gdk::Pixbuf>& pixbuf)
    : Gtk::Image(), m_pixbuf(pixbuf) {}

uint64_t Image::get_rvhash(int numBins, int numSectors, float sigma,
                           float medianThreshold, float stdDevThreshold) const {
  // Преобразование Gdk::Pixbuf в cv::Mat
  cv::Mat image = convertPixbufToMat();

  // Преобразование изображения в оттенки серого
  cv::Mat grayImage = convertToGrayscale(image);

  // Применение размытия Гаусса
  cv::Mat blurredImage = gaussianBlur(grayImage, sigma);

  // Разделение изображения на сектора
  std::vector<std::vector<uchar>> sectorPixels =
      divideImageIntoSectors(blurredImage, numSectors);

  // Вычисление статистик для каждого сектора
  std::vector<float> sectorMedians, sectorStdDevs;
  calculateSectorStatistics(sectorPixels, sectorMedians, sectorStdDevs);

  // Вычисление хеша на основе медиан и стандартных отклонений
  return computeRVHash(sectorMedians, sectorStdDevs, numSectors,
                       medianThreshold, stdDevThreshold);
}

cv::Mat Image::convertPixbufToMat() const {
  const guint8* pixels = m_pixbuf->get_pixels();
  int width = m_pixbuf->get_width();
  int height = m_pixbuf->get_height();
  int rowstride = m_pixbuf->get_rowstride();

  cv::Mat mat(height, width, CV_8UC3);

  int channels = 3;
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      for (int c = 0; c < channels; ++c) {
        mat.at<cv::Vec3b>(y, x)[c] = pixels[y * rowstride + x * channels + c];
      }
    }
  }

  return mat;
}

cv::Mat Image::convertToGrayscale(const cv::Mat& image) const {
  cv::Mat grayImage(image.rows, image.cols, CV_8UC1);

  for (int i = 0; i < image.rows; ++i) {
    for (int j = 0; j < image.cols; ++j) {
      uchar blue = image.at<cv::Vec3b>(i, j)[0];
      uchar green = image.at<cv::Vec3b>(i, j)[1];
      uchar red = image.at<cv::Vec3b>(i, j)[2];
      uchar gray = static_cast<uchar>((red + green + blue) / 3);
      grayImage.at<uchar>(i, j) = gray;
    }
  }

  return grayImage;
}

std::vector<std::vector<uchar>> Image::divideImageIntoSectors(
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

void Image::calculateSectorStatistics(
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

float Image::calculateMedian(const std::vector<uchar>& values) const {
  std::vector<uchar> sortedValues = values;
  std::sort(sortedValues.begin(), sortedValues.end());
  int mid = sortedValues.size() / 2;
  return (sortedValues.size() % 2 == 0)
             ? (sortedValues[mid - 1] + sortedValues[mid]) / 2.0f
             : sortedValues[mid];
}

float Image::calculateStandardDeviation(
    const std::vector<uchar>& values) const {
  float mean =
      std::accumulate(values.begin(), values.end(), 0.0f) / values.size();
  float sqSum =
      std::inner_product(values.begin(), values.end(), values.begin(), 0.0f);
  return sqrt(sqSum / values.size() - mean * mean);
}

uint64_t Image::computeRVHash(const std::vector<float>& sectorMedians,
                              const std::vector<float>& sectorStdDevs,
                              int numSectors, float medianThreshold,
                              float stdDevThreshold) const {
  uint64_t hash = 0;
  for (int i = 0; i < numSectors; ++i) {
    int nextIdx = (i + 1) % numSectors;
    hash |=
        (std::abs(sectorMedians[i] - sectorMedians[nextIdx]) >
             medianThreshold ||
         std::abs(sectorStdDevs[i] - sectorStdDevs[nextIdx]) > stdDevThreshold)
            ? (1ULL << i)
            : 0;
  }
  return hash;
}

cv::Mat Image::gaussianBlur(const cv::Mat& image, float sigma) const {
  int size = std::ceil(6 * sigma) + 1;
  if (size % 2 == 0) size++;

  cv::Mat kernel = createGaussianKernel(size, sigma);
  cv::Mat blurred = filter(image, kernel);

  return blurred;
}

cv::Mat Image::createGaussianKernel(int size, float sigma) const {
  cv::Mat kernel(size, size, CV_32F);
  float sigma2 = sigma * sigma;
  float mean = size / 2;  // NOLINT

  for (int i = 0; i < size; ++i) {
    for (int j = 0; j < size; ++j) {
      float x = i - mean;
      float y = j - mean;
      float exponent = -(x * x + y * y) / (2 * sigma2);
      kernel.at<float>(i, j) = std::exp(exponent) / (2 * CV_PI * sigma2);
    }
  }

  return kernel;
}

cv::Mat Image::filter(const cv::Mat& image, const cv::Mat& kernel) const {
  cv::Mat result(image.rows, image.cols, CV_32F, cv::Scalar(0));

  int kRows = kernel.rows;
  int kCols = kernel.cols;
  int kCenterX = kCols / 2;
  int kCenterY = kRows / 2;

  for (int y = 0; y < image.rows; ++y) {
    for (int x = 0; x < image.cols; ++x) {
      float sum = 0.0;
      for (int ky = 0; ky < kRows; ++ky) {
        int i = y + ky - kCenterY;
        if (i < 0 || i >= image.rows) continue;
        for (int kx = 0; kx < kCols; ++kx) {
          int j = x + kx - kCenterX;
          if (j < 0 || j >= image.cols) continue;
          sum += kernel.at<float>(ky, kx) * image.at<float>(i, j);
        }
      }
      result.at<float>(y, x) = sum;
    }
  }
  return result;
}

}  // namespace CW1
