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
    const std::string RELATION     = "relation";
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

// --- Main Metadata ---

struct ObjectMetadata {
    std::string name;
    std::string folder;
    bool is_passable;
    int frames; // frame count

    // All raw data from CSV row Map<AttrID, ValueString>
    std::unordered_map<int, std::string> attributes;

    ObjectMetadata() : is_passable(true), frames(1) {}
    ObjectMetadata(const std::string& n, const std::string& f, bool p, int fr = 1)
        : name(n), folder(f), is_passable(p), frames(fr) {}

    // Helpers to extract data based on column name (via AttributeRegistry)
    int GetInt(const std::string& key, int def = 0) const;
    std::string GetString(const std::string& key, const std::string& def = "") const;
    bool GetBool(const std::string& key, bool def = false) const;
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
    const std::unordered_map<std::string, int>& GetHeaderMap() const { return m_headerMap; }

private:
    std::unordered_map<std::string, int> m_headerMap;
    std::vector<std::vector<std::string>> m_data;
    std::vector<int> m_attributeCols; // Indices of columns that are attributes
};

class RegistryLoader {
public:
    static void LoadAllData();
    static void LoadSettings(const std::string& path);
    
    // Universal loader for all object types (Blocks, Items, NPCs, etc.)
    static void LoadObjectCSV(const std::string& path, const std::string& defaultFolder = "Road", bool defaultPassable = true);
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
bool ProbabilityGen(int p);
void ResetGameVariables();
int GetMaxGlobalFrame();

// Refactored asset path resolution
std::string GetBaseImagePath(int id);
std::string GetPhaseImagePath(const std::string& basePath, int phase);
std::string GetFullResourcePath(int id);
std::string GetStaticResourcePath(const std::string& relativePath);

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
