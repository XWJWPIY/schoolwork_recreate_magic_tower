#ifndef APPUTIL_HPP
#define APPUTIL_HPP

#include "Util/Time.hpp"
#include "pch.hpp"
#include <string>
#include <unordered_map>
#include <vector>

namespace AppUtil {

struct ObjectMetadata {
    std::string name;
    std::string folder;    // e.g. "Road", "Enemy", "Item", "Door", "Stair"
    bool is_passable;
    bool is_animated;
    int animation_frames = 1; // Default to 1
};

extern const std::unordered_map<int, ObjectMetadata> GlobalObjectRegistry;

// Global or shared utility functions and constant definitions
const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;
constexpr int TOTAL_STORY = 26;

std::string GetIdString(int id);
std::string GetIdResourcePath(int id);

// Game state machine
enum class GameState {
  MAIN_MENU = 0,    // 0: Main menu
  PLAYING = 1,      // 1: Playing session
  INSTRUCTIONS = 2, // 2: Instruction manual
  FAST_ELEVATOR = 3 // 3: Floor selection menu
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
  ParseCsv(const std::string &filepath);
  static std::vector<std::vector<int>>
  ParseCsvToRawIDs(const std::string &filepath);
};

} // namespace AppUtil

#endif // APPUTIL_HPP
