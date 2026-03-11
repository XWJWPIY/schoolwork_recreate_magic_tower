架構構想：

類別設計

一、文字顯示系統
繼承 GameObject
    text 物件 {
        public:
        可存建構、設定欲輸出內容
    }
    數字 紀錄物件 {
        public:
        可存建構、設定欲輸出內容
        提供 記數、內部四則運算、歸零等功能
    }

二、通用物件系統 (AllObjects 架構)
設計抽象類別 `AllObjects` ，此類別繼承 `Util::GameObject`。
此為所有能在地圖上被互動或顯示的實體統一代稱，包含 ID 存取。
裡面包含設計 {
    virtual void SetObjectId(int id)
    virtual int GetObjectId() const
    virtual void ObjectUpdate() // 提供子類別自定義的邏輯更新介面 (避開 GameObject 無法 override 的問題)
}

三、角色與實體系統 (Entity 資料驅動系統)
放棄傳統的 Player / Monster 繼承樹，改用「資料驅動 (Data-Driven)」的單一 `Entity` 類別，繼承自 `AllObjects`。
所有的英雄、怪物、甚至是 NPC，在程式碼上都是 `Entity` 類別的實例。
裡面包含設計 {
    處理基本顯示與座標
    int 角色血量 (HP)
    int 角色攻擊力 (ATK)
    int 角色防禦力 (DEF)
    int 動畫展示圖編號 / 資源路徑 (ImagePath)
    enum EntityType (用來區分 Player, Enemy, NPC 等不同邏輯處理階段)
    State 狀態機 (行為模式)

    public:
    動態換皮與屬性載入 (讀取外部表單決定此 Entity 是誰)
    角色移動許可權設定與碰撞處理
}

四、地圖圖塊系統 (MapBlock 資料驅動系統)
由於已經導入了 `AllObjects` 與工廠模式，地圖圖塊系統也將 be 整合進去：
放棄傳統 Tile 繼承結構，基礎的牆壁、地板皆透過單一的 `MapBlock` 類別 (繼承自 `AllObjects`)。
如同 Entity 一樣，依靠讀表替換 ID 改變外觀與屬性。

裡面包含設計 {
    處理基本顯示與座標
    bool isPassable (是否能穿越)

    public:
    動態換皮與屬性載入 (呼叫 UpdateProperties)
}

備註：對於具有特殊互動邏輯的方塊 (如觸發事件、撿拾道具)，未來將透過在 `Entity` 加入對應標籤 (ComponentType::Event 或 ComponentType::Item) 來處理，逐步取代現存的 EventTile 與 CollectableTile 概念。

五、UI 系統 (目標呈現戰鬥雙方數值、以及右側角色資訊)
繼承 GameObject
    UI 物件 {
        public:
        
    }


六、讀檔紀錄系統
以 csv 格式儲存方便管理
除了地圖以外
橫排會對應到角色的圖檔、數值名稱等
縱排會對應到角色檔案、數值等
