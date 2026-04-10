#include "UI/BackgroundUI.hpp"
#include "Core/AppUtil.hpp"
#include "Util/Time.hpp"

BackgroundUI::BackgroundUI() {
  m_image_drawable = std::make_shared<Util::Image>(ImagePath(0));
  m_inner_obj = std::make_shared<Util::GameObject>(m_image_drawable, -10);
  m_visible = true; // Background is usually visible by default
}

void BackgroundUI::run() {
  if (m_is_loading) {
    m_loading_timer += Util::Time::GetDeltaTimeMs();
    int current_frame = static_cast<int>(m_loading_timer / 150.0f) + 1;
    
    if (current_frame != m_loading_frame) {
      m_loading_frame = current_frame;
      if (m_loading_frame <= 4) {
        SetLoadingFrame(m_loading_frame);
      } else {
        m_is_loading = false;
      }
    }
  }
}

void BackgroundUI::SetVisible(bool visible) {
  m_visible = visible;
  if (m_inner_obj) {
    m_inner_obj->SetVisible(visible);
  }
}

void BackgroundUI::AddToRoot(Util::Renderer& root) {
  if (m_inner_obj) {
    root.AddChild(m_inner_obj);
  }
}

void BackgroundUI::NextPhase(const int phase) {
  if (m_image_drawable) {
    m_image_drawable->SetImage(ImagePath(phase));
  }
}

void BackgroundUI::StartLoading() {
  m_is_loading = true;
  m_loading_timer = 0.0f;
  m_loading_frame = 1;
  SetLoadingFrame(m_loading_frame);
}

void BackgroundUI::SetLoadingFrame(int frame) {
  if (m_image_drawable) {
    m_image_drawable->SetImage(AppUtil::GetPhaseImagePath("bmp/Scene/loading", frame));
  }
}
