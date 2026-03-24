#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include "pch.hpp"

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "AppUtil.hpp"
#include <string>

class Background : public Util::GameObject {
public:
  // 建構子：初始化 GameObject 並傳入第一張圖片，Z-Index 設為 -10
  Background();
  ~Background() override = default;

  // 切換到指定的相階段 (phase) 的背景圖片
  void NextPhase(const int phase);
  void SetLoadingFrame(int frame);

private:
  // 組合圖片路徑的輔助函式
  inline std::string ImagePath(const int phase) const {
    return AppUtil::GetStaticResourcePath("bmp/Scene/scene" + std::to_string(phase) + ".BMP");
  }
};

#endif // BACKGROUND_HPP
