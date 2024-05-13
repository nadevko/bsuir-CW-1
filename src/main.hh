#pragma once
#include <gtkmm.h>

namespace CW1 {
class Application;
template <class Hasher>
class Image;
template <class Hasher>
using iptr = Glib::RefPtr<Image<Hasher>>;
template <class Hasher>
class List;
template <class Hasher>
using lptr = Glib::RefPtr<List<Hasher>>;
template <class C, class T>
inline bool contains(C container, const T& value);
}  // namespace CW1

template <class C, class T>
inline bool CW1::contains(C container, const T& value) {
  return std::find(container.begin(), container.end(), value) !=
         container.end();
}
