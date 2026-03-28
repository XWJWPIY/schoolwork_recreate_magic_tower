#include "Core/AppUtil.hpp"
#include "Objects/Player.hpp"
#include "Util/Logger.hpp"
#include <cctype>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>


namespace AppUtil {

// 全域物件註冊表 (Global Object Registry)
std::unordered_map<int, ObjectMetadata> GlobalObjectRegistry;
std::unordered_map<std::string, std::string> GlobalSettings;

// AttributeRegistry static members
std::unordered_map<std::string, int> AttributeRegistry::m_nameToId;
std::unordered_map<int, std::string> AttributeRegistry::m_idToName;
std::unordered_map<int, Effect> AttributeRegistry::m_idToEffect;
std::unordered_map<Effect, int> AttributeRegistry::m_effectToId;
int AttributeRegistry::m_nextDynamicId = 2000;

void AttributeRegistry::Initialize() {
    m_nameToId.clear();
    m_idToName.clear();
    m_idToEffect.clear();
    m_effectToId.clear();
    m_nextDynamicId = 2000;

    auto reg = [](const std::string& name, int id, Effect e = Effect::NONE) {
        m_nameToId[name] = id;
        m_idToName[id] = name;
        if (e != Effect::NONE) {
            m_idToEffect[id] = e;
            m_effectToId[e] = id;
        }
    };

    // Standard attributes matching Effect enum
    reg(Attr::HP,         1, Effect::HP);
    reg(Attr::ATTACK,     2, Effect::ATTACK);
    reg(Attr::DEFENSE,    3, Effect::DEFENSE);
    reg(Attr::AGILITY,    4, Effect::AGILITY);
    reg(Attr::EXP,        5, Effect::EXP);
    reg(Attr::LEVEL,      6, Effect::LEVEL);
    reg(Attr::YELLOW_KEY, 7, Effect::KEY_YELLOW);
    reg(Attr::BLUE_KEY,   8, Effect::KEY_BLUE);
    reg(Attr::RED_KEY,    9, Effect::KEY_RED);
    reg(Attr::COIN,       10, Effect::COIN);
    reg(Attr::WEAK,       11, Effect::WEAK);
    reg(Attr::POISON,     12, Effect::POISON);
    reg("Fly",            13, Effect::FLY);
    
    // Aliases or special structural attributes
    reg(Attr::FLOOR_DELTA, 100);
    reg(Attr::RELATION, 100); // Same ID as FLOOR_DELTA
}

bool AttributeRegistry::IsAttribute(const std::string& name) {
    // Explicitly ignore structural fields
    if (name == Attr::ID || name == Attr::PATH || name == Attr::FOLDER || 
        name == Attr::PASSABLE || name == Attr::ANIMATION || name == Attr::TITLE || 
        name == Attr::ICON || name == Attr::DIALOG || name == Attr::IS_PASSIVE || 
        name == Attr::FLOOR_DELTA || name == Attr::RELATION || name == Attr::TRANSACTIONS) {
        return false;
    }
    // Anything else in a CSV row that isn't structural is treated as a potential attribute
    return true;
}

int AttributeRegistry::GetId(const std::string& name) {
    auto it = m_nameToId.find(name);
    if (it != m_nameToId.end()) return it->second;

    // Register new dynamic attribute
    int newId = m_nextDynamicId++;
    m_nameToId[name] = newId;
    m_idToName[newId] = name;
    LOG_INFO("AttributeRegistry: Registered new dynamic attribute '{}' with ID {}", name, newId);
    return newId;
}

std::string AttributeRegistry::GetName(int id) {
    auto it = m_idToName.find(id);
    return (it != m_idToName.end()) ? it->second : "UnknownAttr";
}

Effect AttributeRegistry::ToEffect(int id) {
    auto it = m_idToEffect.find(id);
    return (it != m_idToEffect.end()) ? it->second : Effect::NONE;
}

int AttributeRegistry::FromEffect(Effect effect) {
    auto it = m_effectToId.find(effect);
    return (it != m_effectToId.end()) ? it->second : 0;
}

std::string GetGlobalString(const std::string& key, const std::string& defaultValue) {
    auto it = GlobalSettings.find(key);
    if (it != GlobalSettings.end()) return it->second;
    return defaultValue;
}

void RegistryLoader::LoadAllData() {
    LOG_INFO("RegistryLoader: Initializing Attribute Registry...");
    AttributeRegistry::Initialize();

    LOG_INFO("Cleaning and Loading Game Object Registry from CSV...");
    GlobalObjectRegistry.clear();
    GlobalObjectRegistry.emplace(0, ObjectMetadata("road", "Road", true));

    LoadSettings(GetStaticResourcePath("Datas/Data/Settings.csv"));
    LoadSettings(GetStaticResourcePath("Datas/Data/UIStrings.csv"));
    
    // Using the Universal Flattened Loader
    LoadObjectCSV(GetStaticResourcePath("Datas/Data/Block.csv"), "Road", true);
    LoadObjectCSV(GetStaticResourcePath("Datas/Data/Door.csv"), "Door", false);
    LoadObjectCSV(GetStaticResourcePath("Datas/Data/Item.csv"), "Item", false);
    LoadObjectCSV(GetStaticResourcePath("Datas/Data/Stair.csv"), "Stair", true);
    LoadObjectCSV(GetStaticResourcePath("Datas/Data/Shop.csv"), "Shop", false);
    LoadObjectCSV(GetStaticResourcePath("Datas/Data/NPC.csv"), "Road", false);
    LoadObjectCSV(GetStaticResourcePath("Datas/Data/Trigger.csv"), "Trigger", true);
    LoadObjectCSV(GetStaticResourcePath("Datas/Data/Enemy.csv"), "Enemy", false);

    LOG_INFO("RegistryLoader: Total {} object types in registry.", GlobalObjectRegistry.size());
}

void RegistryLoader::LoadObjectCSV(const std::string& path, const std::string& defaultFolder, bool defaultPassable) {
    CSVLoader loader;
    if (!loader.Load(path)) {
        LOG_ERROR("RegistryLoader: FAILED to load CSV: {}", path);
        return;
    }

    LOG_INFO("RegistryLoader: Loading object CSV: {} (rows: {})", path, loader.GetRowCount());

    for (size_t i = 0; i < loader.GetRowCount(); ++i) {
        int id = loader.GetInt(i, Attr::ID);
        if (id <= 0) continue;

        std::string name = loader.GetString(i, Attr::PATH);
        std::string folder = loader.GetString(i, Attr::FOLDER);
        if (folder.empty()) folder = defaultFolder;
        
        bool passable = loader.GetBool(i, Attr::PASSABLE, defaultPassable);
        int frames = loader.GetInt(i, Attr::ANIMATION, 1);

        ObjectMetadata meta(name, folder, passable, frames);
        
        // Store EVERY column into the raw attributes map
        for (auto const& [colName, colIdx] : loader.GetHeaderMap()) {
            meta.attributes[AttributeRegistry::GetId(colName)] = loader.GetString(i, colName);
        }

        GlobalObjectRegistry[id] = std::move(meta);
    }
}

// --- ObjectMetadata Helpers ---

int ObjectMetadata::GetInt(const std::string& key, int def) const {
    auto it = attributes.find(AttributeRegistry::GetId(key));
    if (it == attributes.end() || it->second.empty()) return def;
    try { return std::stoi(it->second); } catch (...) { return def; }
}

std::string ObjectMetadata::GetString(const std::string& key, const std::string& def) const {
    auto it = attributes.find(AttributeRegistry::GetId(key));
    if (it == attributes.end()) return def;
    return it->second;
}

bool ObjectMetadata::GetBool(const std::string& key, bool def) const {
    std::string val = GetString(key, "");
    if (val.empty()) return def;
    std::string lowerVal = val;
    for (auto &c : lowerVal) c = (char)std::tolower((unsigned char)c);
    if (lowerVal == "true") return true;
    if (lowerVal == "false") return false;
    return def;
}

void RegistryLoader::LoadSettings(const std::string& path) {
    auto data = MapParser::ParseCsvToStrings(path);
    if (data.empty()) return;

    // Header: Key,Value
    for (size_t i = 1; i < data.size(); ++i) {
        const auto& row = data[i];
        if (row.size() < 2) continue;
        GlobalSettings[row[0]] = row[1];
    }
    LOG_INFO("RegistryLoader: Loaded {} settings.", GlobalSettings.size());
}

std::string GetIdString(int id) {
  auto it = GlobalObjectRegistry.find(id);
  if (it != GlobalObjectRegistry.end()) {
    return it->second.name;
  }
  return "unknown";
}

std::string GetBaseImagePath(int id) {
  auto it = GlobalObjectRegistry.find(id);
  if (it == GlobalObjectRegistry.end()) return "";

  const auto& meta = it->second;
  std::string folder = meta.GetString(Attr::FOLDER);
  std::string path = meta.GetString(Attr::PATH);
  
  if (path.empty()) return "";

  std::string baseDir = "bmp/";
  if (!folder.empty()) {
      baseDir += folder + "/";
  }
  return baseDir + path;
}

std::string GetPhaseImagePath(const std::string& basePath, int phase) {
  if (basePath.empty()) return "";

  std::string fullBasePath = basePath + std::to_string(phase);

  auto fileExists = [](const std::string& p) {
      std::string fullPath = std::string(MAGIC_TOWER_RESOURCE_DIR) + "/" + p;
      std::ifstream f(fullPath);
      return f.good();
  };

  std::vector<std::string> variations = { ".bmp", ".BMP", ".png", ".PNG" };
  for (const auto& ext : variations) {
      if (fileExists(fullBasePath + ext)) {
          return GetStaticResourcePath(fullBasePath + ext);
      }
  }

  return GetStaticResourcePath(fullBasePath + ".bmp");
}

std::string GetStaticResourcePath(const std::string& relativePath) {
  return std::string(MAGIC_TOWER_RESOURCE_DIR) + "/" + relativePath;
}

std::string GetFullResourcePath(int id) {
  auto it = GlobalObjectRegistry.find(id);
  if (it == GlobalObjectRegistry.end()) return "";

  int totalFrames = it->second.frames;
  int currentFrame = (totalFrames > 1) ? TileAnimationManager::GetGlobalFrame2() : 1;
  return GetPhaseImagePath(GetBaseImagePath(id), currentFrame);
}

std::vector<std::vector<int>>
MapParser::ParseCsv(const std::string &filepath) {
  std::vector<std::vector<int>> mapData;
  std::ifstream file(filepath);

  if (!file.is_open()) {
    LOG_ERROR("MapParser failed to open file: {}", filepath);
    return mapData;
  }

  std::string line;
  while (std::getline(file, line)) {
    if (line.empty())
      continue;

    // Handle carriage returns for Windows/Linux compatibility
    if (!line.empty() && line.back() == '\r') {
      line.pop_back();
    }

    std::vector<int> row;
    std::stringstream ss(line);
    std::string cellString;

    while (std::getline(ss, cellString, ',')) {
      // Trim whitespace
      cellString.erase(0, cellString.find_first_not_of(" \t\r\n"));
      cellString.erase(cellString.find_last_not_of(" \t\r\n") + 1);

      if (cellString.empty())
        continue;

      int id = 0;
      try {
        id = std::stoi(cellString);
      } catch (const std::exception &e) {
        LOG_WARN("MapParser encountered invalid cell data '{}' in {}: {}",
                 cellString, filepath, e.what());
        id = 0;
      }
      row.push_back(id);
    }

    if (!row.empty()) {
      mapData.push_back(row);
    }
  }

  return mapData;
}

std::vector<std::vector<std::string>>
MapParser::ParseCsvToStrings(const std::string &filepath) {
  std::vector<std::vector<std::string>> mapData;
  std::ifstream file(filepath);

  if (!file.is_open()) {
    LOG_ERROR("MapParser failed to open file: {}", filepath);
    return mapData;
  }

  std::string line;
  while (std::getline(file, line)) {
    if (line.empty())
      continue;

    if (!line.empty() && line.back() == '\r') {
      line.pop_back();
    }

    std::vector<std::string> row;
    std::stringstream ss(line);
    std::string cellString;

    while (std::getline(ss, cellString, ',')) {
      // Trim whitespace
      cellString.erase(0, cellString.find_first_not_of(" \t\r\n"));
      cellString.erase(cellString.find_last_not_of(" \t\r\n") + 1);

      row.push_back(cellString);
    }

    if (!row.empty()) {
      mapData.push_back(row);
    }
  }

  return mapData;
}

// Removed ParseCsvToRawIDs as it was redundant after MapCell flattening.

// ParseShopOptions moved to ShopSystem

// --- CSVLoader Implementation ---

bool CSVLoader::Load(const std::string& path) {
    m_data.clear();
    m_headerMap.clear();
    m_attributeCols.clear();

    std::ifstream file(path);
    if (!file.is_open()) {
        LOG_ERROR("CSVLoader: Failed to open file {}", path);
        return false;
    }

    std::string line;
    if (std::getline(file, line)) {
        if (!line.empty() && (line.back() == '\r' || line.back() == '\n')) line.pop_back();
        if (!line.empty() && (line.back() == '\r' || line.back() == '\n')) line.pop_back();

        std::stringstream ss(line);
        std::string cell;
        int colIdx = 0;
        while (std::getline(ss, cell, ',')) {
            cell.erase(0, cell.find_first_not_of(" \t\r\n\xEF\xBB\xBF")); // Trim BOM
            cell.erase(cell.find_last_not_of(" \t\r\n") + 1);
            m_headerMap[cell] = colIdx;
            LOG_INFO("CSVLoader: Header -> '{}' at index {}", cell, colIdx);
            
            if (AttributeRegistry::IsAttribute(cell)) {
                m_attributeCols.push_back(colIdx);
            }
            colIdx++;
        }
    }

    LOG_INFO("CSVLoader: Parsing file {} (found {} columns, {} attributes)", path, m_headerMap.size(), m_attributeCols.size());

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        if (line.back() == '\r') line.pop_back();
        std::stringstream ss(line);
        std::string cell;
        std::vector<std::string> row;
        while (std::getline(ss, cell, ',')) {
            cell.erase(0, cell.find_first_not_of(" \t\r\n"));
            cell.erase(cell.find_last_not_of(" \t\r\n") + 1);
            row.push_back(cell);
        }
        if (!row.empty()) m_data.push_back(std::move(row));
    }
    LOG_INFO("CSVLoader: Total rows loaded: {}", m_data.size());
    return true;
}

