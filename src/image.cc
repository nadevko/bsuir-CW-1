#include "image.hh"

#include <gdkmm/pixbuf.h>

#include <cmath>
#include <numeric>

namespace CW1 {

Image::Image(const Glib::RefPtr<Gdk::Pixbuf>& pixbuf)
    : Gtk::Image(), m_pixbuf(pixbuf) {}

uint64_t Image::get_rvhash(int numBins, int numSectors, float sigma,
                           float medianThreshold, float stdDevThreshold) const {
  // Преобразование Gdk::Pixbuf в оттенки серого
  Glib::RefPtr<Gdk::Pixbuf> grayPixbuf = convertToGrayscale(m_pixbuf);

  // Применение размытия Гаусса
  Glib::RefPtr<Gdk::Pixbuf> blurredPixbuf = gaussianBlur(grayPixbuf, sigma);

  // Разделение изображения на сектора
  std::vector<std::vector<char>> sectorPixels =
      divideImageIntoSectors(blurredPixbuf, numSectors);

  // Вычисление статистик для каждого сектора
  std::vector<float> sectorMedians, sectorStdDevs;
  calculateSectorStatistics(sectorPixels, sectorMedians, sectorStdDevs);

  // Вычисление хеша на основе медиан и стандартных отклонений
  return computeRVHash(sectorMedians, sectorStdDevs, numSectors,
                       medianThreshold, stdDevThreshold);
}

Glib::RefPtr<Gdk::Pixbuf> Image::convertToGrayscale(
    const Glib::RefPtr<Gdk::Pixbuf>& pixbuf) const {
  // Получаем размеры изображения
  int width = pixbuf->get_width();
  int height = pixbuf->get_height();

  // Создаем новый Pixbuf с одним каналом
  Glib::RefPtr<Gdk::Pixbuf> grayPixbuf =
      Gdk::Pixbuf::create(Gdk::Colorspace::RGB, true, 8, width, height);

  // Получаем указатель на пиксели исходного изображения
  const guint8* srcPixels = pixbuf->get_pixels();
  int srcRowstride = pixbuf->get_rowstride();

  // Получаем указатель на пиксели нового изображения
  guint8* destPixels = grayPixbuf->get_pixels();
  int destRowstride = grayPixbuf->get_rowstride();

  // Преобразовываем пиксели в оттенки серого
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      // Получаем индекс текущего пикселя в массиве
      int srcIndex = y * srcRowstride + x * 3;
      // Вычисляем яркость и присваиваем её всем каналам в новом пикселе
      guint8 gray = static_cast<guint8>((srcPixels[srcIndex] * 0.3) +
                                        (srcPixels[srcIndex + 1] * 0.59) +
                                        (srcPixels[srcIndex + 2] * 0.11));
      // Устанавливаем значения пикселя в новом изображении
      destPixels[y * destRowstride + x] = gray;
    }
  }

  return grayPixbuf;
}

std::vector<std::vector<char>> Image::divideImageIntoSectors(
    const Glib::RefPtr<Gdk::Pixbuf>& pixbuf, int numSectors) const {
  int width = pixbuf->get_width();
  int height = pixbuf->get_height();
  std::vector<std::vector<char>> sectorPixels(numSectors);

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      float angle = atan2(y - height / 2, x - width / 2) * 180.0f / M_PI;
      if (angle < 0) angle += 360.0f;
      int sectorIdx = static_cast<int>(angle / (360.0f / numSectors));
      const guint8* pixel =
          pixbuf->get_pixels() + y * pixbuf->get_rowstride() + x;
      sectorPixels[sectorIdx].push_back(*pixel);
    }
  }

  return sectorPixels;
}

