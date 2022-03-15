#pragma once

#include <string>

#include "config.h"

#define KEPUB_VER_MAJOR 2

#define KEPUB_VER_MINOR 2

#define KEPUB_VER_PATCH 2

#define KEPUB_STRINGIZE2(s) #s
#define KEPUB_STRINGIZE(s) KEPUB_STRINGIZE2(s)

#define KEPUB_VERSION_STRING       \
  KEPUB_STRINGIZE(KEPUB_VER_MAJOR) \
  "." KEPUB_STRINGIZE(KEPUB_VER_MINOR) "." KEPUB_STRINGIZE(KEPUB_VER_PATCH)

namespace kepub {

std::string KEPUB_PUBLIC version_str();

}  // namespace kepub
