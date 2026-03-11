架構設計方案：

類別架構 (Entity 系統)

一、基底物件 (`AllObjects`)
- 繼承 `Util::GameObject`
- 提供所有地圖物件的基礎：`ObjectId`、座標 (`Transform`)、顯隱控制。
- 虛擬函數：`ObjectUpdate()`。

二、實體系統 (`Entity` 系統)
- 繼承 `AllObjects`。
- **虛擬基類**：新增 `virtual void reaction() = 0;` 作為所有互動的出發點。
- 包含基本數值：`HP`, `ATK`, `DEF`, `Level`。
- **多型衍生**：
    1. **`Player` (主角)**：
        - 獨立於地圖網格管理，由 `App` 持有。
        - 負責處理 `Util::Input`、背包系統、升級邏輯。
        - **Z-Index 設定為 -3** (確保顯示在所有物件上方)。
    2. **`Character` (角色/怪物)**：
        - 包含 `NPC`, `Bat` (怪物) 等具備戰鬥或對話能力的實體。
        - `reaction()`：觸發戰鬥、開啟對話盒。
    3. **`Item` (道具)**：
        - 包含 `Key`, `Potion` 等。
        - `reaction()`：被主角撿起，從地圖移除並加入主角背包。

三、層級控制 (Z-Index 渲染順序)
- **Z = -5 (地板層)**：`RoadMap` (牆壁、地板)。
- **Z = -4 (物件層)**：`ThingsMap` (怪物、道具、NPC、樓梯)。
- **Z = -3 (主角層)**：單一 `Player` 實例。

地圖系統 (FloorMap 3D 結構)

一、多樓層存儲
- 將 `m_Blocks` 改為 **3D 陣列 `[story][y][x]`**，預計支援 25 層。
- 變數 `m_CurrentStory` (now) 紀錄目前活動樓層。

二、物件管理
- `FloorMap` 透過 `BlockFactory` 根據 ID 動態生成對應的衍生類別 (`Item`, `Character`)。
- 常駐記憶體設計：切換樓層時僅變更顯示與更新指標，不銷毀物件，確保樓層狀態（如怪物死亡）持續保存。

三、交互觸發流程
1. `Player` 嘗試移動。
2. 檢查 `RoadMap` 是否可通行。
3. 如果目標位置在 `ThingsMap` 有物件，呼叫該物件的 `reaction()`。
4. 根據 `reaction()` 結果決定移動是否成功或觸發特殊事件。

UI 與存檔系統 (待擴充)
- UI 層將繼承 `GameObject`。
- 存檔將以 CSV 格式紀錄主角數值與 25 層樓的 `m_Blocks` ID 變動。
