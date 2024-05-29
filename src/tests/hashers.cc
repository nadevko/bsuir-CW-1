#include <iostream>
#include <opencv2/img_hash.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, char** argv) {
  if (argc != 3) {
    cerr << "Использование: " << argv[0]
         << " <путь_к_изображению1> <путь_к_изображению2>" << endl;
    return 1;
  }

  string image1_path = argv[1];
  string image2_path = argv[2];

  // Загружаем изображения
  Mat image1 = imread(image1_path, IMREAD_GRAYSCALE);
  Mat image2 = imread(image2_path, IMREAD_GRAYSCALE);

  if (image1.empty() || image2.empty()) {
    cerr << "Не удалось загрузить изображения." << endl;
    return 1;
  }

  // Создаем вектор пар алгоритмов и соответствующих хешеров
  vector<pair<string, Ptr<img_hash::ImgHashBase>>> hashers = {
      {"Средний хеш", img_hash::AverageHash::create()},
      {"Перцептивный хеш", img_hash::PHash::create()},
      {"Хеш блочного среднего", img_hash::BlockMeanHash::create()},
      {"Цветовой хеш", img_hash::ColorMomentHash::create()},
      {"Радиальный вариантный хеш", img_hash::RadialVarianceHash::create()}};

  // Сравниваем изображения для каждого алгоритма
  for (const auto& entry : hashers) {
    // Вычисляем хэши для изображений
    Mat hash1, hash2;
    entry.second->compute(image1, hash1);
    entry.second->compute(image2, hash2);

    // Вычисляем расстояние Хэмминга между хэшами
    double similarity = entry.second->compare(hash1, hash2);

    // Выводим результат
    cout << entry.first << ": " << similarity << endl;
  }

  return 0;
}
