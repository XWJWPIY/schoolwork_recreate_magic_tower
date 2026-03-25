#ifndef NUMERIC_DISPLAY_TEXT_HPP
#define NUMERIC_DISPLAY_TEXT_HPP

#include "Util/GameObject.hpp"
#include "Util/Text.hpp"
#include <string>

class NumericDisplayText : public Util::GameObject {
public:
  NumericDisplayText(const std::string &fontPath, int fontSize);

  void SetPrefix(const std::string &prefix);
  void SetSuffix(const std::string &suffix);
  void SetNumber(int number);

  void SetShowNumber(bool show);
  void SetShowText(bool show);

  std::string GetPrefix() const { return m_prefix; }
  int GetNumber() const { return m_number; }

  void SetColor(const Util::Color &color);
  void UpdateDisplayText();

private:
  std::shared_ptr<Util::Text> m_text_drawable;

  std::string m_prefix = "";
  std::string m_suffix = "";
  int m_number = 0;

  bool m_show_number = true;
  bool m_show_text = true;

  bool m_needs_update = true;
};

#endif // NUMERIC_DISPLAY_TEXT_HPP
