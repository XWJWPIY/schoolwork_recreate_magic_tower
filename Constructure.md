架構設計方案：

類別架構 (Entity 系統)

一、基底物件 (`AllObjects`)
- 繼承 `Util::GameObject`
- 提供所有地圖物件的基礎：`ObjectId`、座標 (`Transform`)、顯隱控制。
- 虛擬函數：`ObjectUpdate()`。

二、實體系統 (`Entity` 系統)
- 繼承 `AllObjects`。
- **虛擬基類**：新增 `virtual void reaction() = 0;` 與 `virtual bool IsPassable();` (預設為 `false`)。
- **實作分離**：所有衍生實體 (`Stair`, `Shop`, `Item` 等) 一律採用 `.hpp` 聲明與 `.cpp` 實作分離模式。
- **動態資源機制**：
    - 透過 `AppUtil::IdStringMap` 管理 ID 與名稱對映。
    - 使用 `AppUtil::GetIdResourcePath(id)` 自動根據名稱產出小寫底線格式之資源路徑 (例如 `"Yellow Key"` -> `"yellow_key.bmp"`)。
- **多型衍生**：
    1. **`Player` (主角)**：
        - 獨立於地圖網格管理，由 `App` 持有。
        - 負責處理 `Util::Input`、背包系統、升級邏輯。
        - **Z-Index 設定為 -3**。
    2. **`Character` (角色/怪物/NPC)**：包含 `NPC`, `Enemy` 等。
    3. **`Item` (道具)**：包含鍵、藥水等。
    4. **`Stair` (樓梯)**：具備 `m_OnTrigger` 回調函式，觸發時呼叫 `App::ChangeFloor`。

三、層級控制 (Z-Index 渲染順序)
- **Z = -5 (地板層)**：`RoadMap` (牆壁、地板)。
- **Z = -4 (物件層)**：`ThingsMap` (怪物、道具、NPC、樓梯)。
- **Z = -3 (主角層)**：單一 `Player` 實例。

地圖系統 (FloorMap 3D 結構)

一、多樓層存儲與切換
- 使用 **3D 陣列 `[story][y][x]`** 支援多樓層。
- **`App::ChangeFloor(int delta)`**：中心化切換邏輯，同步更新 `RoadMap`, `ThingsMap` 的樓層指標，並觸發 `Player->SyncPosition`。

二、物件管理
- `FloorMap` 透過 `BlockFactory` 根據 ID 動態生成對應的衍生類別。
- `Stair` 在建立時會被注入 lambda 閉包，使其能安全觸發 `App` 的樓層切換方法。

三、交互觸發流程
1. `Player` 嘗試移動。
2. 檢查 `RoadMap` 是否可通行 (`IsPassable`)。
3. 如果目標位置在 `ThingsMap` 有物件，呼叫該物件的 `reaction()`。
4. **穿透與阻擋條件**：
    - 若物件為不可通行且 reaction 後仍為 `Visible`，則阻擋移動。
    - 若物件 `IsPassable()` 為 `true` (如樓梯)，則允許重疊並觸發 reaction。
4. 根據 `reaction()` 結果決定移動是否成功或觸發特殊事件。

四、UI 系統 (文字與數值顯示)
- **`NumericDisplayText`**：
    - 繼承自 `Util::GameObject`。
    - **格式**：`Prefix` + `Number` + `Suffix` (例如：`m_Number` + `" F"` 顯示樓層)。
    - **控制旗標**：`m_ShowNumber`, `m_ShowText` 可獨立切換顯示。
    - **更新機制**：手動呼叫 `UpdateDisplayText()` 或在數值變動時更新，避免每影格重複渲染以節省效能。

UI 與存檔系統 (待擴充)
- UI 疊層將優先生於 z-index 更靠前的位置。
- 存檔將以 CSV 格式紀錄主角數值與 25 層樓的 `m_Blocks` ID 變動。
