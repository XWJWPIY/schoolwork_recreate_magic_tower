#ifndef APPUTIL_HPP
#define APPUTIL_HPP

#include "Util/Time.hpp"
#include "pch.hpp"
#include <string>
#include <unordered_map>
#include <vector>

namespace AppUtil {

// Global or shared utility functions and constant definitions
const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;
constexpr int TOTAL_STORY = 4;

extern const std::unordered_map<int, std::string> IdStringMap;

std::string GetIdString(int id);
std::string GetIdResourcePath(int id);

// Game state machine
enum class GameState {
  MainMenu = 0, // 0: Main menu
  Playing = 1   // 1: Playing session
};

struct MapCell {
  int id = 0;
};

struct TileAnimationManager {
  /**
   * @brief Get the current frame for 2-frame animations based on global time.
   * @param intervalMs How long each frame lasts in milliseconds.
   * @return 1 or 2
   */
  static int GetGlobalFrame2(int intervalMs = 500) {
    auto ms = static_cast<long long>(Util::Time::GetElapsedTimeMs());
    return static_cast<int>((ms / intervalMs) % 2) + 1;
  }
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
