#include <clocale>
#include <ctime>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include <fmt/compile.h>
#include <fmt/format.h>
#include <klib/error.h>
#include <klib/hash.h>
#include <klib/http.h>
#include <klib/util.h>
#include <spdlog/spdlog.h>
#include <CLI/CLI.hpp>
#include <boost/algorithm/string.hpp>

#include "json.h"
#include "progress_bar.h"
#include "trans.h"
#include "util.h"
#include "version.h"

namespace {

const std::string authorization =
    "Basic YW5kcm9pZHVzZXI6MWEjJDUxLXl0Njk7KkFjdkBxeHE=";
// TODO Updated version
const std::string user_agent = "boluobao/4.3.20(android;22)/HomePage";
const std::string device_token = "AAC3B586-D131-32DE-942C-F5CCED55B45E";

std::string sf_security() {
  std::string uuid = boost::to_upper_copy(klib::uuid());
  auto timestamp = std::time(nullptr);
  std::string sign = boost::to_upper_copy(klib::md5_hex(
      uuid + std::to_string(timestamp) + device_token + "xw3#a12-x"));

  return fmt::format(
      FMT_COMPILE("nonce={}&timestamp={}&devicetoken={}&sign={}"), uuid,
      timestamp, device_token, sign);
}

klib::Response http_get(
    const std::string &url,
    const std::unordered_map<std::string, std::string> &params = {}) {
  static klib::Request request;
  request.set_no_proxy();
  request.set_user_agent(user_agent);
#ifndef NDEBUG
  request.verbose(true);
#endif

  return request.get(
      url, params,
      {{"Authorization", authorization}, {"SFSecurity", sf_security()}});
}

klib::Response http_post(const std::string &url, const std::string &json) {
  klib::Request request;
  request.set_no_proxy();
  request.set_user_agent(user_agent);
#ifndef NDEBUG
  request.verbose(true);
#endif

  return request.post(
      url, json,
      {{"Authorization", authorization}, {"SFSecurity", sf_security()}});
}

bool show_user_info() {
  auto response = http_get("https://api.sfacg.com/user");
  UserInfo info(response.text());

  if (info.login_expired()) {
    return false;
  } else {
    spdlog::info("Use existing cookies, nick name: {}", info.nick_name());
    return true;
  }
}

void login(const std::string &login_name, const std::string &password) {
  http_post("https://api.sfacg.com/sessions", serialize(login_name, password));

  auto response = http_get("https://api.sfacg.com/user");
  LoginInfo info(response.text());
  spdlog::info("Login successful, nick name: {}", info.nick_name());
}

std::tuple<std::string, std::string, std::vector<std::string>> get_book_info(
    const std::string &book_id) {
  auto response = http_get("https://api.sfacg.com/novels/" + book_id,
                           {{"expand", "intro"}});
  BookInfo info(response.text());

  spdlog::info("Book name: {}", info.book_name());
  spdlog::info("Author: {}", info.author());
  spdlog::info("Cover url: {}", info.cover_url());

  std::string cover_name = "cover.jpg";
  response = http_get(info.cover_url());
  response.save_to_file(cover_name, true);
  spdlog::info("Cover downloaded successfully: {}", cover_name);

  return {info.book_name(), info.author(), info.intro()};
}

std::vector<
    std::pair<std::string,
              std::vector<std::tuple<std::string, std::string, std::string>>>>
get_volume_chapter(const std::string &book_id) {
  auto response = http_get(fmt::format(
      FMT_COMPILE("https://api.sfacg.com/novels/{}/dirs"), book_id));
  return VolumeChapter(response.text()).get_volume_chapter();
}

std::vector<std::string> get_content(const std::string &chapter_id) {
  auto response = http_get("https://api.sfacg.com/Chaps/" + chapter_id,
                           {{"expand", "content"}});

  auto content_str = Content(response.text()).content();

  static std::int32_t image_count = 1;
  std::vector<std::string> content;
  for (auto &line : klib::split_str(content_str, "\n")) {
    line = kepub::trans_str(line, false);

    if (line.starts_with("[img")) {
      auto begin = line.find("https");
      if (begin == std::string::npos) {
        klib::error("No image url");
      }

      auto end = line.find("[/img]");
      if (end == std::string::npos) {
        klib::error("No image url");
      }

      auto image_url = line.substr(begin, end - begin);
      boost::replace_all(image_url, "：", ":");

      auto image = http_get(image_url);
      auto image_name = kepub::num_to_str(image_count++);
      image.save_to_file(image_name + ".jpg", true);

      line = "[IMAGE] " + image_name;

      content.push_back(line);
    } else {
      kepub::push_back(content, line, false);
    }
  }

  return content;
}

}  // namespace

int main(int argc, const char *argv[]) try {
  std::setlocale(LC_ALL, "en_US.UTF-8");

  CLI::App app;
  app.set_version_flag("-v,--version", kepub::version_str());

  std::string book_id;
  app.add_option("book-id", book_id, "The book id of the book to be downloaded")
      ->required();

  CLI11_PARSE(app, argc, argv)

  kepub::check_is_book_id(book_id);

  if (!show_user_info()) {
    auto login_name = kepub::get_login_name();
    auto password = kepub::get_password();
    login(login_name, password);
    klib::cleanse(password);
  }

  auto [book_name, author, description] = get_book_info(book_id);
  auto volume_chapter = get_volume_chapter(book_id);

  std::int32_t chapter_count = 0;
  for (const auto &[volume_name, chapters] : volume_chapter) {
    chapter_count += std::size(chapters);
  }

  kepub::ProgressBar bar(book_name, chapter_count);
  for (auto &[volume_name, chapters] : volume_chapter) {
    for (auto &[chapter_id, chapter_title, content] : chapters) {
      bar.set_postfix_text(chapter_title);
      content = boost::join(get_content(chapter_id), "\n");
      bar.tick();
    }
  }

  kepub::generate_txt(book_name, author, description, volume_chapter);
} catch (const std::exception &err) {
  klib::error(err.what());
} catch (...) {
  klib::error("Unknown exception");
}