#include "rvhash.hh"

#include <cmath>
#include <numeric>

namespace CW1 {

RVHash::RVHash() {}

void RVHash::set_bins(int bins) { this->bins = bins; }
void RVHash::set_sectors(int sectors) { this->sectors = sectors; }
void RVHash::set_sigma(float sigma) { this->sigma = sigma; }
void RVHash::set_stdDeviationThreshold(float stdDeviationThreshold) {
  this->stdDeviationThreshold = stdDeviationThreshold;
}
void RVHash::set_medianThreshold(float medianThreshold) {
  this->medianThreshold = medianThreshold;
}

uint64_t RVHash::compute(const buffer& pixbuf) const {
  // Преобразование в оттенки серого
  auto gray = get_grayscale(pixbuf);
  // Применение размытия Гаусса
  auto blurred = get_gaussian_blur(gray);
  // Разделение изображения на сектора
  auto sectors = get_sectors(blurred);
  // Вычисление статистик для каждого сектора
  stats medians, stddeviation;
  get_sector_stats(sectors, medians, stddeviation);
  // Вычисление хеша на основе медиан и стандартных отклонений
  return compute(medians, stddeviation);
}

RVHash::buffer RVHash::get_grayscale(const buffer& color) const {
  // Получаем размеры изображения
  auto width = color->get_width();
  auto height = color->get_height();

  // Создаем новый буффер с одним каналом
  buffer gray =
      Gdk::Pixbuf::create(Gdk::Colorspace::RGB, true, 8, width, height);

  // Получаем указатель на пиксели исходного изображения
  auto srcPixels = color->get_pixels();
  auto srcRowstride = color->get_rowstride();

  // Получаем указатель на пиксели нового изображения
  auto destPixels = gray->get_pixels();
  auto destRowstride = gray->get_rowstride();

  // Преобразовываем пиксели в оттенки серого
  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x) {
      // Получаем индекс текущего пикселя в массиве
      int current = y * srcRowstride + x * 3;
      // Вычисляем яркость и присваиваем её всем каналам в новом пикселе
      auto pixel = static_cast<guint8>((srcPixels[current] * 0.3) +
                                       (srcPixels[current + 1] * 0.59) +
                                       (srcPixels[current + 2] * 0.11));
      // Устанавливаем значения пикселя в новом изображении
      destPixels[y * destRowstride + x] = pixel;
    }

  return gray;
}

std::vector<RVHash::sector> RVHash::get_sectors(const buffer& pixbuf) const {
  // Получаем размеры изображения
  auto width = pixbuf->get_width();
  auto height = pixbuf->get_height();
  std::vector<sector> vecSectors(sectors);

  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x) {
      // Вычисляем угол для определения сектора
      float angle =
          atan2(y - height / 2, x - width / 2) * 180.0f / M_PI;  // NOLINT
      if (angle < 0) angle += 360.0f;
      // Определяем индекс сектора для текущего пикселя
      auto index = static_cast<int>(angle / (360.0f / sectors));
      // Получаем указатель на текущий пиксель
      auto pixel = pixbuf->get_pixels() + y * pixbuf->get_rowstride() + x;
      // Добавляем значение пикселя в соответствующий сектор
      vecSectors[index].push_back(*pixel);
    }

  return vecSectors;
}

void RVHash::get_sector_stats(const std::vector<sector>& sectors,
                              stats& medians, stats& stddeviations) const {
  // Для каждого сектора вычисляем медиану и стандартное отклонение
  for (const auto& sector : sectors) {
    if (sector.empty()) {
      // Если сектор пуст, добавляем нули в вектора статистик
      medians.push_back(0.0f);
      stddeviations.push_back(0.0f);
    } else {
      // Добавляем полученные значения в соответствующие вектора
      medians.push_back(get_median(sector));
      stddeviations.push_back(get_stddeviation(sector));
    }
  }
}

float RVHash::get_median(const sector& sector) const {
  // Копируем значения пикселей для сортировки
  auto sorted = sector;
  // Сортируем значения пикселей
  std::sort(sorted.begin(), sorted.end());
  // Вычисляем индекс середины массива
  auto mid = sorted.size() / 2;
  // Если количество элементов четное, то медиана - среднее двух средних
  // элементов, иначе медиана - значение среднего элемента
  return (sorted.size() % 2 == 0) ? (sorted[mid - 1] + sorted[mid]) / 2.0f
                                  : sorted[mid];
}

