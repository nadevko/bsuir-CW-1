#include <filesystem>

#include "main.hh"

template <class Hasher>
class CW1::List {
 private:
  Glib::RefPtr<Gio::File> root;
  std::vector<CW1::iptr<Hasher>> images;
  std::map<std::pair<CW1::iptr<Hasher>, CW1::iptr<Hasher>>, double> hashmap;
  std::vector<std::string> extensions = {
      ".bmp",  ".dib",  ".jpeg", ".jpg", ".jpe", ".jp2", ".png", ".webp",
      ".avif", ".pbm",  ".pgm",  ".ppm", ".pxm", ".pnm", ".pfm", ".sr",
      ".ras",  ".tiff", ".tif",  ".exr", ".hdr", ".pic"};
  formats format;

 public:
  List();
  List(formats format);
  List(Glib::RefPtr<Gio::File> root);
  List(formats format, Glib::RefPtr<Gio::File> root);
  double to_percent(double compare) const;
  std::string to_string() const;
  auto begin() const;
  auto end() const;
  double percentage = 95;
  double max = 0;
};

template <class Hasher>
CW1::List<Hasher>::List() {
  new (this)
      List(formats::none, Gio::File::create_for_path(Glib::get_current_dir()));
}

template <class Hasher>
CW1::List<Hasher>::List(formats format) {
  new (this) List(format, Gio::File::create_for_path(Glib::get_current_dir()));
}

template <class Hasher>
CW1::List<Hasher>::List(Glib::RefPtr<Gio::File> root) {
  new (this) List(formats::none, root);
}

template <class Hasher>
CW1::List<Hasher>::List(formats format, Glib::RefPtr<Gio::File> root)
    : root(root), format(format) {
  std::stack<Glib::RefPtr<Gio::FileEnumerator>> iterators;
  iterators.push(root->enumerate_children());
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
        if (CW1::contains(extensions, extension))
          images.push_back(CW1::Image<Hasher>::from(child));
    }
  }
  std::sort(images.begin(), images.end());
  for (auto i = images.begin(); i != images.end(); i++)
    for (auto j = images.begin(); j != i; j++) {
      auto compare = (*j)->compare(*i);
      if (max < compare) max = compare;
      hashmap[std::make_pair(*j, *i)] = compare;
    }
  for (auto j : hashmap) j.second = to_percent(j.second);
  for (auto i : images)
    for (auto j : hashmap) {
      auto next = to_percent(j.second);
      if ((i == j.first.first || i == j.first.second) && i->percentage < next) {
        i->percentage = next;
        i->max = (i == j.first.first) ? j.first.second : j.first.first;
      }
    }
}

template <class Hasher>
double CW1::List<Hasher>::to_percent(double compare) const {
  return 100 - 100 * compare / this->max;
}

template <class Hasher>
std::string CW1::List<Hasher>::to_string() const {
  std::string result;
  std::vector<CW1::iptr<Hasher>> maxes;
  if (format == formats::sh) result += "#!/bin/sh\n";
  for (auto i = begin(); i != end(); i++) {
    auto isRemove = percentage < (*i)->percentage;
    if (isRemove && CW1::contains(maxes, *i)) isRemove = false;
    std::stringstream ss;
    if (format == formats::sh)
      ss << (isRemove ? "" : "#") << "rm " << (*i)->file->get_path() << " # "
         << root->get_relative_path((*i)->max->file) << " " << (*i)->percentage
         << "%\n";
    else
      ss << (isRemove ? "DELETE " : "LEAVE ")
         << root->get_relative_path((*i)->file)
         << " :: " << root->get_relative_path((*i)->max->file) << " : "
         << (*i)->percentage << "\n";
    maxes.push_back((*i)->max);
    result += ss.str();
  }
  return result;
}

template <class Hasher>
auto CW1::List<Hasher>::begin() const {
  return this->images.begin();
}

template <class Hasher>
auto CW1::List<Hasher>::end() const {
  return this->images.end();
}