void Image::calculateSectorStatistics(
    const std::vector<std::vector<char>>& sectorPixels,
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

float Image::calculateMedian(const std::vector<char>& values) const {
  std::vector<char> sortedValues = values;
  std::sort(sortedValues.begin(), sortedValues.end());
  int mid = sortedValues.size() / 2;
  return (sortedValues.size() % 2 == 0)
             ? (sortedValues[mid - 1] + sortedValues[mid]) / 2.0f
             : sortedValues[mid];
}

float Image::calculateStandardDeviation(const std::vector<char>& values) const {
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

Glib::RefPtr<Gdk::Pixbuf> Image::gaussianBlur(
    const Glib::RefPtr<Gdk::Pixbuf>& pixbuf, float sigma) const {
  // Получаем размеры изображения
  int width = pixbuf->get_width();
  int height = pixbuf->get_height();
  int channels = pixbuf->get_n_channels();

  // Создаем новый Pixbuf для размытого изображения
  Glib::RefPtr<Gdk::Pixbuf> blurredPixbuf =
      Gdk::Pixbuf::create(Gdk::Colorspace::RGB, true, 8, width, height);

  // Создаем ядро Гаусса
  int kernelSize = std::ceil(6 * sigma) + 1;
  if (kernelSize % 2 == 0) kernelSize++;
  std::vector<std::vector<float>> kernel(kernelSize,
                                         std::vector<float>(kernelSize));

  float sigma2 = sigma * sigma;
  float mean = kernelSize / 2;  // NOLINT

  // Заполняем ядро Гаусса
  for (int i = 0; i < kernelSize; ++i) {
    for (int j = 0; j < kernelSize; ++j) {
      float x = i - mean;
      float y = j - mean;
      float exponent = -(x * x + y * y) / (2 * sigma2);
      kernel[i][j] = std::exp(exponent) / (2 * M_PI * sigma2);
    }
  }

  // Применяем размытие Гаусса
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      std::vector<float> pixelValue(channels, 0.0f);
      float totalWeight = 0.0f;
      for (int ky = 0; ky < kernelSize; ++ky) {
        for (int kx = 0; kx < kernelSize; ++kx) {
          int imgX = std::max(0, std::min(width - 1, x - kernelSize / 2 + kx));
          int imgY = std::max(0, std::min(height - 1, y - kernelSize / 2 + ky));
          const guint8* srcPixel = pixbuf->get_pixels() +
                                   imgY * pixbuf->get_rowstride() +
                                   imgX * channels;
          float weight = kernel[ky][kx];
          for (int c = 0; c < channels; ++c) {
            pixelValue[c] += srcPixel[c] * weight;
          }
          totalWeight += weight;
        }
      }
      guint8* destPixel = blurredPixbuf->get_pixels() +
                          y * blurredPixbuf->get_rowstride() + x * channels;
      for (int c = 0; c < channels; ++c) {
        destPixel[c] = static_cast<guint8>(pixelValue[c] / totalWeight);
      }
    }
  }

  return blurredPixbuf;
}

std::vector<std::vector<float>> Image::createGaussianKernel(int size,
                                                            float sigma) const {
  // Создаем пустое ядро Гаусса
  std::vector<std::vector<float>> kernel(size, std::vector<float>(size));

  float sigma2 = sigma * sigma;
  float mean = size / 2;  // NOLINT

  // Заполняем ядро Гаусса
  for (int i = 0; i < size; ++i) {
    for (int j = 0; j < size; ++j) {
      float x = i - mean;
      float y = j - mean;
      float exponent = -(x * x + y * y) / (2 * sigma2);
      kernel[i][j] = std::exp(exponent) / (2 * M_PI * sigma2);
    }
  }

  return kernel;
}

Glib::RefPtr<Gdk::Pixbuf> Image::filter(
    const Glib::RefPtr<Gdk::Pixbuf>& image,
    const std::vector<std::vector<float>>& kernel) const {
  // Получаем размеры изображения
  int width = image->get_width();
  int height = image->get_height();
  int channels = image->get_n_channels();

  // Создаем новый Pixbuf для результата фильтрации
  Glib::RefPtr<Gdk::Pixbuf> resultPixbuf =
      Gdk::Pixbuf::create(Gdk::Colorspace::RGB, true, 8, width, height);

  // Применяем фильтр
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      std::vector<float> pixelValue(channels, 0.0f);
      for (size_t ky = 0; ky < kernel.size(); ++ky) {
        for (size_t kx = 0; kx < kernel[ky].size(); ++kx) {
          int imgX = std::max(
              0,
              std::min(width - 1, x - static_cast<int>(kernel[ky].size()) / 2 +
                                      static_cast<int>(kx)));
          int imgY = std::max(
              0, std::min(height - 1, y - static_cast<int>(kernel.size()) / 2 +
                                          static_cast<int>(ky)));
          const guint8* srcPixel = image->get_pixels() +
                                   imgY * image->get_rowstride() +
                                   imgX * channels;
          float weight = kernel[ky][kx];
          for (int c = 0; c < channels; ++c) {
            pixelValue[c] += srcPixel[c] * weight;
          }
        }
      }
      guint8* destPixel = resultPixbuf->get_pixels() +
                          y * resultPixbuf->get_rowstride() + x * channels;
      for (int c = 0; c < channels; ++c) {
        destPixel[c] = static_cast<guint8>(pixelValue[c]);
      }
    }
  }

  return resultPixbuf;
}

}  // namespace CW1