float RVHash::get_stddeviation(const sector& sector) const {
  // Вычисляем среднее значение
  auto mean =
      std::accumulate(sector.begin(), sector.end(), 0.0f) / sector.size();
  // Вычисляем сумму квадратов разностей между значениями и средним
  auto squaresSum =
      std::inner_product(sector.begin(), sector.end(), sector.begin(), 0.0f);
  // Вычисляем стандартное отклонение
  return sqrt(squaresSum / sector.size() - pow(mean, 2));
}

uint64_t RVHash::compute(const stats& medians,
                         const stats& stddeviations) const {
  uint64_t hash = 0;
  // Для каждого сектора вычисляем хеш на основе медианы и стандартного
  // отклонения
  for (int i = 0; i < sectors; ++i) {
    // Вычисляем индекс следующего сектора
    int next = (i + 1) % sectors;
    // Если разница между медианами или стандартными отклонениями превышает
    // порог, устанавливаем соответствующий бит в хеше
    hash |= (std::abs(medians[i] - medians[next]) > medianThreshold ||
             std::abs(stddeviations[i] - stddeviations[next]) >
                 stdDeviationThreshold)
                ? (1ULL << i)
                : 0;
  }
  // Возвращаем вычисленный хеш
  return hash;
}

RVHash::buffer RVHash::get_gaussian_blur(
    const Glib::RefPtr<Gdk::Pixbuf>& pixbuf) const {
  // Создаем ядро Гаусса
  int kernelSize = std::ceil(6 * sigma) + 1;
  if (kernelSize % 2 == 0) kernelSize++;
  // Применяем фильтр с использованием созданного ядра Гаусса
  return get_filtered(pixbuf, get_gaussian_kernel(kernelSize));
}

RVHash::kernel RVHash::get_gaussian_kernel(const int& size) const {
  // Создаем пустое ядро Гаусса
  kernel kernel(size, stats(size));

  // Заполняем ядро Гаусса
  for (int i = 0; i < size; ++i)
    for (int j = 0; j < size; ++j) {
      // Вычисляем расстояние от текущего пикселя до центра ядра
      auto x = i - size / 2;
      auto y = j - size / 2;
      // Вычисляем значение экспоненты в формуле Гаусса
      auto exponent = -(x * x + y * y) / (2 * pow(sigma, 2));
      // Вычисляем значение в ядре Гаусса
      kernel[i][j] = std::exp(exponent) / (2 * M_PI * pow(sigma, 2));
    }

  return kernel;
}

RVHash::buffer RVHash::get_filtered(const buffer& pixbuf,
                                    const kernel& kernel) const {
  // Получаем размеры изображения
  int width = pixbuf->get_width();
  int height = pixbuf->get_height();
  int channels = pixbuf->get_n_channels();

  // Создаем новый Pixbuf для результата фильтрации
  buffer result =
      Gdk::Pixbuf::create(Gdk::Colorspace::RGB, true, 8, width, height);

  // Применяем фильтр
  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x) {
      std::vector<float> pixels(channels, 0.0f);
      // Проходим по каждому пикселю в окрестности текущего пикселя
      for (size_t ky = 0; ky < kernel.size(); ++ky)
        for (size_t kx = 0; kx < kernel[ky].size(); ++kx) {
          // Вычисляем координаты текущего пикселя
          auto bufX = std::max(
              0,
              std::min(width - 1, x - static_cast<int>(kernel[ky].size()) / 2 +
                                      static_cast<int>(kx)));
          auto bufY = std::max(
              0, std::min(height - 1, y - static_cast<int>(kernel.size()) / 2 +
                                          static_cast<int>(ky)));
          // Получаем указатель на текущий пиксель
          auto current = pixbuf->get_pixels() + bufY * pixbuf->get_rowstride() +
                         bufX * channels;
          auto weight = kernel[ky][kx];
          // Вычисляем сумму взвешенных значений пикселей
          for (int c = 0; c < channels; ++c) pixels[c] += current[c] * weight;
        }
      // Устанавливаем новое значение пикселя в результате фильтрации
      auto destPixel =
          result->get_pixels() + y * result->get_rowstride() + x * channels;
      for (int c = 0; c < channels; ++c)
        destPixel[c] = static_cast<guint8>(pixels[c]);
    }

  return result;
}

}  // namespace CW1
