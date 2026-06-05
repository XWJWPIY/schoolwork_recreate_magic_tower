# 2026 OOPL Final Report

## 組別資訊

- 組別：42
- 組員：113820030 呂翊詳
- 復刻遊戲：魔塔

## 專案簡介

### 遊戲簡介

- 復刻經典遊戲魔塔，原本為地上 20 層，地下 25 層以及 10 關隱藏關卡，本次製作地上 20 層魔塔以及 5 關隱藏關卡，在劇情上做了一些調整。

### 組別分工

- 我是一人組，遊戲一個人實作。

## 遊戲介紹

### 遊戲規則

#### 遊戲目標

解救公主，遊戲結局分為兩種

| 結局 | 說明 |
|------|------|
| 勝利 | 解救公主 |
| 失敗 | 體力歸零(或是低於 0) |

#### 按鍵說明

- `Space` - 確認(但若 UI 有顯示其他按鍵請以 UI 的為主)
- `上下左右` - 操縱勇者移動
- `R` - 直接重新開始 (沒有確認，注意使用)
- `Q` - 撤退（攻擊中使用）
- `L` - 開啟按鍵說明(再按一次關閉)
- `F` - 開啟電梯（獲得指定道具後使用，但隱藏關卡無法使用）
- `D` - 開啟心鏡（查看預估攻擊怪物的數值，限獲得指定道具後使用）
- `G` - 切換超級長頸鹿模式提高攻擊、防禦力、金幣、經驗、體力、敏捷，可供高攻擊傷害與測試商店，並可直接使用電梯與心鏡
    - 再按一次時恢復原本勇者屬性，基本上屬性不互通(除以下例外)，注意吃到道具時的角色狀態
    - 不過衰弱與中毒狀態兩者設計是互通的
- `I` - 凍結戰鬥(再按一次解開)
- `8` - 直接強制原地上樓
- `2` - 直接強制原地下樓

#### 屬性說明

| 屬性 | 說明 |
|------|------|
| 體力 (HP) | 歸零即遊戲結束 |
| 攻擊力 (ATK) | 決定對敵人的基礎傷害 |
| 防禦力 (DEF) | 減少敵人對自己的傷害 |
| 敏捷 (AGI) | 每次被攻擊時，有 AGI% 機率閃避該次傷害 |
| 等級 (LV) | 就單純等級，無任何作用 |
| 經驗值 (EXP) | 打怪獲得，商店購物使用 |
| 金幣 (Coin) | 商店購物使用 |
| 黃/藍/紅鑰匙 | 分別用來開啟對應顏色的門 |

#### 戰鬥機制

每一回合流程：**勇者攻擊敵人 1 次 → 敵人攻擊勇者 N 次（N 由敵人的攻擊段數決定，本次實作範圍是 1~3 次），一律勇者先攻**

**勇者攻擊敵人：**
- 傷害 = 勇者攻擊力 − 敵人防禦力，最低為 1
- 若勇者處於「衰弱」狀態，該回合攻擊力先 × 0.8 再去與敵人相減
- 有（敵人敏捷）% 的機率使敵人完全閃避本次攻擊（傷害為 0）

**敵人攻擊勇者（每段獨立計算）：**
- 傷害 = 敵人攻擊力 − 勇者防禦力，最低為 1
- 若敵人具「無視防禦」能力，傷害直接等於敵人攻擊力
- 若勇者處於「衰弱」狀態，該回合防禦力先 × 0.8 再去與敵人相減
- 有（勇者敏捷）% 的機率閃避本次傷害（閃避成功則跳過所有狀態判定）
- 若敵人有「必殺攻擊」特殊能力，每次攻擊有 10% 機率直接擊殺勇者
- 若敵人有「衰弱」特殊能力，命中後有 9% 機率令勇者進入衰弱狀態
- 若敵人有「中毒」特殊能力，命中後有 9% 機率令勇者進入中毒狀態

**戰鬥結束（擊敗敵人）：**
- 獲得敵人的經驗值與金幣
- 敵人從地圖上消失（部分敵人死後會觸發下一隻敵人或獎勵地圖）

#### 狀態異常

| 狀態 | 效果 | 解除方式 |
|------|------|---------|
| 衰弱 | 攻擊力和防禦力在打架時會 x0.8 再去與敵人相減 | 吃火酒 |
| 中毒 | 每一輪動畫週期扣 1 點體力，最低保留 1 點（不致死） | 吃抗毒劑 |

