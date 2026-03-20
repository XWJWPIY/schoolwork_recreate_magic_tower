#include "AppUtil.hpp"
#include "Util/Logger.hpp"
#include <cctype>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>


namespace AppUtil {

// 全域物件註冊表 (Global Object Registry)
// 格式：{ ID, {名稱, 資源資料夾, 是否可通行} }
std::unordered_map<int, ObjectMetadata> GlobalObjectRegistry = {
    {0, {"road", "Road", true}} // 預設為空，但參考道路的圖片大小
};

void RegistryLoader::LoadAllData() {
    LOG_INFO("Loading Game Object Registry from CSV...");
    // Keep ID 0, but we can clear others if LoadAllData is called multiple times
    // For now, let's keep it simple as it's called once at start.
    LoadBlocks(MAGIC_TOWER_RESOURCE_DIR "/Datas/Data/Block.csv");
    LoadDoors(MAGIC_TOWER_RESOURCE_DIR "/Datas/Data/Door.csv");
    LoadItems(MAGIC_TOWER_RESOURCE_DIR "/Datas/Data/Item.csv");
    LoadStairs(MAGIC_TOWER_RESOURCE_DIR "/Datas/Data/Stair.csv");
    LoadShops(MAGIC_TOWER_RESOURCE_DIR "/Datas/Data/Shop.csv");
    LOG_INFO("Object Registry loaded. Total objects: {}", GlobalObjectRegistry.size());
}

void RegistryLoader::LoadBlocks(const std::string& path) {
    auto data = MapParser::ParseCsvToStrings(path);
    if (data.empty()) return;

    // Header: ID,Path,Name,Passable,Animation
    for (size_t i = 1; i < data.size(); ++i) {
        const auto& row = data[i];
        if (row.size() < 5) continue;

        int id = std::stoi(row[0]);
        std::string res_name = row[1];
        std::string folder = "Road"; // Blocks are usually in Road folder
        bool passable = (row[3] == "true");
        int frames = std::stoi(row[4]);

        GlobalObjectRegistry.emplace(id, ObjectMetadata(res_name, folder, passable, frames));
    }
}

void RegistryLoader::LoadDoors(const std::string& path) {
    auto data = MapParser::ParseCsvToStrings(path);
    if (data.empty()) return;

    // Header: ID,Path,Passable,Animation,yellow_key,blue_key,red_key
    for (size_t i = 1; i < data.size(); ++i) {
        const auto& row = data[i];
        if (row.size() < 7) continue;

        int id = std::stoi(row[0]);
        std::string res_name = row[1];
        bool passable = (row[2] == "true");
        int frames = std::stoi(row[3]); // Doors usually have 5 frames for open animation, but now from CSV

        ObjectMetadata meta(res_name, "Door", passable, frames);
        meta.door_props = std::make_shared<DoorComponent>();
        meta.door_props->yellow_key = std::stoi(row[4]);
        meta.door_props->blue_key = std::stoi(row[5]);
        meta.door_props->red_key = std::stoi(row[6]);
        if (row.size() >= 8) {
            meta.door_props->is_passive = (row[7] == "true");
        }

        GlobalObjectRegistry.emplace(id, std::move(meta));
    }
}

void RegistryLoader::LoadItems(const std::string& path) {
    auto data = MapParser::ParseCsvToStrings(path);
    if (data.empty()) return;

    // Header: ID,Path,Passable,Animation,Level,HP,ATK,DEF,AGI,EXP,yellow key,blue key,red key,Coin,Weak,Poison,Dialog
    const auto& header = data[0];
    std::unordered_map<std::string, size_t> col_map;
    for (size_t j = 0; j < header.size(); ++j) col_map[header[j]] = j;

    for (size_t i = 1; i < data.size(); ++i) {
        const auto& row = data[i];
        if (row.size() < header.size()) continue;

        int id = std::stoi(row[0]);
        std::string res_name = row[1];
        bool passable = (row[2] == "true");
        int frames = std::stoi(row[3]);

        ObjectMetadata meta(res_name, "Item", passable, frames);
        meta.item_props = std::make_shared<ItemComponent>();

        // Map column values to effects
        auto add_effect = [&](const std::string& col, Effect type) {
            if (col_map.count(col)) {
                int val = std::stoi(row[col_map[col]]);
                if (val != 0) meta.item_props->effects.push_back({type, val});
            }
        };

        add_effect("Level", Effect::LEVEL);
        add_effect("HP", Effect::HP);
        add_effect("ATK", Effect::ATTACK);
        add_effect("DEF", Effect::DEFENSE);
        add_effect("AGI", Effect::AGILITY);
        add_effect("EXP", Effect::EXP);
        add_effect("yellow key", Effect::KEY_YELLOW);
        add_effect("blue key", Effect::KEY_BLUE);
        add_effect("red key", Effect::KEY_RED);
        add_effect("Coin", Effect::COIN);
        add_effect("Weak", Effect::WEAK);
        add_effect("Poison", Effect::POISON);

        if (col_map.count("Dialog") && !row[col_map["Dialog"]].empty()) {
            meta.dialog_props = std::make_shared<DialogComponent>();
            meta.dialog_props->lines.push_back(row[col_map["Dialog"]]);
        }

        GlobalObjectRegistry.emplace(id, std::move(meta));
    }
}

void RegistryLoader::LoadStairs(const std::string& path) {
    auto data = MapParser::ParseCsvToStrings(path);
    if (data.empty()) return;

    // Header: ID,Path,Passable,Animation
    for (size_t i = 1; i < data.size(); ++i) {
        const auto& row = data[i];
        if (row.size() < 4) continue;

        int id = std::stoi(row[0]);
        std::string res_name = row[1];
        bool passable = (row[2] == "true");
        int frames = std::stoi(row[3]);

        GlobalObjectRegistry.emplace(id, ObjectMetadata(res_name, "Stair", passable, frames));
    }
}

void RegistryLoader::LoadShops(const std::string& path) {
    auto data = MapParser::ParseCsvToStrings(path);
    if (data.empty()) return;

    // Header: ID,Path,Folder,Passable,Animation,HP_Value,ATK_Value,DEF_Value,Cost,Title
    for (size_t i = 1; i < data.size(); ++i) {
        const auto& row = data[i];
        if (row.size() < 10) continue;

        int id = std::stoi(row[0]);
        std::string res_name = row[1];
        std::string folder = row[2];
        bool passable = (row[3] == "true");
        int frames = std::stoi(row[4]);

        ObjectMetadata meta(res_name, folder, passable, frames);
        
        int hp = std::stoi(row[5]);
        int atk = std::stoi(row[6]);
        int def = std::stoi(row[7]);
        int cost = std::stoi(row[8]);
        std::string title = row[9];
        std::string icon_path = (row.size() >= 11) ? row[10] : "";
        int initial_transactions = (row.size() >= 12) ? std::stoi(row[11]) : 0;

        if (cost > 0) {
            meta.shop_props = std::make_shared<ShopComponent>();
            meta.shop_props->hp_reward = hp;
            meta.shop_props->atk_reward = atk;
            meta.shop_props->def_reward = def;
            meta.shop_props->cost = cost;
            meta.shop_props->title = title;
            meta.shop_props->icon_path = icon_path;
            meta.shop_props->transaction_count = initial_transactions;
        }

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

    // Road and Shop folders always use numbered filenames (e.g. road1.bmp, shop_1_11.bmp)
    if (meta.folder == "Road" || meta.folder == "Shop") {
        return "/bmp/" + meta.folder + "/" + meta.name + std::to_string(frame) + ".bmp";
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

} // namespace AppUtil
