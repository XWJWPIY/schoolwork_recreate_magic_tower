#include "NumericDisplayText.hpp"
#include "Util/Color.hpp"

NumericDisplayText::NumericDisplayText(const std::string &fontPath,
                                       int fontSize) {
  m_Prefix = "";
  m_Suffix = "";
  m_Number = 0;
  m_TextDrawable = std::make_unique<Util::Text>(
      fontPath, fontSize, "0", Util::Color::FromRGB(255, 255, 255));
  SetDrawable(m_TextDrawable);
}

void NumericDisplayText::SetPrefix(const std::string &prefix) {
  if (m_Prefix != prefix) {
    m_Prefix = prefix;
    m_NeedsUpdate = true;
  }
}

void NumericDisplayText::SetSuffix(const std::string &suffix) {
  if (m_Suffix != suffix) {
    m_Suffix = suffix;
    m_NeedsUpdate = true;
  }
}

void NumericDisplayText::SetNumber(int number) {
  if (m_Number != number) {
    m_Number = number;
    m_NeedsUpdate = true;
  }
}

void NumericDisplayText::SetShowNumber(bool show) {
  if (m_ShowNumber != show) {
    m_ShowNumber = show;
    m_NeedsUpdate = true;
  }
}

void NumericDisplayText::SetShowText(bool show) {
  if (m_ShowText != show) {
    m_ShowText = show;
    m_NeedsUpdate = true;
  }
}

void NumericDisplayText::SetColor(const Util::Color &color) {
  m_TextDrawable->SetColor(color);
}

void NumericDisplayText::UpdateDisplayText() {
  std::string result = "";

  if (m_ShowText) {
    result += m_Prefix;
  }

  if (m_ShowNumber) {
    result += std::to_string(m_Number);
  }

  if (m_ShowText) {
    result += m_Suffix;
  }

  m_TextDrawable->SetText(result);
}