std::string CSVLoader::GetString(size_t rowIndex, const std::string& colName, const std::string& def) const {
    auto it = m_headerMap.find(colName);
    if (it == m_headerMap.end() || rowIndex >= m_data.size()) return def;
    const auto& row = m_data[rowIndex];
    if (it->second >= row.size()) return def;
    return row[it->second];
}

int CSVLoader::GetInt(size_t rowIndex, const std::string& colName, int def) const {
    std::string val = GetString(rowIndex, colName, "");
    if (val.empty()) return def;
    try {
        return std::stoi(val);
    } catch (...) {
        return def;
    }
}

bool CSVLoader::GetBool(size_t rowIndex, const std::string& colName, bool def) const {
    std::string val = GetString(rowIndex, colName, "");
    if (val.empty()) return def;

    // Convert to lowercase for case-insensitive comparison
    std::string lowerVal = val;
    for (auto &c : lowerVal) c = (char)std::tolower((unsigned char)c);

    if (lowerVal == "true") return true;
    if (lowerVal == "false") return false;
    return def;
}

std::vector<SubEffect> CSVLoader::GetRowEffects(size_t rowIndex) const {
    std::vector<SubEffect> effects;
    if (rowIndex >= m_data.size()) return effects;
    const auto& row = m_data[rowIndex];

    for (int colIdx : m_attributeCols) {
        if (colIdx < (int)row.size() && !row[colIdx].empty()) {
            int val = 0;
            try { val = std::stoi(row[colIdx]); } catch (...) { continue; }
            if (val != 0) {
                // Determine column name from index by searching m_headerMap
                std::string colName;
                for (const auto& pair : m_headerMap) {
                    if (pair.second == colIdx) {
                        colName = pair.first;
                        break;
                    }
                }
                
                if (!colName.empty()) {
                    effects.push_back({AttributeRegistry::GetId(colName), val});
                }
            }
        }
    }
    return effects;
}


