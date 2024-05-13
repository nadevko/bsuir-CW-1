#include <gtkmm.h>

#include <compare>
#include <concepts>
#include <opencv2/img_hash/img_hash_base.hpp>
#include <opencv2/opencv.hpp>

#include "main.hh"

template <class Hasher>
class CW1::Image {
  static_assert(std::derived_from<Hasher, cv::img_hash::ImgHashBase> == true);
  cv::Mat _hash;

 public:
  Image(Glib::RefPtr<Gio::File> file);
  Image(std::string path);
  Glib::RefPtr<Gio::File> file;
  cv::Mat hash();
  double compare(Image& image);
  std::strong_ordering operator<=>(const Image& b) const;
};

template <class Hasher>
CW1::Image<Hasher>::Image(Glib::RefPtr<Gio::File> file) {
  this->file = file;
}

template <class Hasher>
CW1::Image<Hasher>::Image(std::string path) {
  auto file = Gio::File::create_for_path(path);
  new (this) Image(file);
}

template <class Hasher>
cv::Mat CW1::Image<Hasher>::hash() {
  if (!_hash.empty()) return _hash;
  cv::Ptr<Hasher> hasher = Hasher::create();
  auto image = cv::imread(file->get_path());
  hasher->compute(image, _hash);
  return _hash;
}

template <class Hasher>
double CW1::Image<Hasher>::compare(Image& b) {
  cv::Ptr<Hasher> hasher = Hasher::create();
  return hasher->compare(hash(), b.hash());
}

template <class Hasher>
std::strong_ordering CW1::Image<Hasher>::operator<=>(const Image& b) const {
  return this->file->get_path() <=> b.file->get_path();
}
