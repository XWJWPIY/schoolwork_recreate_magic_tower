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

二、角色系統 (Actor 系統)
設計抽象類別 Actor ，此類別繼承 GameObject
裡面包含設計 {
    處理基本顯示與座標
    bool 角色是否允許移動 (預設為否)
    int 角色血量
    int 角色攻擊力
    int 角色防禦力
    int 角色等級
    動畫組 (vector to shared_ptr Image)
    int 動畫展示圖編號
    圖塊是幾*幾組成 (以左上角當角色所在座標定位，但攻擊時只要碰到任何一格身體即可判定)

    public:
    角色移動許可權設定
    角色自身向上、下、左、右的移動判斷式(但若角色是否允許移動設為否，直接禁止移動)，回傳 bool false 代表撞到東西，可能需要與其他物件互動
    動畫播放與狀態機
}

繼承 Actor
    Player 物件 {
        public:
        增加資源管理（鑰匙、金幣、經驗值、藥水效果計數）
    }

繼承 Actor
    Monster 物件 {
        public:
        增加 onBattle() 邏輯與特殊的「能力標記」
    }

三、地圖圖塊系統 (Tile 系統)
設計抽象類別 Tile ，此類別繼承 GameObject
裡面包含設計 {
    地圖上的每一格
    能否穿越(預設為是，繼承者可自定義)
    方塊特性
}

繼承 Tile
    StaticTile 物件 {
        (牆、地板)
        public:
            bool 是否能穿越
    }

繼承 Tile
    EventTile 物件 {
        (樓梯、隱形牆、傳送點)
        public:
    }

繼承 Tile
    CollectableTile 物件 {
        (藥水、寶石、鑰匙、金幣等道具)
        public:
    }

四、UI 系統 (目標呈現戰鬥雙方數值、以及右側角色資訊)
繼承 GameObject
    UI 物件 {
        public:
        
    }


五、讀檔紀錄系統
以 csv 格式儲存方便管理
除了地圖以外
橫排會對應到角色的圖檔、數值名稱等
縱排會對應到角色檔案、數值等
