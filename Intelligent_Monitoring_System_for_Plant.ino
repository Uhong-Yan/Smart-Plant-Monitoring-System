#include "Ultrasonic.h"   

#include <DHT.h>          

#include <LWiFi.h>        

  

// 定義感測器和裝置的接腳 

#define DHTPIN 4              // DHT22 溫濕度感測器資料腳位 

#define DHTTYPE DHT22         // 指定 DHT22 型號 

#define LIGHT_SENSOR_PIN A0   // 光感測器模組接腳 

#define SOIL_MOISTURE_PIN A1  // 土壤濕度感測器模組接腳 

#define TRIGGER_PIN  12       // 超音波感測器觸發腳位 

#define ECHO_PIN     13       // 超音波感測器回音腳位 

#define LED_BUILTIN 13        // 板載 LED 

#define LED_PIN_RED 7         // 外接紅色 LED 腳位 

  

// ThingSpeak API 配置 

#define HOST "api.thingspeak.com"  // ThingSpeak 伺服器位址 

#define PORT 80                   // HTTP 通訊使用的埠號 

char ssid[] = "iPhone";     // Wi-Fi 名稱 

char pass[] = "ruby0907";        // Wi-Fi 密碼 

String apiKey = "SLRH8CI4CUCKDKYX"; // ThingSpeak 的 API Key 

WiFiClient client;                // Wi-Fi 客戶端物件 

  

DHT dht(DHTPIN, DHTTYPE);         // 初始化 DHT22 感測器 

Ultrasonic ultrasonic(TRIGGER_PIN, ECHO_PIN); // 初始化超音波感測器 

  

void setup() { 

    Serial.begin(9600);  // 初始化序列埠通訊 

    dht.begin();         // 啟動 DHT22 感測器 

    pinMode(LIGHT_SENSOR_PIN, INPUT); 

    pinMode(SOIL_MOISTURE_PIN, INPUT); 

    pinMode(LED_BUILTIN, OUTPUT); 

    pinMode(LED_PIN_RED, OUTPUT); 

    digitalWrite(LED_BUILTIN, LOW); 

    digitalWrite(LED_PIN_RED, LOW); 

  

    // 初始化 Wi-Fi 連線 

    while (WiFi.begin(ssid, pass) != WL_CONNECTED) { 

        delay(500); // 每次嘗試連線後延遲 500 毫秒 

        Serial.print("."); 

    } 

    Serial.println("\nWi-Fi connected"); 

    printWifiStatus();  // 打印連線狀態 

    Serial.println("System started, sensor testing..."); 

} 

  

void loop() { 

    // 讀取感測器數據 

    float humidity = dht.readHumidity();       // 讀取濕度 

    float temperature = dht.readTemperature(); // 讀取溫度 

    int lightLevel = analogRead(LIGHT_SENSOR_PIN);  // 讀取光強度 

    int soilMoisture = analogRead(SOIL_MOISTURE_PIN); // 讀取土壤濕度 

    int distance = ultrasonic.MeasureInCentimeters(); // 讀取超音波測得的距離 

  

    // 檢查溫濕度數據是否有效 

    if (isnan(humidity) || isnan(temperature)) { 

        Serial.println("Failed to read from DHT sensor."); 

        return; // 若數據無效則停止執行 loop 內的其他程式碼 

    } 

  

    // 打印感測器數據到序列埠 

    Serial.print("Temperature: "); 

    Serial.print(temperature); 

    Serial.println(" °C"); 

    Serial.print("Humidity: "); 

    Serial.print(humidity); 

    Serial.println(" %"); 

    Serial.print("Light Intensity: "); 

    Serial.println(lightLevel); 

    Serial.print("Soil Moisture: "); 

    Serial.println(soilMoisture); 

    Serial.print("Distance: "); 

    Serial.println(distance); 

    Serial.println(" cm"); 

  

    // 土壤濕度低於門檻時啟動警報 

    if (soilMoisture < 500) { 

        Serial.println("Warning: Low soil moisture."); 

        digitalWrite(LED_BUILTIN, HIGH); // 啟動板載 LED 

        digitalWrite(LED_PIN_RED, HIGH); // 啟動紅色 LED 

    } else { 

        digitalWrite(LED_BUILTIN, LOW); // 關閉板載 LED 

        digitalWrite(LED_PIN_RED, LOW); // 關閉紅色 LED 

        Serial.println("Soil moisture normal."); 

    } 

  

    // 處理超音波測距反饋 

    handleUltrasonicFeedback(distance); 

  

    // 將數據上傳至 ThingSpeak 

    uploadToThingSpeak(humidity, temperature, lightLevel, soilMoisture, distance); 

  

    delay(15000); // 依據 ThingSpeak 的更新限制，延遲 15 秒 

} 

  

void handleUltrasonicFeedback(int distance) { 

    // 根據測得的距離進行 LED 控制 

    if (distance < 10 && distance > 0) { 

        Serial.println("Close distance detected, fast LED blinking."); 

        for (int i = 0; i < 3; i++) { // 快速閃爍三次 

            digitalWrite(LED_BUILTIN, HIGH); 

            delay(200); 

            digitalWrite(LED_BUILTIN, LOW); 

            delay(200); 

        } 

    } else if (distance >= 10 && distance < 20) { 

        Serial.println("Moderate distance detected, slow LED blinking."); 

        digitalWrite(LED_BUILTIN, HIGH); // 慢速閃爍 

        delay(500); 

        digitalWrite(LED_BUILTIN, LOW); 

        delay(500); 

    } else if (distance >= 20) { 

        Serial.println("Distance normal, LED off."); 

        digitalWrite(LED_BUILTIN, LOW); // 距離正常，關閉 LED 

    } else { 

        Serial.println("No data from ultrasonic sensor."); 

    } 

} 

  

void uploadToThingSpeak(float humidity, float temperature, int lightLevel, int soilMoisture, int distance) { 

    // 建立與 ThingSpeak 的連線並傳送數據 

    if (client.connect(HOST, PORT)) { 

        String getStr = String("GET /update?api_key=") + apiKey + 

                        "&field1=" + String(humidity) + 

                        "&field2=" + String(temperature) + 

                        "&field3=" + String(lightLevel) + 

                        "&field4=" + String(soilMoisture) + 

                        "&field5=" + String(distance) + 

                        " HTTP/1.1\r\nHost: " + HOST + "\r\nConnection: close\r\n\r\n"; 

        client.print(getStr); // 傳送 HTTP GET 請求 

        Serial.println("Data sent to ThingSpeak"); 

  

        // 打印伺服器回應 

        while (client.connected()) { 

            while (client.available()) { 

                char c = client.read(); 

                Serial.print(c); 

            } 

        } 

        client.stop(); // 結束連線 

    } else { 

        Serial.println("Failed to connect to ThingSpeak."); 

    } 

} 

  

void printWifiStatus() { 

    // 打印 Wi-Fi 連線資訊 

    Serial.print("SSID: "); 

    Serial.println(WiFi.SSID()); 

    IPAddress ip = WiFi.localIP(); 

    Serial.print("IP Address: "); 

    Serial.println(ip); 

    long rssi = WiFi.RSSI(); 

    Serial.print("Signal strength (RSSI): "); 

    Serial.print(rssi); 

    Serial.println(" dBm"); 

} 
