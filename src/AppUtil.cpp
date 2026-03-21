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
    LOG_INFO("Cleaning and Loading Game Object Registry from CSV...");
    // Clear old data to ensure re-entry/restart resets transaction counts
    GlobalObjectRegistry.clear();
    GlobalObjectRegistry.emplace(0, ObjectMetadata("road", "Road", true));
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

    // Header: ID,Path,Passable,Animation,floor_delta
    for (size_t i = 1; i < data.size(); ++i) {
        const auto& row = data[i];
        if (row.size() < 4) continue;

        int id = std::stoi(row[0]);
        std::string res_name = row[1];
        bool passable = (row[2] == "true");
        int frames = std::stoi(row[3]);

        ObjectMetadata meta(res_name, "Stair", passable, frames);
        meta.stair_props = std::make_shared<StairComponent>();
        if (row.size() >= 5 && !row[4].empty()) {
            meta.stair_props->floor_delta = std::stoi(row[4]);
        } else {
            // Legacy mapping fallback during load time for stairs missing delta column
            if (id == static_cast<int>(StairId::UP)) meta.stair_props->floor_delta = 1;
            else if (id == static_cast<int>(StairId::DOWN)) meta.stair_props->floor_delta = -1;
        }

        GlobalObjectRegistry.emplace(id, std::move(meta));
    }
}

void RegistryLoader::LoadShops(const std::string& path) {
    auto data = MapParser::ParseCsvToStrings(path);
    if (data.empty()) return;

    // Header: ID,Path,Folder,Passable,Animation,Title,Icon,Initial_Transactions
    for (size_t i = 1; i < data.size(); ++i) {
        const auto& row = data[i];
        if (row.size() < 8) continue;

        int id = std::stoi(row[0]);
        std::string res_name = row[1];
        std::string folder = row[2];
        bool passable = (row[3] == "true");
        int frames = std::stoi(row[4]);

        ObjectMetadata meta(res_name, folder, passable, frames);
        
        std::string title = row[5];
        std::string icon_path = (row.size() >= 7) ? row[6] : "";
        int initial_transactions = (row.size() >= 8) ? std::stoi(row[7]) : 0;

        meta.shop_props = std::make_shared<ShopComponent>();
        meta.shop_props->title = title;
        meta.shop_props->icon_path = icon_path;
        meta.shop_props->transaction_count = initial_transactions;

        // Legacy mapping for Greed God
        if (id == 602) {
            meta.shop_props->pricing_type = ShopPricingType::SCALING_GREED;
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

std::vector<ShopOption> MapParser::ParseShopOptions(const std::string& filepath) {
    std::vector<ShopOption> options;
    auto data = ParseCsvToStrings(filepath);
    if (data.empty()) return options;

    // Header: Dialog,Level,HP,ATK,DEF,EXP,yellow key,blue key,red key,Coin
    for (size_t i = 1; i < data.size(); ++i) {
        const auto& row = data[i];
        if (row.size() < 10) continue;

        ShopOption opt;
        opt.text = row[0];

        auto add_effect = [&](size_t idx, Effect type) {
            if (idx < row.size() && !row[idx].empty()) {
                int val = std::stoi(row[idx]);
                if (val != 0) opt.effects.push_back({type, val});
            }
        };

        add_effect(1, Effect::LEVEL);
        add_effect(2, Effect::HP);
        add_effect(3, Effect::ATTACK);
        add_effect(4, Effect::DEFENSE);
        add_effect(5, Effect::EXP);
        add_effect(6, Effect::KEY_YELLOW);
        add_effect(7, Effect::KEY_BLUE);
        add_effect(8, Effect::KEY_RED);
        add_effect(9, Effect::COIN);

        if (!opt.effects.empty() || !opt.text.empty()) {
            options.push_back(std::move(opt));
        }
    }
    return options;
}


} // namespace AppUtil
