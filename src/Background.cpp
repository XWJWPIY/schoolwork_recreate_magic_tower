#include "Background.hpp"

Background::Background()
    : GameObject(std::make_unique<Util::Image>(ImagePath(0)), -10) {}

void Background::NextPhase(const int phase) {
  auto temp = std::dynamic_pointer_cast<Util::Image>(m_Drawable);
  if (temp) {
    temp->SetImage(ImagePath(phase));
  }
}
