#ifndef APPUTIL_HPP
#define APPUTIL_HPP

#include "pch.hpp"
#include <string>
#include <vector>

namespace AppUtil {

// 全域或共用的工具函式與常數定義
const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;
constexpr int TOTAL_STORY = 4;

// 遊戲狀態機
enum class GameState {
  MainMenu = 0, // 0: 主頁
  Playing = 1   // 1: 正在遊戲
};

struct MapCell {
  int id = 0;
};

class MapParser {
public:
  static std::vector<std::vector<MapCell>>
  ParseCSV(const std::string &filepath);
  static std::vector<std::vector<int>>
  ParseCSVToRawIDs(const std::string &filepath);
};

} // namespace AppUtil

#endif // APPUTIL_HPP
