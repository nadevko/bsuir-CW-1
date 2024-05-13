#include <filesystem>
#include <utility>

#include "main.hh"

template <class Hasher>
class CW1::List {
 private:
  Glib::RefPtr<Gio::File> root;
  std::vector<CW1::Image<Hasher>> images;
  std::map<std::pair<CW1::Image<Hasher>, CW1::Image<Hasher>>, double> hashmap;

 public:
  List();
  List(Glib::RefPtr<Gio::File> root);
  void set_root(Glib::RefPtr<Gio::File> root);
  Glib::RefPtr<Gio::File> get_root();
  auto begin() const;
  auto end() const;
  double max = 0;
  double to_percent(double compare) const;
};

template <class Hasher>
CW1::List<Hasher>::List() {
  new (this) List(Gio::File::create_for_path(Glib::get_current_dir()));
}

template <class Hasher>
CW1::List<Hasher>::List(Glib::RefPtr<Gio::File> root) {
  set_root(root);
}

template <class Hasher>
void CW1::List<Hasher>::set_root(Glib::RefPtr<Gio::File> root) {
  std::stack<Glib::RefPtr<Gio::FileEnumerator>> iterators;
  iterators.push(root->enumerate_children());
  images = {};
  while (!iterators.empty()) {
    auto info = iterators.top()->next_file();
    if (!info) {
      iterators.pop();
      continue;
    }
    auto child = iterators.top()->get_child(info);
    switch (info->get_file_type()) {
      case Gio::FileType::DIRECTORY:
        iterators.push(child->enumerate_children());
      default:
        auto extension =
            std::filesystem::path(child->get_path()).extension().string();
        std::vector<std::string> extensions = {
            ".bmp",  ".dib",  ".jpeg", ".jpg", ".jpe", ".jp2", ".png", ".webp",
            ".avif", ".pbm",  ".pgm",  ".ppm", ".pxm", ".pnm", ".pfm", ".sr",
            ".ras",  ".tiff", ".tif",  ".exr", ".hdr", ".pic"};
        if (CW1::contains(extensions, extension))
          images.push_back(CW1::Image<Hasher>(child));
    }
  }
  hashmap = {};
  std::sort(images.begin(), images.end());
  for (auto i = images.begin(); i != images.end(); i++)
    for (auto j = images.begin(); j != i; j++) {
      auto compare = j->compare(*i);
      if (max < compare) max = compare;
      hashmap.emplace(std::make_pair(std::make_pair(*j, *i), compare));
    }
}

template <class Hasher>
Glib::RefPtr<Gio::File> CW1::List<Hasher>::get_root() {
  return root;
}

template <class Hasher>
auto CW1::List<Hasher>::begin() const {
  return this->hashmap.begin();
}

template <class Hasher>
auto CW1::List<Hasher>::end() const {
  return this->hashmap.end();
}

template <class Hasher>
double CW1::List<Hasher>::to_percent(double compare) const {
  return 100 - 100 * compare / this->max;
}
