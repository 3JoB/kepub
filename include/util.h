#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <unicode/unistr.h>
#include <unicode/utypes.h>

namespace kepub {

void check_file_exist(const std::string &file_name);

void check_is_txt_file(const std::string &file_name);

void remove_file_or_dir(const std::string &path);

void check_is_book_id(const std::string &book_id);

std::vector<std::string> read_file_to_vec(const std::string &file_name,
                                          bool translation);

void str_check(const std::string &str);

std::int32_t str_size(const std::string &str);

void title_check(const std::string &title);

void push_back(std::vector<std::string> &texts, const std::string &str,
               bool connect_chinese);

void push_back_no_connect(std::vector<std::string> &texts,
                          const std::string &str);

void write_file_if_not_exists(std::string_view path, bool binary,
                              std::string_view content);

void write_file_if_not_exists(std::string_view path, bool binary,
                              const char *str, std::size_t size);

std::string trim(const std::string &str);

void replace_error_char(icu::UnicodeString &str);

void check_icu(UErrorCode status);

std::string get_login_name();

std::string get_password();

std::string num_to_str(std::int32_t i);

void generate_txt(
    const std::string &book_name, const std::string &author,
    const std::vector<std::string> &description,
    const std::vector<std::pair<std::string, std::string>> &chapters);

void generate_txt(
    const std::string &book_name, const std::string &author,
    const std::vector<std::string> &description,
    const std::vector<std::pair<
        std::string,
        std::vector<std::tuple<std::string, std::string, std::string>>>>
        &volume_chapter);

}  // namespace kepub
