#include <cstddef>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>

#include <klib/archive.h>
#include <klib/error.h>
#include <spdlog/spdlog.h>
#include <CLI/CLI.hpp>

#include "epub.h"
#include "trans.h"
#include "util.h"
#include "version.h"

int main(int argc, const char *argv[]) try {
  CLI::App app;
  app.set_version_flag("-v,--version", kepub::version_str());

  std::string file_name;
  app.add_option("file", file_name, "TXT file to be processed")->required();

  bool translation = false;
  app.add_flag("-t,--translation", translation,
               "Translate Traditional Chinese to Simplified Chinese");

  bool connect_chinese = false;
  app.add_flag("-c,--connect", connect_chinese,
               "Remove extra line breaks between Chinese");

  bool no_cover = false;
  app.add_flag("--no-cover", no_cover, "Do not generate cover");

  std::int32_t illustration_num = 0;
  app.add_option("-i,--illustration", illustration_num,
                 "Generate illustration");

  std::int32_t image_num = 0;
  app.add_option("--image", image_num,
                 "Specify the number of generated images");

  bool generate_postscript = false;
  app.add_flag("-p,--postscript", generate_postscript, "Generate postscript");

  bool remove = false;
  app.add_flag("-r,--remove", remove, "Remove txt files and possible images");

  std::string uuid;
  app.add_option("--uuid", uuid, "Specify the uuid(for testing)");

  std::string date;
  app.add_option("--date", date, "Specify the date(for testing)");

  bool no_compress = false;
  app.add_flag("--no-compress", no_compress, "Do not compress(for testing)");

  CLI11_PARSE(app, argc, argv)

  kepub::check_is_txt_file(file_name);
  auto book_name =
      kepub::trans_str(std::filesystem::path(file_name).stem(), translation);
  spdlog::info("Book name: {}", book_name);

  kepub::Epub epub(argv[0]);
  epub.set_creator("kaiser");
  epub.set_book_name(book_name);
  epub.set_generate_cover(!no_cover);
  epub.set_illustration_num(illustration_num);
  epub.set_image_num(image_num);
  epub.set_generate_postscript(generate_postscript);
  // For testing
  if (!std::empty(uuid)) {
    epub.set_uuid(uuid);
  }
  if (!std::empty(date)) {
    epub.set_date(date);
  }

  auto vec = kepub::read_file_to_vec(file_name, translation);
  auto size = std::size(vec);

  std::string title_prefix = "[WEB] ";
  auto title_prefix_size = std::size(title_prefix);

  std::string volume_name;
  std::string volume_prefix = "[VOLUME] ";
  auto volume_prefix_size = std::size(volume_prefix);

  std::string author;
  std::string author_prefix = "[AUTHOR]";

  std::vector<std::string> introduction;
  std::string introduction_prefix = "[INTRO]";

  std::vector<std::string> postscript;
  std::string postscript_prefix = "[POST]";

  std::int32_t word_count = 0;

  auto is_prefix = [&](const std::string &line) {
    return line.starts_with(author_prefix) ||
           line.starts_with(introduction_prefix) ||
           line.starts_with(postscript_prefix) ||
           line.starts_with(title_prefix) || line.starts_with(volume_prefix);
  };

  for (std::size_t i = 0; i < size; ++i) {
    if (vec[i].starts_with(author_prefix)) {
      ++i;

      if (!std::empty(author)) {
        klib::warn("Author has been defined");
      }

      author = vec[i];
    } else if (vec[i].starts_with(introduction_prefix)) {
      ++i;

      if (!std::empty(introduction)) {
        klib::warn("Introduction has been defined");
        introduction.clear();
      }

      for (; i < size && !is_prefix(vec[i]); ++i) {
        kepub::push_back(introduction, vec[i], connect_chinese);
      }
      --i;
    } else if (vec[i].starts_with(postscript_prefix)) {
      ++i;

      if (!std::empty(postscript)) {
        klib::warn("Postscript has been defined");
        postscript.clear();
      }

      for (; i < size && !is_prefix(vec[i]); ++i) {
        kepub::push_back(postscript, vec[i], connect_chinese);
      }
      --i;
    } else if (vec[i].starts_with(volume_prefix)) {
      volume_name = vec[i].substr(volume_prefix_size);
    } else if (vec[i].starts_with(title_prefix)) {
      auto title = vec[i].substr(title_prefix_size);
      kepub::title_check(title);
      ++i;

      std::vector<std::string> content;
      for (; i < size && !is_prefix(vec[i]); ++i) {
        auto line = vec[i];
        kepub::str_check(line);
        word_count += kepub::str_size(line);
        kepub::push_back(content, line, connect_chinese);
      }
      --i;

      epub.add_content(volume_name, title, content);
    }
  }

  if (!std::empty(author)) {
    spdlog::info("Author: {}", author);
    epub.set_author(author);
  }
  if (!std::empty(introduction)) {
    epub.set_introduction(introduction);
  }
  if (generate_postscript && !std::empty(postscript)) {
    epub.set_postscript(postscript);
  }

  bool postscript_done =
      !generate_postscript || (generate_postscript && !std::empty(postscript));

  epub.generate();
  if (remove) {
    std::filesystem::remove(file_name);
  }
  spdlog::info("Total words: {}", word_count);

  bool cover_done = true;
  if (!no_cover) {
    std::string cover_name = "cover.jpg";

    if (!std::filesystem::exists(cover_name)) {
      klib::warn("No cover");
      cover_done = false;
    } else {
      std::filesystem::copy(cover_name, std::filesystem::path(book_name) /
                                            kepub::Epub::images_dir /
                                            cover_name);
      if (remove) {
        std::filesystem::remove(cover_name);
      }
    }
  }

  bool image_done = true;
  if (image_num != 0) {
    for (std::int32_t i = 1; i <= image_num; ++i) {
      auto jpg_name = kepub::num_to_str(i) + ".jpg";

      if (!std::filesystem::exists(jpg_name)) {
        klib::warn("Incorrect number of image");
        image_done = false;
        break;
      }

      std::filesystem::copy(jpg_name, std::filesystem::path(book_name) /
                                          kepub::Epub::images_dir / jpg_name);
      if (remove) {
        std::filesystem::remove(jpg_name);
      }
    }
  }

  bool book_done = !std::empty(author) && !std::empty(introduction) &&
                   postscript_done && cover_done && image_done;

  if (!no_compress && book_done) {
    spdlog::info("Start to compress and generate epub files");
    klib::compress(book_name, klib::Algorithm::Zip, book_name + ".epub", false);
    std::filesystem::remove_all(book_name);
    spdlog::info("{} generate epub complete", book_name);
  } else {
    spdlog::info(
        "An element is missing or an error occurs, and no compression is "
        "performed");
  }
} catch (const std::exception &err) {
  klib::error(KLIB_CURR_LOC, err.what());
} catch (...) {
  klib::error(KLIB_CURR_LOC, "Unknown exception");
}