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

  void SetColor(const Util::Color &color);
  void UpdateDisplayText();

private:
  std::shared_ptr<Util::Text> m_TextDrawable;

  std::string m_Prefix = "";
  std::string m_Suffix = "";
  int m_Number = 0;

  bool m_ShowNumber = true;
  bool m_ShowText = true;

  bool m_NeedsUpdate = true;
};

#endif // NUMERIC_DISPLAY_TEXT_HPP
