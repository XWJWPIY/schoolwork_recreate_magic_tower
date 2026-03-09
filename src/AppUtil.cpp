#include "AppUtil.hpp"
#include "Util/Logger.hpp"
#include <fstream>
#include <sstream>

namespace AppUtil {

// TODO: Implement utility functions here
std::vector<std::vector<MapCell>>
MapParser::ParseCSV(const std::string &filepath) {
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
MapParser::ParseCSVToRawIDs(const std::string &filepath) {
  auto parsedCells = ParseCSV(filepath);
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
