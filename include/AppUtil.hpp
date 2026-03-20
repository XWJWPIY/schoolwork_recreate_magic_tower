#ifndef APPUTIL_HPP
#define APPUTIL_HPP

#include "Util/Time.hpp"
#include "pch.hpp"
#include <string>
#include <unordered_map>
#include <vector>

namespace AppUtil {

enum class Effect {
    NONE,
    HP,
    ATTACK,
    DEFENSE,
    AGILITY,
    KEY_YELLOW,
    KEY_BLUE,
    KEY_RED,
    COIN,
    LEVEL,
    EXP,
    WEAK,
    POISON
};

struct SubEffect {
    Effect type;
    int value;
};

// --- Components ---

struct ItemComponent {
    std::vector<SubEffect> effects;
};

struct CombatComponent {
    int hp = 0;
    int attack = 0;
    int defense = 0;
    int exp_reward = 0;
    int coin_reward = 0;
};

struct DialogComponent {
    std::vector<std::string> lines;
};

struct DoorComponent {
    int yellow_key = 0;
    int blue_key = 0;
    int red_key = 0;
    bool is_passive = false;
};

// --- Main Metadata ---

struct ObjectMetadata {
    std::string name;
    std::string folder;
    bool is_passable;
    bool is_animated;
    int animation_frames = 1;

    // Optional Components
    std::shared_ptr<ItemComponent>   item_props   = nullptr;
    std::shared_ptr<CombatComponent> combat_props = nullptr;
    std::shared_ptr<DialogComponent> dialog_props = nullptr;
    std::shared_ptr<DoorComponent>   door_props   = nullptr;

    // Picture Static Object (Wall, Road, NPC, etc.)
    ObjectMetadata(std::string n, std::string f, bool p)
        : name(std::move(n)), folder(std::move(f)), is_passable(p), 
          is_animated(false), animation_frames(1) {}

    // Picture Animated Object (Lava, Doors)
    ObjectMetadata(std::string n, std::string f, bool p, int frames)
        : name(std::move(n)), folder(std::move(f)), is_passable(p), 
          is_animated(frames > 1), animation_frames(frames) {}

    // Item Object (Implicitly passable, static)
    // We add a helper to attach items
    static ObjectMetadata Item(std::string n, std::string f, std::vector<SubEffect> e) {
        ObjectMetadata meta(std::move(n), std::move(f), true);
        meta.item_props = std::make_shared<ItemComponent>();
        meta.item_props->effects = std::move(e);
        return meta;
    }
};

extern std::unordered_map<int, ObjectMetadata> GlobalObjectRegistry;

class RegistryLoader {
public:
    static void LoadAllData();
    static void LoadBlocks(const std::string& path);
    static void LoadDoors(const std::string& path);
    static void LoadItems(const std::string& path);
    static void LoadStairs(const std::string& path);
};

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
  FAST_ELEVATOR = 3, // 3: Floor selection menu
  ITEM_DIALOG = 4,   // 4: Item acquisition dialog (Modal)
  LOADING = 5        // 5: Loading state
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
  static std::vector<std::vector<std::string>>
  ParseCsvToStrings(const std::string &filepath);
  static std::vector<std::vector<int>>
  ParseCsvToRawIDs(const std::string &filepath);
};

} // namespace AppUtil

#endif // APPUTIL_HPP
