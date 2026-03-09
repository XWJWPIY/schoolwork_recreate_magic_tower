#ifndef APPUTIL_HPP
#define APPUTIL_HPP

#include "pch.hpp"

namespace AppUtil {

// 全域或共用的工具函式與常數定義
const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;

// 遊戲狀態機
enum class GameState {
  MainMenu = 0, // 0: 主頁
  Playing = 1   // 1: 正在遊戲
};

} // namespace AppUtil

#endif // APPUTIL_HPP