- 衰弱與中毒可同時存在，狀態欄顯示「衰弱中毒」
- 超級模式下衰弱/中毒的觸發邏輯不變，但因 HP 極高實際影響可忽略
- 衰弱與中毒狀態是正常模式與超級模式共享

#### 門的種類

| 門的種類 | 消耗鑰匙 |
|---------|---------|
| 黃門 | 黃鑰匙 ×1 |
| 藍門 | 藍鑰匙 ×1 |
| 紅門 | 紅鑰匙 ×1 |
| 綠門 | 黃、藍、紅鑰匙各 ×1 |
| 鐵柵欄 | 無需鑰匙 |

#### 樓梯機制

- 踩上「上樓梯」即前往上一層樓
- 踩上「下樓梯」即前往下一層樓
- 特定樓梯可傳送至指定座標（用於隱藏關卡或非線性路徑）
- debug:
    - 按 8 強制原地上樓
    - 按 2 強制原地下樓

#### 商店機制

**神像商店（地圖上固定位置）：**
- 貪婪之神：
    - 費用公式：第 N 次購買費用 = 20 + N，超超過第 25 次後每次額外再 +4
- 戰鬥之神：購買防禦力提升（花費經驗值）
- 同種神像的交易次數全地圖共用（跨樓層）

**NPC 商店（由對話觸發）：**
- 每位 NPC 有獨立的交易次數計數，不與其他樓層的同款 NPC 互通
- 部分 NPC 商店有最大交易次數限制，達到上限後無法再購買

### 遊戲畫面

| 說明 | 畫面 |
|---------|---------|
| 待機畫面 | <img src="./FinalProjectImg/Start.png" width="550">     |
| 遊戲一開始 | <img src="./FinalProjectImg/First_game.png" width="550">     |
| 按鍵說明 | <img src="./FinalProjectImg/Help.png" width="550">     |
| NPC 對話 | <img src="./FinalProjectImg/NPC.png" width="550">     |
| 對戰 | <img src="./FinalProjectImg/Battle.png" width="550">     |
| 對戰獲勝 | <img src="./FinalProjectImg/Battle_win.png" width="550">     |
| 撿到道具 | <img src="./FinalProjectImg/Get_item.png" width="550">     |
| 大商店交易 | <img src="./FinalProjectImg/Shop.png" width="550">     |
| NPC商店購買 | <img src="./FinalProjectImg/NPC_Shop.png" width="550">     |
| 敵人圖鑑 | <img src="./FinalProjectImg/Enemy_Book.png" width="550">     |
| 獲得負面狀態 | <img src="./FinalProjectImg/Debuf.png" width="550">     |
| 電梯 | <img src="./FinalProjectImg/Elevator.png" width="550">     |
| 巨大怪物 | <img src="./FinalProjectImg/Big_Enemy.png" width="550">     |
| 假公主 | <img src="./FinalProjectImg/Fake_Princess.png" width="550">     |
| 公主 | <img src="./FinalProjectImg/Princess.png" width="550">     |
| 勝利 | <img src="./FinalProjectImg/Win.png" width="550">     |
| 死亡 | <img src="./FinalProjectImg/Death.png" width="550">     |
| 長頸鹿好幫手 | <img src="./FinalProjectImg/Giraffe_Helper.png" width="550">     |



## 程式設計

### 程式架構

#### 繼承架構圖 (Hierarchy Overview)

這是一份省略屬性與方法的純繼承關聯圖，供快速掌握類別層級關係：

```mermaid
classDiagram
    direction TB
    class GameObject["Util::GameObject"]
    class Entity
    class UIComponent

    GameObject <|-- Entity
    GameObject <|-- NumericDisplayText
    
    Entity <|-- MapBlock
    Entity <|-- Actor
    Entity <|-- Door
    Entity <|-- NPC
    Entity <|-- Item
    Entity <|-- Stair
    Entity <|-- Shop
    Entity <|-- ActorPart

    Actor <|-- Player
    Actor <|-- Enemy

    UIComponent <|-- DialogueUI
    UIComponent <|-- ShopUI
    UIComponent <|-- FlyUI
    UIComponent <|-- NoticeUI
    UIComponent <|-- ItemNoticeUI
    UIComponent <|-- EnemyBookUI
    UIComponent <|-- BattleUI
    UIComponent <|-- EndSceneUI
    UIComponent <|-- StatusUI
    UIComponent <|-- BackgroundUI
```

