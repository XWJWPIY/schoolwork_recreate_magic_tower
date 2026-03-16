#include "AppUtil.hpp"
#include "Util/Logger.hpp"
#include <cctype>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>


namespace AppUtil {

// 全域物件註冊表
// 格式：{ ID, {名稱, 資源資料夾, 是否可通行, 是否為動畫, [動畫幀數]} }
const std::unordered_map<int, ObjectMetadata> GlobalObjectRegistry = {
    // Road/Blocks (ID 0-99)
    {0, {"road", "Road", true, false}}, // 0 is nothing, but use road1.bmp as default to avoid error
    {1, {"road", "Road", true, false}},
    {2, {"lava_road", "Road", true, true}},
    {3, {"wall", "Road", false, false}},
    {4, {"wall_b", "Road", false, false}},
    {5, {"wall_shine", "Road", false, true}},
    {6, {"wall_special", "Road", false, false}},
    {7, {"lava", "Road", false, true}},

    // Items (ID 200-299)
    {201, {"yellow_key", "Item", true, false}},
    {202, {"blue_key", "Item", true, false}},
    {203, {"red_key", "Item", true, false}},
    {204, {"coin", "Item", true, false}},

    // Doors/Fences (ID 300-399)
    {301, {"iron_fence", "Door", false, true, 5}},
    {302, {"yellow_door", "Door", false, true, 5}},
    {303, {"blue_door", "Door", false, true, 5}},
    {304, {"red_door", "Door", false, true, 5}},
    {305, {"green_door", "Door", false, true, 5}},

    // Combatants (ID 400-499)
    {401, {"slime", "Enemy", false, false}},

    // NPCs/Shops (ID 500-699)
    {501, {"npc", "NPC", false, false}},
    {601, {"shop", "Things", false, false}},

    // Stairs/Transitions (ID 700-799)
    {701, {"upstair", "Stair", true, false}},
    {702, {"downstair", "Stair", true, false}}
};

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
    if (meta.folder == "Road") {
        // Special case for roads which often have "1" suffix or are animated
        return "/bmp/Road/" + meta.name + "1.bmp";
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
