#include "Entity.hpp"
#include "AppUtil.hpp"
#include "Util/Logger.hpp"
#include "Util/Image.hpp"

/* --- 動畫與同步機制註記 ---
 * 1. NPC、Shop：目前的影像資源具備多幀循環（frames > 1），統一由 AllObjects 基底類別
 *    透過 GLOBAL_SYNC 模式與「地磚/場景物件」進行全域同步控制。
 * 2. Enemy：未來實作呼吸動畫時，同樣需在 CSV 標註 Animation > 1，
 *    系統將自動納入全域同步軌道。
 * 3. Item、Stair、Trigger：由於 CSV 標註為 1 幀，系統將保持靜態顯示（STATIC）。
 */

Entity::Entity(int initialId, const std::string &imagePath, bool canReact) 
    : AllObjects(initialId) {
    SetZIndex(-4);
    m_can_react = canReact;
    SetObjectId(initialId); // 觸發基底類別的資源載入與動畫初始化
}

void Entity::Reaction(std::shared_ptr<Player> player) {
    // Default implementation does nothing
}

void Entity::TriggerReplacement(int targetId) {
    if (m_replacement_comp) {
        m_replacement_comp->ReplaceWith(m_grid_x, m_grid_y, targetId);
    }
}
