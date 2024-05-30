#pragma once

#include "glibmm/refptr.h"

namespace CW1 {

template <typename T>
using ref = const Glib::RefPtr<T>;

}  // namespace CW1
