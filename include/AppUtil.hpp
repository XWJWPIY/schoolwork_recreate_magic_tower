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
    POISON,
    FLY
};

namespace Attr {
    const std::string ID           = "ID";
    const std::string PATH         = "Path";
    const std::string FOLDER       = "Folder";
    const std::string PASSABLE     = "Passable";
    const std::string ANIMATION    = "Animation";
    const std::string TITLE        = "Title";
    const std::string ICON         = "Icon";
    const std::string HP           = "HP";
    const std::string ATTACK       = "ATK";
    const std::string DEFENSE      = "DEF";
    const std::string AGILITY      = "AGI";
    const std::string EXP          = "EXP";
    const std::string LEVEL        = "Level";
    const std::string YELLOW_KEY   = "yellow_key";
    const std::string BLUE_KEY     = "blue_key";
    const std::string RED_KEY      = "red_key";
    const std::string COIN         = "Coin";
    const std::string WEAK         = "Weak";
    const std::string POISON       = "Poison";
    const std::string IS_PASSIVE   = "is_passive";
    const std::string FLOOR_DELTA  = "floor_delta";
    const std::string TRANSACTIONS = "Initial_Transactions";
    const std::string DIALOG       = "Dialog";
}

class AttributeRegistry {
public:
    static int GetId(const std::string& name);
    static std::string GetName(int id);
    static Effect ToEffect(int id);
    static int FromEffect(Effect effect);
    static void Initialize();
    static bool IsAttribute(const std::string& name);

private:
    static std::unordered_map<std::string, int> m_nameToId;
    static std::unordered_map<int, std::string> m_idToName;
    static std::unordered_map<int, Effect> m_idToEffect;
    static std::unordered_map<Effect, int> m_effectToId;
    static int m_nextDynamicId;
};

struct SubEffect {
    int type_id; // Using dynamic ID instead of enum
    int value;
};

// Well-known ID mapping to avoid hardcoded literals
enum class StairId : int {
    UP = 701,
    DOWN = 702
};

enum class ShopPricingType {
    FIXED,
    SCALING_GREED
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
    std::string title;      // Name of the speaker
    std::string icon_path;  // Icon to show in dialogue box
    std::vector<std::string> lines;
};

struct DoorComponent {
    int yellow_key = 0;
    int blue_key = 0;
    int red_key = 0;
    bool is_passive = false;
};
struct ShopComponent {
    std::string title = "Store Explorer";
    std::string icon_path = "";
    int transaction_count = 0;
    ShopPricingType pricing_type = ShopPricingType::FIXED;
};
struct StairComponent {
    int floor_delta = 0;
};

// --- Main Metadata ---

struct ObjectMetadata {
    ObjectMetadata() = default;
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
    std::shared_ptr<ShopComponent>   shop_props   = nullptr;
    std::shared_ptr<StairComponent>  stair_props  = nullptr;

    // Initial attributes for actor construction
    std::unordered_map<int, int> initial_attributes;

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
extern std::unordered_map<std::string, std::string> GlobalSettings;
std::string GetGlobalString(const std::string& key, const std::string& defaultValue = "");

class CSVLoader {
public:
    bool Load(const std::string& path);
    size_t GetRowCount() const { return m_data.size(); }
    
    std::string GetString(size_t rowIndex, const std::string& colName, const std::string& def = "") const;
    int GetInt(size_t rowIndex, const std::string& colName, int def = 0) const;
    bool GetBool(size_t rowIndex, const std::string& colName, bool def = false) const;

    // Get all attributes found in this row that are registered in AttributeRegistry
    std::vector<SubEffect> GetRowEffects(size_t rowIndex) const;

private:
    std::unordered_map<std::string, int> m_headerMap;
    std::vector<std::vector<std::string>> m_data;
    std::vector<int> m_attributeCols; // Indices of columns that are attributes
};

class RegistryLoader {
public:
    static void LoadAllData();
    static void LoadSettings(const std::string& path);
    static void LoadBlocks(const std::string& path);
    static void LoadDoors(const std::string& path);
    static void LoadItems(const std::string& path);
    static void LoadStairs(const std::string& path);
    static void LoadShops(const std::string& path);
    static void LoadNPCs(const std::string& path);
    static void LoadTriggers(const std::string& path);
};

// Global or shared utility functions and constant definitions
const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;
constexpr int TOTAL_STORY = 26;

struct ShopOption {
    std::string text;
    std::vector<SubEffect> effects;
};

struct ShopData {
    std::string title;
    std::string icon_path;
    std::vector<std::string> prompts;
    int transaction_count;
    std::vector<ShopOption> options;
    std::string special_price_str; // e.g., real-time cost for Greed God
};

std::string GetIdString(int id);
std::string GetIdResourcePath(int id);

// Game state machine
enum class GameState {
  MAIN_MENU = 0,    // 0: Main menu
  PLAYING = 1,      // 1: Playing session
  INSTRUCTIONS = 2, // 2: Instruction manual
  FAST_ELEVATOR = 3, // 3: Floor selection menu
  ITEM_DIALOG = 4,   // 4: Item acquisition dialog (Modal)
  LOADING = 5,       // 5: Loading state
  SHOP = 6           // 6: Shop state
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
  static std::vector<ShopOption> ParseShopOptions(const std::string &filepath);
};

} // namespace AppUtil

#endif // APPUTIL_HPP