以下的點代表繼承、數字代表詳細解釋

* `Util::GameObject` - PTSD 中的基礎遊戲物件
  * `Entity` - 地圖上所有互動物件的通用基底
    * `MapBlock` - 地圖上的靜態障礙物 (牆壁、岩漿、水)
    * `Actor` - 擁有生命與攻防等數值屬性的動態角色
      * `Player` - 玩家角色
      * `Enemy` - 地圖上的怪物
    * `Door` - 地圖上需要消耗鑰匙開啟的門
    * `NPC` - 地圖上可對話的角色
    * `Item` - 地圖上的消耗性或裝備道具
    * `Stair` - 地圖上的樓梯 (負責樓層切換)
    * `Shop` - 地圖上的商店
    * `ActorPart` - 巨型敵人的身體部位
  * `NumericDisplayText` - 顯示數字的文字元件 (StatusUI 的子元件)
* `UIComponent` - 所有使用者介面的通用基底，管理渲染與生命週期
  * `DialogueUI` - 負責對話文本與選項渲染
  * `ShopUI` - 商店的商品選單
  * `FlyUI` - 樓層跳躍介面 (隨意門)
  * `NoticeUI` - 中央提示訊息框
  * `ItemNoticeUI` - 獲得道具時的提示框
  * `EnemyBookUI` - 怪物圖鑑 (計算攻防傷害與預估耗血)
  * `BattleUI` - 戰鬥畫面
  * `EndSceneUI` - 遊戲結局畫面
  * `StatusUI` - 側邊狀態欄 (顯示血量、鑰匙、攻防)
  * `BackgroundUI` - 遊戲背景層 (取代原本的靜態背景圖片)

### 程式技術

1. **直接回調技術 (Direct Callback)**
   透過 C++11 的 `std::function` 與 `Lambda` 實現依賴注入 (Dependency Injection)。凡是需要「跨系統通訊」的地圖互動（如 NPC 觸發對話、商店開啟 UI），皆統一在工廠階段注入 Callback。這免除了 `Update()` 迴圈中無效的狀態輪詢 (Polling)，達到**瞬間響應 (Zero-Latency)**，且徹底讓地圖物件與 UI 邏輯解耦。對於不需要外部系統介入的實體（如 `Door` 開門），則保留權限最小化，不注入 Callback 確保輕量。

2. **3D 物件矩陣與資料驅動地圖 (Data-Driven Floor Management)**
   運用 `std::vector` 的 3D 陣列打造三維空間索引，並結合 CSV 地圖檔實作**延遲載入 (Lazy Loading)**。樓層切換時，利用物件可見度 (`SetVisible`) 進行瞬時撥動，取代頻繁的記憶體銷毀與重建，保證極高的空間查詢效率 ($O(1)$) 與流暢的地圖切換體驗。

3. **雙軌制動畫系統 (Global Sync & Independent Animations)**
   * **同步動畫 (Synchronous)**：採用全域時鐘，讓地圖上的岩漿、NPC、怪物與道具透過主動對齊 (Pull Mode) 同步閃爍，保持場景整齊不紊亂。
   * **獨立動畫 (Independent)**：將「玩家走路」與「開門」視為事件驅動 (Event-Driven) 的獨立動畫，並將物理狀態與動畫生命週期綁定（如門動畫播完後才自我銷毀），確保操作回饋精準。

4. **防呆記憶體管理與安全防護 (Robust Memory Management)**
   全面採用 `std::shared_ptr`。在傳遞自身指標時嚴格使用 `std::enable_shared_from_this` 搭配 `std::static_pointer_cast`，避免 Double Free 記憶體崩潰。

### 使用到 AI/AI Agent 的部分




## 結語

### 問題與解決方法
### 自評

| 項次 | 項目                   | 完成 |
|------|------------------------|-------|
| 1    | 這是範例 |  V  |
| 2    | 完成專案權限改為 public |  V  |
| 3    | 具有 debug mode 的功能  |    |
| 4    | 解決專案上所有 Memory Leak 的問題  |    |
| 5    | 報告中沒有任何錯字，以及沒有任何一項遺漏  |    |
| 6    | 報告至少保持基本的美感，人類可讀  |    |

### 心得
### 貢獻比例