long long CalculateDamage(::Player* player, int enemyId) {
    auto it = GlobalObjectRegistry.find(enemyId);
    if (it == GlobalObjectRegistry.end() || !player) return 0;

    const auto& meta = it->second;
    int eHP = meta.GetInt(Attr::HP);
    int eATK = meta.GetInt(Attr::ATTACK);
    int eDEF = meta.GetInt(Attr::DEFENSE);
    
    int pHP = player->GetAttr(Effect::HP);
    int pATK = player->GetAttr(Effect::ATTACK);
    int pDEF = player->GetAttr(Effect::DEFENSE);

    // Special Abilities
    bool ignoreDef = meta.GetInt("Ignore_DEF") > 0;
    int atkTime = meta.GetInt("ATK_Time", 1);
    
    // Player's damage to enemy per hit
    int pDamage = pATK - eDEF;
    if (pDamage <= 0) return -1; // Invincible enemy

    // Rounds to kill enemy
    int rounds = (eHP + pDamage - 1) / pDamage;

    // Enemy's damage to player per hit (or set of hits)
    int eDamageBase = ignoreDef ? eATK : (eATK - pDEF);
    if (eDamageBase < 0) eDamageBase = 0;
    
    long long totalDamage = (long long)eDamageBase * atkTime * (rounds - 1);

    // Additional special logic (e.g. poisoning, critical strikes) can be added here
    // For now, matching the standard formula
    
    return totalDamage;
}

} // namespace AppUtil
