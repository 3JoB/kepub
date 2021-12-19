#include "version.h"

#include <fmt/compile.h>
#include <fmt/format.h>
#include <klib/version.h>
#include <mimalloc.h>
#include <simdjson.h>
#include <spdlog/version.h>
#include <unicode/uvernum.h>
#include <CLI/Version.hpp>
#include <boost/version.hpp>
#include <gsl/gsl-lite.hpp>
#include <pugixml.hpp>

namespace kepub {

std::string version_str() {
  std::string result;

  result +=
      fmt::format(FMT_COMPILE("kepub version {}\n"), KEPUB_VERSION_STRING);

  result += "Libraries: ";
  result += fmt::format(FMT_COMPILE("klib/{} "), KLIB_VERSION_STRING);
  result +=
      fmt::format(FMT_COMPILE("mimalloc/{}.{}.{} "), MI_MALLOC_VERSION / 100,
                  MI_MALLOC_VERSION / 10 % 10, MI_MALLOC_VERSION % 10);
  result += fmt::format(FMT_COMPILE("Boost/{}.{}.{} "), BOOST_VERSION / 100000,
                        BOOST_VERSION / 100 % 1000, BOOST_VERSION % 100);
  result += fmt::format(FMT_COMPILE("gsl-lite/{}.{}.{} "), gsl_lite_MAJOR,
                        gsl_lite_MINOR, gsl_lite_PATCH);
  result += fmt::format(FMT_COMPILE("CLI11/{} "), CLI11_VERSION);
  result += fmt::format(FMT_COMPILE("fmt/{}.{}.{} "), FMT_VERSION / 10000,
                        FMT_VERSION / 100 % 100, FMT_VERSION % 100);
  result += fmt::format(FMT_COMPILE("spdlog/{}.{}.{} "), SPDLOG_VER_MAJOR,
                        SPDLOG_VER_MINOR, SPDLOG_VER_PATCH);
  result += fmt::format(
      FMT_COMPILE("simdjson/{}.{}.{} "), simdjson::SIMDJSON_VERSION_MAJOR,
      simdjson::SIMDJSON_VERSION_MINOR, simdjson::SIMDJSON_VERSION_REVISION);
  result +=
      fmt::format(FMT_COMPILE("pugixml/{}.{}.{} "), PUGIXML_VERSION / 1000,
                  PUGIXML_VERSION / 10 % 100, PUGIXML_VERSION % 10);
  result += fmt::format(FMT_COMPILE("ICU/{}.{}.{} "), U_ICU_VERSION_MAJOR_NUM,
                        U_ICU_VERSION_MINOR_NUM, U_ICU_VERSION_PATCHLEVEL_NUM);
  result += "OpenCC/1.1.3 ";
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
