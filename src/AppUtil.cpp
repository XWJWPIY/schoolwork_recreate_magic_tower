#include "AppUtil.hpp"
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
}

bool AttributeRegistry::IsAttribute(const std::string& name) {
    // Explicitly ignore structural fields
    if (name == Attr::ID || name == Attr::PATH || name == Attr::FOLDER || 
        name == Attr::PASSABLE || name == Attr::ANIMATION || name == Attr::TITLE || 
        name == Attr::ICON || name == Attr::DIALOG || name == Attr::IS_PASSIVE || 
        name == Attr::FLOOR_DELTA || name == Attr::TRANSACTIONS) {
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
    // Clear old data to ensure re-entry/restart resets transaction counts
    GlobalObjectRegistry.clear();
    GlobalObjectRegistry.emplace(0, ObjectMetadata("road", "Road", true));
    LoadSettings(MAGIC_TOWER_RESOURCE_DIR "/Datas/Data/Settings.csv");
    LoadBlocks(MAGIC_TOWER_RESOURCE_DIR "/Datas/Data/Block.csv");
    LoadDoors(MAGIC_TOWER_RESOURCE_DIR "/Datas/Data/Door.csv");
    LoadItems(MAGIC_TOWER_RESOURCE_DIR "/Datas/Data/Item.csv");
    LoadStairs(MAGIC_TOWER_RESOURCE_DIR "/Datas/Data/Stair.csv");
    LoadShops(std::string(MAGIC_TOWER_RESOURCE_DIR) + "/Datas/Data/Shop.csv");
    LoadNPCs(std::string(MAGIC_TOWER_RESOURCE_DIR) + "/Datas/Data/NPC.csv");
    LoadTriggers(std::string(MAGIC_TOWER_RESOURCE_DIR) + "/Datas/Data/Trigger.csv");
    LOG_INFO("RegistryLoader: Total {} object types in registry.", GlobalObjectRegistry.size());
}

void RegistryLoader::LoadSettings(const std::string& path) {
    auto data = MapParser::ParseCsvToStrings(path);
    if (data.empty()) return;

    GlobalSettings.clear();
    // Header: Key,Value
    for (size_t i = 1; i < data.size(); ++i) {
        const auto& row = data[i];
        if (row.size() < 2) continue;
        GlobalSettings[row[0]] = row[1];
    }
    LOG_INFO("RegistryLoader: Loaded {} settings.", GlobalSettings.size());
}

void RegistryLoader::LoadBlocks(const std::string& path) {
    CSVLoader loader;
    if (!loader.Load(path)) return;

    for (size_t i = 0; i < loader.GetRowCount(); ++i) {
        int id = loader.GetInt(i, Attr::ID);
        std::string res_name = loader.GetString(i, Attr::PATH);
        std::string folder = loader.GetString(i, Attr::FOLDER, "Road");
        bool passable = loader.GetBool(i, Attr::PASSABLE, true);
        int frames = loader.GetInt(i, Attr::ANIMATION, 1);

        GlobalObjectRegistry.emplace(id, ObjectMetadata(res_name, folder, passable, frames));
    }
}

void RegistryLoader::LoadDoors(const std::string& path) {
    CSVLoader loader;
    if (!loader.Load(path)) return;

    for (size_t i = 0; i < loader.GetRowCount(); ++i) {
        int id = loader.GetInt(i, Attr::ID);
        std::string res_name = loader.GetString(i, Attr::PATH);
        bool passable = loader.GetBool(i, Attr::PASSABLE, false);
        int frames = loader.GetInt(i, Attr::ANIMATION, 1);

        ObjectMetadata meta(res_name, "Door", passable, frames);
        meta.door_props = std::make_shared<DoorComponent>();
        meta.door_props->yellow_key = loader.GetInt(i, Attr::YELLOW_KEY);
        meta.door_props->blue_key = loader.GetInt(i, Attr::BLUE_KEY);
        meta.door_props->red_key = loader.GetInt(i, Attr::RED_KEY);
        meta.door_props->is_passive = loader.GetBool(i, Attr::IS_PASSIVE, false);

        GlobalObjectRegistry.emplace(id, std::move(meta));
    }
}

void RegistryLoader::LoadItems(const std::string& path) {
    CSVLoader loader;
    if (!loader.Load(path)) {
        LOG_ERROR("RegistryLoader: FAILED to load Item CSV: {}", path);
        return;
    }

    LOG_INFO("RegistryLoader: Starting Item processing... Row count: {}", loader.GetRowCount());

    for (size_t i = 0; i < loader.GetRowCount(); ++i) {
        int id = loader.GetInt(i, Attr::ID);
        if (id == 0) continue;

        std::string res_name = loader.GetString(i, Attr::PATH);
        bool passable = loader.GetBool(i, Attr::PASSABLE, false);
        int frames = loader.GetInt(i, Attr::ANIMATION, 1);

        LOG_INFO("RegistryLoader: Processing Item Row {} -> ID: {}, Name: {}", i, id, res_name);

        ObjectMetadata meta(res_name, "Item", passable, frames);
        meta.item_props = std::make_shared<ItemComponent>();
        
        auto effects = loader.GetRowEffects(i);
        for (const auto& eff : effects) {
            meta.item_props->effects.push_back(eff);
            meta.initial_attributes[eff.type_id] = eff.value;
        }

        if (effects.empty()) {
            LOG_WARN("RegistryLoader: Item ID {} has NO attributes loaded!", id);
        }

        std::string dialog = loader.GetString(i, Attr::DIALOG);
        if (!dialog.empty()) {
            meta.dialog_props = std::make_shared<DialogComponent>();
            meta.dialog_props->lines.push_back(dialog);
        }

        GlobalObjectRegistry[id] = std::move(meta);
    }
    LOG_INFO("RegistryLoader: Finished loading items. Registry size now: {}", GlobalObjectRegistry.size());
}

void RegistryLoader::LoadStairs(const std::string& path) {
    CSVLoader loader;
    if (!loader.Load(path)) return;

    for (size_t i = 0; i < loader.GetRowCount(); ++i) {
        int id = loader.GetInt(i, Attr::ID);
        std::string res_name = loader.GetString(i, Attr::PATH);
        bool passable = loader.GetBool(i, Attr::PASSABLE, true);
        int frames = loader.GetInt(i, Attr::ANIMATION, 1);

        ObjectMetadata meta(res_name, "Stair", passable, frames);
        meta.stair_props = std::make_shared<StairComponent>();
        meta.stair_props->floor_delta = loader.GetInt(i, Attr::FLOOR_DELTA, 0);

        // Fallback for legacy stair IDs if delta is not provided
        if (meta.stair_props->floor_delta == 0) {
            if (id == static_cast<int>(StairId::UP)) meta.stair_props->floor_delta = 1;
            else if (id == static_cast<int>(StairId::DOWN)) meta.stair_props->floor_delta = -1;
        }

        GlobalObjectRegistry.emplace(id, std::move(meta));
    }
}

void RegistryLoader::LoadShops(const std::string& path) {
    CSVLoader loader;
    if (!loader.Load(path)) return;

    for (size_t i = 0; i < loader.GetRowCount(); ++i) {
        int id = loader.GetInt(i, Attr::ID);
        std::string res_name = loader.GetString(i, Attr::PATH);
        std::string folder = loader.GetString(i, Attr::FOLDER, "Shop");
        bool passable = loader.GetBool(i, Attr::PASSABLE, false);
        int frames = loader.GetInt(i, Attr::ANIMATION, 1);

        ObjectMetadata meta(res_name, folder, passable, frames);
        meta.shop_props = std::make_shared<ShopComponent>();
        meta.shop_props->title = loader.GetString(i, Attr::TITLE);
        meta.shop_props->icon_path = loader.GetString(i, Attr::ICON);
        meta.shop_props->transaction_count = loader.GetInt(i, Attr::TRANSACTIONS, 0);

        // Special pricing for Greed God
        if (id == 602) {
            meta.shop_props->pricing_type = ShopPricingType::SCALING_GREED;
        }

        GlobalObjectRegistry.emplace(id, std::move(meta));
    }
}

void RegistryLoader::LoadNPCs(const std::string& path) {
    CSVLoader loader;
    if (!loader.Load(path)) return;

    for (size_t i = 0; i < loader.GetRowCount(); ++i) {
        int id = loader.GetInt(i, Attr::ID);
        std::string res_name = loader.GetString(i, Attr::PATH);
        std::string folder = loader.GetString(i, Attr::FOLDER, "Road");
        bool passable = loader.GetBool(i, Attr::PASSABLE, false);
        int frames = loader.GetInt(i, Attr::ANIMATION, 1);

        ObjectMetadata meta(res_name, folder, passable, frames);
        meta.dialog_props = std::make_shared<DialogComponent>();
        meta.dialog_props->title = loader.GetString(i, Attr::TITLE);
        meta.dialog_props->icon_path = loader.GetString(i, Attr::ICON);

        GlobalObjectRegistry.emplace(id, std::move(meta));
    }
}

void RegistryLoader::LoadTriggers(const std::string& path) {
    CSVLoader loader;
    if (!loader.Load(path)) return;

    for (size_t i = 0; i < loader.GetRowCount(); ++i) {
        int id = loader.GetInt(i, Attr::ID);
        std::string res_name = loader.GetString(i, Attr::PATH);
        std::string folder = loader.GetString(i, Attr::FOLDER, "Trigger");
        bool passable = loader.GetBool(i, Attr::PASSABLE, true);
        int frames = loader.GetInt(i, Attr::ANIMATION, 1);

        ObjectMetadata meta(res_name, folder, passable, frames);
        meta.dialog_props = std::make_shared<DialogComponent>();
        meta.dialog_props->title = loader.GetString(i, Attr::TITLE);
        meta.dialog_props->icon_path = loader.GetString(i, Attr::ICON);

        GlobalObjectRegistry.emplace(id, std::move(meta));
    }
}

std::string GetIdString(int id) {
  auto it = GlobalObjectRegistry.find(id);
  if (it != GlobalObjectRegistry.end()) {
    return it->second.name;
  }
  return "unknown";
}

std::string GetIdResourcePath(int id) {
  auto it = GlobalObjectRegistry.find(id);
  if (it != GlobalObjectRegistry.end()) {
    const auto& meta = it->second;
    int frame = meta.is_animated ? TileAnimationManager::GetGlobalFrame2() : 1;

    // Road, Shop, and Door folders always use numbered filenames (e.g. road1.bmp, blue_door1.bmp)
    if (meta.folder == "Road" || meta.folder == "Shop" || meta.folder == "Door") {
        return "/bmp/" + meta.folder + "/" + meta.name + std::to_string(frame) + ".bmp";
    }

    if (meta.folder == "Trigger") {
        // Triggers are meant to be invisible by default but logically active
        return "/bmp/Trigger/no_door.png";
    }

    // Other folders (Item, Door, etc.) only use numbers if animated
    if (meta.is_animated) {
        return "/bmp/" + meta.folder + "/" + meta.name + std::to_string(frame) + ".bmp";
    }
    return "/bmp/" + meta.folder + "/" + meta.name + ".bmp";
  }
  return "";
}
std::vector<std::vector<MapCell>>
MapParser::ParseCsv(const std::string &filepath) {
  std::vector<std::vector<MapCell>> mapData;
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

    std::vector<MapCell> row;
    std::stringstream ss(line);
    std::string cellString;

    while (std::getline(ss, cellString, ',')) {
      // Trim whitespace
      cellString.erase(0, cellString.find_first_not_of(" \t\r\n"));
      cellString.erase(cellString.find_last_not_of(" \t\r\n") + 1);

      if (cellString.empty())
        continue;

      MapCell cell;
      try {
        cell.id = std::stoi(cellString);
      } catch (const std::exception &e) {
        LOG_WARN("MapParser encounted invalid cell data '{}' in {}: {}",
                 cellString, filepath, e.what());
        cell.id = 0;
      }
      row.push_back(cell);
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

std::vector<std::vector<int>>
MapParser::ParseCsvToRawIDs(const std::string &filepath) {
  auto parsedCells = ParseCsv(filepath);
  std::vector<std::vector<int>> rawData;

  rawData.reserve(parsedCells.size());

  for (const auto &row : parsedCells) {
    std::vector<int> idRow;
    idRow.reserve(row.size());
    for (const auto &cell : row) {
      idRow.push_back(cell.id);
    }
    rawData.push_back(idRow);
  }

  return rawData;
}

std::vector<ShopOption> MapParser::ParseShopOptions(const std::string& filepath) {
    std::vector<ShopOption> options;
    CSVLoader loader;
    if (!loader.Load(filepath)) return options;

    for (size_t i = 0; i < loader.GetRowCount(); ++i) {
        ShopOption opt;
        opt.text = loader.GetString(i, Attr::DIALOG);
        opt.effects = loader.GetRowEffects(i);

        if (!opt.effects.empty() || !opt.text.empty()) {
            options.push_back(std::move(opt));
        }
    }
    return options;
}

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


} // namespace AppUtil
