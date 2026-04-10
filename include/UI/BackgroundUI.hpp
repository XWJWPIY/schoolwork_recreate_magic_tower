#ifndef BACKGROUND_UI_HPP
#define BACKGROUND_UI_HPP

#include "pch.hpp"

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Core/AppUtil.hpp"
#include "UI/UIComponent.hpp"
#include <string>
#include <memory>

class BackgroundUI : public UIComponent {
public:
  BackgroundUI();
  virtual ~BackgroundUI() override = default;

  // UIComponent Interface
  void run() override;
  void SetVisible(bool visible) override;
  void AddToRoot(Util::Renderer& root) override;
  bool IsActive() const override { return m_visible; }
  bool IsIntercepting() const override { return false; }

  // Logic
  void NextPhase(const int phase);
  void StartLoading();
  void SetLoadingFrame(int frame);
  int  GetLoadingFrame() const { return m_loading_frame; }

private:
  // 組合圖片路徑的輔助函式
  inline std::string ImagePath(const int phase) const {
    return AppUtil::GetPhaseImagePath("bmp/Scene/scene", phase);
  }

  std::shared_ptr<Util::GameObject> m_inner_obj;
  std::shared_ptr<Util::Image> m_image_drawable;
  
  bool  m_is_loading = false;
  float m_loading_timer = 0.0f;
  int   m_loading_frame = 0;
};

#endif // BACKGROUND_UI_HPP
