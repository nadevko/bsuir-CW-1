#include <gtkmm.h>

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "image.hh"

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
        maxPathLength >= images.back().size() ?: images.back().size();
  }

  for (const auto& path : images) {
    try {
      auto pixbuf = Gdk::Pixbuf::create_from_file(path);
      CW1::Image image(pixbuf);
      uint64_t hash = image.get_rvhash();
      std::cout << std::setw(maxPathLength) << std::left << path << " | "
                << hash << '\n';
    } catch (const Glib::FileError& ex) {
      std::cerr << "Error loading image " << path << ": " << ex.what() << '\n';
    }
  }

  return 0;
}
