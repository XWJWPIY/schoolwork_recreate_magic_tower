#include "UI/NumericDisplayText.hpp"
#include "Util/Color.hpp"

NumericDisplayText::NumericDisplayText(const std::string &fontPath,
                                       int fontSize) {
  m_prefix = "";
  m_suffix = "";
  m_number = 0;
  m_text_drawable = std::make_unique<Util::Text>(
      fontPath, fontSize, "0", Util::Color::FromRGB(255, 255, 255));
  SetDrawable(m_text_drawable);
}

void NumericDisplayText::SetPrefix(const std::string &prefix) {
  if (m_prefix != prefix) {
    m_prefix = prefix;
    m_needs_update = true;
  }
}

void NumericDisplayText::SetSuffix(const std::string &suffix) {
  if (m_suffix != suffix) {
    m_suffix = suffix;
    m_needs_update = true;
  }
}

void NumericDisplayText::SetNumber(int number) {
  if (m_number != number) {
    m_number = number;
    m_needs_update = true;
  }
}

void NumericDisplayText::SetShowNumber(bool show) {
  if (m_show_number != show) {
    m_show_number = show;
    m_needs_update = true;
  }
}

void NumericDisplayText::SetShowText(bool show) {
  if (m_show_text != show) {
    m_show_text = show;
    m_needs_update = true;
  }
}

void NumericDisplayText::SetColor(const Util::Color &color) {
  m_text_drawable->SetColor(color);
}

void NumericDisplayText::UpdateDisplayText() {
  std::string result = "";

  if (m_show_text) {
    result += m_prefix;
  }

  if (m_show_number) {
    result += std::to_string(m_number);
  }

  if (m_show_text) {
    result += m_suffix;
  }

  if (result.empty()) {
    result = " ";
  }

  m_text_drawable->SetText(result);
}
