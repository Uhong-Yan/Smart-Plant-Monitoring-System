# 智慧植物監測系統 (Smart Plant Monitoring System)

本專案為「物聯網概論」課程之期末實作成果。開發目標是透過 **LinkIt 7697** 整合多種感測器，監測植物生長的環境數據（溫濕度、光照、土壤水分），並透過 Wi-Fi 將資料同步至 **ThingSpeak** 雲端平台進行長期觀察。

## 專案演示與文檔
本倉庫保留了開發過程中的完整記錄，包含程式碼與調試報告：

*  **[核心程式碼 (Arduino .ino)](Intelligent_Monitoring_System_for_Plant.ino)**：包含感測器讀取邏輯與雲端 API 串接實作。
*  **[期末專題報告 (PDF)](Final_Project_Report_Smart_Plant.pdf)**：內含實作截圖與**跨環境調試紀錄（超音波感測器排錯過程）**。
*  **[專題進度報告 (PDF)](Progress_Report_Smart_Plant.pdf)**：初期開發規劃與硬體測試紀錄。

## 核心功能
* **環境數據採集**：即時讀取溫度、濕度、光強度、土壤濕度及超音波測距數據。
* **即時警示機制**：
  - **土壤缺水提醒**：當土壤濕度低於門檻時，自動點亮紅色 LED 警示。
  - **近距離互動**：根據超音波感測距離，調整 LED 閃爍頻率（快/慢）。
* **雲端數據同步**：利用行動熱點 Wi-Fi 連線，將數據定期上傳至 ThingSpeak 平台。

## 技術實作與個人貢獻
在專案中，我負責軟硬體整合測試與網路通訊開發，重點在於確保數據能從硬體穩定傳輸至雲端：

1. **網路通訊與雲端串接**：
   - 實作 **LWiFi 庫** 的連線邏輯，透過行動熱點（iPhone SSID）建立 Wi-Fi 連結。
   - 撰寫 **HTTP GET 請求** 封裝，將五項感測數據上傳至 ThingSpeak API。
2. **硬體調試與環境測試**：
   - 負責 DHT22 與超音波感測器的基本功能測試與門檻值設定。
   - 參與系統在不同環境（如教室與居家環境）的部署測試，並針對超音波感測器於特定環境失效的問題進行初步排查。
3. **基礎邏輯編寫**：
   - 實作 `handleUltrasonicFeedback` 函數，將超音波數值轉化為簡單的 LED 閃爍提示。

## 開發技術棧
* **硬體平台**：LinkIt 7697
* **開發環境**：Arduino IDE (C++)
* **雲端平台**：ThingSpeak (IoT Analytics)
* **通訊協議**：HTTP / Wi-Fi (LWiFi)
* **感測技術**：DHT22, Ultrasonic, Soil Moisture, Photoresistor
