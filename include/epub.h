#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include <pugixml.hpp>

namespace kepub {

struct Chapter {
  std::string title_;
  std::vector<std::string> text_;
};

struct Volume {
  std::string title_;
  std::vector<Chapter> chapters_;
};

struct Novel {
  std::string name_;
  std::string author_;

  std::vector<std::string> introduction_;
  std::int32_t illustration_num_ = 0;
  std::vector<std::string> postscript_;

  std::string cover_path_;
  std::vector<std::string> image_paths_;

  std::vector<Volume> volumes_;
};

class Epub {
 public:
  Epub();

  void set_rights(const std::string &rights) { rights_ = rights; }

  void set_uuid(const std::string &uuid) {
    uuid_ = uuid;
    debug_ = true;
  }
  void set_datetime(const std::string &date) {
    date_ = date;
    debug_ = true;
  }

  void set_novel(const Novel &novel);

  void flush_font(const std::string &book_dir);
  void append();

  void generate();

  constexpr static std::string_view meta_inf_dir = "META-INF";
  constexpr static std::string_view epub_dir = "EPUB";
  constexpr static std::string_view style_dir = "EPUB/css";
  constexpr static std::string_view font_dir = "EPUB/font";
  constexpr static std::string_view image_dir = "EPUB/image";
  constexpr static std::string_view text_dir = "EPUB/text";

  constexpr static std::string_view container_xml_path =
      "META-INF/container.xml";
  constexpr static std::string_view style_css_path = "EPUB/css/style.css";
  constexpr static std::string_view font_woff2_path =
      "EPUB/font/SourceHanSansSC-Bold.woff2";
  constexpr static std::string_view cover_xhtml_path = "EPUB/text/cover.xhtml";
  constexpr static std::string_view introduction_xhtml_path =
      "EPUB/text/introduction.xhtml";
  constexpr static std::string_view postscript_xhtml_path =
      "EPUB/text/postscript.xhtml";
  constexpr static std::string_view nav_xhtml_path = "EPUB/nav.xhtml";
  constexpr static std::string_view package_opf_path = "EPUB/package.opf";
  constexpr static std::string_view mimetype_path = "mimetype";

 private:
  void generate_container() const;
  void generate_style() const;
  void generate_font(bool flush_font_words = true);
  void generate_image() const;
  void generate_volume() const;
  void generate_chapter() const;
  void generate_cover() const;
  void generate_illustration() const;
  void generate_introduction() const;
  void generate_postscript() const;
  void generate_nav() const;
  void generate_package() const;
  void generate_mimetype() const;

  void do_deal_with_nav(pugi::xml_node &ol, std::int32_t first_volume_id,
                        std::int32_t first_chapter_id) const;
  void deal_with_nav(std::int32_t first_volume_id,
                     std::int32_t first_chapter_id) const;
  void do_deal_with_package(pugi::xml_node &manifest,
                            std::int32_t first_chapter_id,
                            std::int32_t first_volume_id) const;
  [[nodiscard]] std::pair<std::int32_t, std::int32_t> deal_with_package() const;
  void deal_with_volume(std::int32_t first_volume_id) const;
  void deal_with_chapter(std::int32_t first_chapter_id) const;

  bool ready_ = false;

  std::string rights_;

  std::string uuid_;
  std::string date_;

  Novel novel_;

  std::string_view style_;
  std::string_view font_;

  std::string font_words_ = "封面彩页简介后记0123456789";
  bool debug_ = false;
};

}  // namespace kepub
