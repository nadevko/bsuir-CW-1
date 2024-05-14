#pragma once
#include <gtkmm.h>

namespace CW1 {
// classes
class Application;
template <class Hasher>
class Image;
template <class Hasher>
class List;
// templates
template <class C, class T>
inline bool contains(C container, const T& value);
// type aliases
template <class Hasher>
using iptr = Glib::RefPtr<Image<Hasher>>;
template <class Hasher>
using lptr = Glib::RefPtr<List<Hasher>>;
}  // namespace CW1

template <class C, class T>
inline bool CW1::contains(C container, const T& value) {
  return std::find(container.begin(), container.end(), value) !=
         container.end();
}
