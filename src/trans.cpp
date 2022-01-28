#include "trans.h"

#include <klib/util.h>
#include <opencc.h>
#include <unicode/translit.h>
#include <unicode/unistr.h>
#include <unicode/utypes.h>

#include "util.h"

extern char tw2s[];
extern int tw2s_size;

extern char TSPhrases[];
extern int TSPhrases_size;

extern char TWVariantsRevPhrases[];
extern int TWVariantsRevPhrases_size;

extern char TWVariantsRev[];
extern int TWVariantsRev_size;

extern char TSCharacters[];
extern int TSCharacters_size;

namespace kepub {

namespace {

class Trans {
 public:
  ~Trans();

  Trans(const Trans &) = delete;
  Trans(Trans &&) = delete;
  Trans &operator=(const Trans &) = delete;
  Trans &operator=(Trans &&) = delete;

  static const Trans &get();
  [[nodiscard]] std::string trans_str(std::string_view str,
                                      bool translation) const;

 private:
  Trans();

  icu::Transliterator *fullwidth_halfwidth_;
};

void replace_all_punct(icu::UnicodeString &str, const std::string &old_text,
                       std::string_view new_text) {
  std::string space = " ";
  auto new_text_str = icu::UnicodeString::fromUTF8(new_text);

  str.findAndReplace((space + old_text).c_str(), new_text_str);
  str.findAndReplace((old_text + space).c_str(), new_text_str);
  str.findAndReplace(old_text.c_str(), new_text_str);
}

void replace_all_multi(icu::UnicodeString &str, const std::string &text) {
  auto search_text = icu::UnicodeString::fromUTF8(text + text);
  auto new_text = icu::UnicodeString::fromUTF8(text);

  while (str.indexOf(search_text) != -1) {
    str.findAndReplace(search_text, new_text);
  }
}

void custom_trans(icu::UnicodeString &str) {
  replace_error_char(str);

  replace_all_multi(str, " ");

  str.findAndReplace("⋯", "…");
  str.findAndReplace("─", "—");

  // https://zh.wikipedia.org/wiki/%E5%85%A8%E5%BD%A2%E5%92%8C%E5%8D%8A%E5%BD%A2
  replace_all_punct(str, "“", "“");
  replace_all_punct(str, "”", "”");
  replace_all_punct(str, "｢", "「");
  replace_all_punct(str, "｣", "」");
  replace_all_punct(str, "『", "『");
  replace_all_punct(str, "』", "』");
  replace_all_punct(str, "《", "《");
  replace_all_punct(str, "》", "》");
  replace_all_punct(str, "【", "【");
  replace_all_punct(str, "】", "】");
  replace_all_punct(str, "(", "（");
  replace_all_punct(str, ")", "）");
  replace_all_punct(str, "…", "…");

  replace_all_punct(str, ",", "，");
  replace_all_punct(str, ":", "：");
  replace_all_punct(str, ";", "；");
  replace_all_punct(str, "｡", "。");
  replace_all_punct(str, "､", "、");
  replace_all_punct(str, "!", "！");
  replace_all_punct(str, "?", "？");
  replace_all_punct(str, "ｰ", "ー");
  replace_all_punct(str, "･", "・");
  replace_all_punct(str, "~", "～");

  replace_all_punct(str, "♂", "♂");
  replace_all_punct(str, "♀", "♀");
  replace_all_punct(str, "◇", "◇");
  replace_all_punct(str, "￮", "￮");
  replace_all_punct(str, "+", "+");
  replace_all_punct(str, "=", "=");
  replace_all_punct(str, "￪", "↑");
  replace_all_punct(str, "￬", "↓");
  replace_all_punct(str, "￩", "←");
  replace_all_punct(str, "￫", "→");

  replace_all_multi(str, "，");
  replace_all_multi(str, "。");
  replace_all_multi(str, "、");

  str.findAndReplace("妳", "你");
  str.findAndReplace("壊", "坏");
  str.findAndReplace("拚", "拼");
  str.findAndReplace("噁", "恶");
  str.findAndReplace("歳", "岁");
  str.findAndReplace("経", "经");
  str.findAndReplace("験", "验");
  str.findAndReplace("険", "险");
  str.findAndReplace("撃", "击");
  str.findAndReplace("錬", "炼");
  str.findAndReplace("隷", "隶");
  str.findAndReplace("毎", "每");
  str.findAndReplace("捩", "折");
  str.findAndReplace("殻", "壳");
  str.findAndReplace("牠", "它");
  str.findAndReplace("矇", "蒙");
  str.findAndReplace("髮", "发");
  str.findAndReplace("姊", "姐");
  str.findAndReplace("黒", "黑");
  str.findAndReplace("歴", "历");
  str.findAndReplace("様", "样");
  str.findAndReplace("甦", "苏");
  str.findAndReplace("牴", "抵");
  str.findAndReplace("銀", "银");
  str.findAndReplace("齢", "龄");
  str.findAndReplace("従", "从");
  str.findAndReplace("酔", "醉");
  str.findAndReplace("値", "值");
  str.findAndReplace("発", "发");
  str.findAndReplace("続", "续");
  str.findAndReplace("転", "转");
  str.findAndReplace("剣", "剑");
  str.findAndReplace("砕", "碎");
  str.findAndReplace("鉄", "铁");
  str.findAndReplace("甯", "宁");
  str.findAndReplace("鬪", "斗");
  str.findAndReplace("寛", "宽");
  str.findAndReplace("変", "变");
  str.findAndReplace("鳮", "鸡");
  str.findAndReplace("悪", "恶");
  str.findAndReplace("霊", "灵");
  str.findAndReplace("戦", "战");
  str.findAndReplace("権", "权");
  str.findAndReplace("効", "效");
  str.findAndReplace("応", "应");
  str.findAndReplace("覚", "觉");
  str.findAndReplace("観", "观");
  str.findAndReplace("気", "气");
  str.findAndReplace("覧", "览");
  str.findAndReplace("殭", "僵");
  str.findAndReplace("郞", "郎");
  str.findAndReplace("虊", "药");
  str.findAndReplace("踼", "踢");
  str.findAndReplace("逹", "达");
  str.findAndReplace("鑜", "锁");
  str.findAndReplace("髲", "发");
  str.findAndReplace("髪", "发");
  str.findAndReplace("実", "实");
  str.findAndReplace("內", "内");
  str.findAndReplace("穨", "颓");
  str.findAndReplace("糸", "系");
  str.findAndReplace("賍", "赃");
  str.findAndReplace("掦", "扬");
  str.findAndReplace("覇", "霸");
  str.findAndReplace("姉", "姐");
  str.findAndReplace("楽", "乐");
  str.findAndReplace("継", "继");
  str.findAndReplace("隠", "隐");
  str.findAndReplace("巻", "卷");
  str.findAndReplace("膞", "膊");
  str.findAndReplace("髑", "骷");
  str.findAndReplace("劄", "札");
  str.findAndReplace("擡", "抬");
  str.findAndReplace("⼈", "人");
  str.findAndReplace("⾛", "走");
  str.findAndReplace("⼤", "大");
  str.findAndReplace("⽤", "用");
  str.findAndReplace("⼿", "手");
  str.findAndReplace("⼦", "子");
  str.findAndReplace("⽽", "而");
  str.findAndReplace("⾄", "至");
  str.findAndReplace("⽯", "石");
  str.findAndReplace("⼗", "十");
  str.findAndReplace("⽩", "白");
  str.findAndReplace("⽗", "父");
  str.findAndReplace("⽰", "示");
  str.findAndReplace("⾁", "肉");
  str.findAndReplace("⼠", "士");
  str.findAndReplace("⽌", "止");
  str.findAndReplace("⼀", "一");
  str.findAndReplace("⺠", "民");
  str.findAndReplace("揹", "背");
  str.findAndReplace("镳", "镖");
  str.findAndReplace("佈", "布");
  str.findAndReplace("勐", "猛");
  str.findAndReplace("嗳", "哎");
  str.findAndReplace("纔", "才");
  str.findAndReplace("繄", "紧");
  str.findAndReplace("勧", "劝");
  str.findAndReplace("鐡", "铁");
  str.findAndReplace("犠", "牺");
  str.findAndReplace("繊", "纤");
  str.findAndReplace("郷", "乡");
  str.findAndReplace("亊", "事");
  str.findAndReplace("騒", "骚");
  str.findAndReplace("聡", "聪");
  str.findAndReplace("遅", "迟");
  str.findAndReplace("唖", "哑");
  str.findAndReplace("獣", "兽");
  str.findAndReplace("読", "读");
  str.findAndReplace("囙", "因");
  str.findAndReplace("寘", "置");
  str.findAndReplace("対", "对");
  str.findAndReplace("処", "处");
  str.findAndReplace("団", "团");
  str.findAndReplace("祢", "你");
  str.findAndReplace("閙", "闹");
  str.findAndReplace("谘", "咨");
  str.findAndReplace("摀", "捂");
  str.findAndReplace("類", "类");
  str.findAndReplace("諷", "讽");
  str.findAndReplace("唿", "呼");
  str.findAndReplace("噹", "当");
  str.findAndReplace("沒", "没");
  str.findAndReplace("別", "别");
  str.findAndReplace("歿", "殁");
  str.findAndReplace("羅", "罗");
  str.findAndReplace("給", "给");
  str.findAndReplace("頽", "颓");
  str.findAndReplace("來", "来");
  str.findAndReplace("裝", "装");
  str.findAndReplace("燈", "灯");
  str.findAndReplace("蓋", "盖");
  str.findAndReplace("迴", "回");
  str.findAndReplace("單", "单");
  str.findAndReplace("勢", "势");
  str.findAndReplace("結", "结");
  str.findAndReplace("砲", "炮");
  str.findAndReplace("採", "采");
  str.findAndReplace("財", "财");
  str.findAndReplace("頂", "顶");
  str.findAndReplace("倆", "俩");

  str.findAndReplace("幺", "么");
  str.findAndReplace("颠复", "颠覆");

  str.findAndReplace("赤果果", "赤裸裸");
  str.findAndReplace("赤果", "赤裸");
}

Trans::~Trans() { delete fullwidth_halfwidth_; }

const Trans &Trans::get() {
  static Trans trans;
  return trans;
}

std::string Trans::trans_str(std::string_view str, bool translation) const {
  std::string std_str(str);
  if (translation) {
    const static opencc::SimpleConverter converter("/tmp/tw2s.json");
    std_str = converter.Convert(std_str);
  }

  auto icu_str = icu::UnicodeString::fromUTF8(std_str.c_str());
  fullwidth_halfwidth_->transliterate(icu_str);
  custom_trans(icu_str);

  icu_str.trim();

  std::string temp;
  return icu_str.toUTF8String(temp);
}

Trans::Trans() {
  klib::write_file("/tmp/tw2s.json", false, tw2s, tw2s_size);
  klib::write_file("/tmp/TSPhrases.ocd2", true, TSPhrases, TSPhrases_size);
  klib::write_file("/tmp/TWVariantsRevPhrases.ocd2", true, TWVariantsRevPhrases,
                   TWVariantsRevPhrases_size);
  klib::write_file("/tmp/TWVariantsRev.ocd2", true, TWVariantsRev,
                   TWVariantsRev_size);
  klib::write_file("/tmp/TSCharacters.ocd2", true, TSCharacters,
                   TSCharacters_size);

  UErrorCode status = U_ZERO_ERROR;
  fullwidth_halfwidth_ = icu::Transliterator::createInstance(
      "Fullwidth-Halfwidth", UTRANS_FORWARD, status);
  check_icu(status);
}

}  // namespace

std::string trans_str(const char *str, bool translation) {
  return trans_str(std::string_view(str), translation);
}

std::string trans_str(std::string_view str, bool translation) {
  return Trans::get().trans_str(str, translation);
}

std::string trans_str(const std::string &str, bool translation) {
  return trans_str(std::string_view(str), translation);
}

}  // namespace kepub
