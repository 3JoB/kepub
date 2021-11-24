#include "version.h"

#include <Python.h>
#include <fmt/compile.h>
#include <fmt/format.h>
#include <klib/version.h>
#include <simdjson.h>
#include <spdlog/version.h>
#include <unicode/uvernum.h>
#include <CLI/Version.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/version.hpp>
#include <pugixml.hpp>
#include <scope_guard.hpp>

namespace kepub {

namespace {

std::string get_python_version() {
  std::string str = Py_GetVersion();

  auto index = str.find('(');
  return boost::trim_copy(str.substr(0, index));
}

}  // namespace

std::string version_str() {
  std::string result;

  result +=
      fmt::format(FMT_COMPILE("kepub version {}\n"), KEPUB_VERSION_STRING);

  result += "Libraries: ";
  result += fmt::format(FMT_COMPILE("klib/{} "), KLIB_VERSION_STRING);
  result += fmt::format(FMT_COMPILE("Boost/{}.{}.{} "), BOOST_VERSION / 100000,
                        BOOST_VERSION / 100 % 1000, BOOST_VERSION % 100);
  result += fmt::format(FMT_COMPILE("CLI11/{} "), CLI11_VERSION);
  result += fmt::format(FMT_COMPILE("fmt/{}.{}.{} "), FMT_VERSION / 10000,
                        FMT_VERSION / 100 % 100, FMT_VERSION % 100);
  result += fmt::format(FMT_COMPILE("spdlog/{}.{}.{} "), SPDLOG_VER_MAJOR,
                        SPDLOG_VER_MINOR, SPDLOG_VER_PATCH);
  result += fmt::format(FMT_COMPILE("scope_guard/{}.{}.{} "),
                        SCOPE_GUARD_VERSION_MAJOR, SCOPE_GUARD_VERSION_MINOR,
                        SCOPE_GUARD_VERSION_PATCH);
  result += fmt::format(
      FMT_COMPILE("simdjson/{}.{}.{} "), simdjson::SIMDJSON_VERSION_MAJOR,
      simdjson::SIMDJSON_VERSION_MINOR, simdjson::SIMDJSON_VERSION_REVISION);
  result +=
      fmt::format(FMT_COMPILE("pugixml/{}.{}.{} "), PUGIXML_VERSION / 1000,
                  PUGIXML_VERSION / 10 % 100, PUGIXML_VERSION % 10);
  result += fmt::format(FMT_COMPILE("ICU/{}.{}.{} "), U_ICU_VERSION_MAJOR_NUM,
                        U_ICU_VERSION_MINOR_NUM, U_ICU_VERSION_PATCHLEVEL_NUM);
  result += fmt::format(FMT_COMPILE("CPython/{} "), get_python_version());
  // NOTE
  result += "indicators/2.2.0\n";

  result += "Compiler: ";
#if defined(__clang_version__)
  result.append("Clang ").append(__clang_version__);
#elif defined(__GNUG__)
  result.append("GCC ").append(__VERSION__);
#else
  result.append("Unknown compiler");
#endif
  result += "\n";

  result.append("Build time: ").append(__DATE__).append(" ").append(__TIME__);

  return result;
}

}  // namespace kepub
