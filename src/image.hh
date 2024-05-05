#include <gtkmm.h>

#include <opencv2/opencv.hpp>

#include "giomm/file.h"
#include "glibmm/refptr.h"
#include "main.hh"

class CW1::Image {
 private:
  const int histSize[2] = {50, 60};
  const float h_ranges[2] = {0, 180};
  const float s_ranges[2] = {0, 56};
  const float* ranges[2] = {h_ranges, s_ranges};
  const int channels[2] = {0, 1};

 public:
  Image(Glib::RefPtr<Gio::File> file);
  Glib::RefPtr<Gio::File> file;
  cv::Mat histogram;
  double compare(Image b) const;
};
