#include "image.hh"

CW1::Image::Image(Glib::RefPtr<Gio::File> file) : file(file) {
  // read as hsv
  auto image = cv::imread(file->get_path());
  cv::cvtColor(image, image, cv::COLOR_BGR2HSV);

  // get histogram
  calcHist(&image, 1, channels, cv::Mat(), histogram, 2, histSize, ranges, true,
           false);
  normalize(histogram, histogram, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
}

double CW1::Image::compare(Image b) const {
  return compareHist(this->histogram, b.histogram, cv::HISTCMP_CORREL);
}
