#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// 1. ตั้งค่า Wi-Fi 
const char* ssid = "Zen";
const char* password = "0954686204zen25";

// 2. ตั้งค่า Local MQTT Broker (Mosquitto)
const char* mqtt_server = "172.20.10.4"; // IP ของคอมพิวเตอร์ที่รัน Mosquitto

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
const long interval = 5000; // ส่งข้อมูลทุกๆ 5 วินาที

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    // เชื่อมต่อโดยไม่ใช้ Password (สำหรับ Local Mosquitto)
    String clientId = "ESP32Client-ZenCloudTest";
    if (client.connect(clientId.c_str())) {
      Serial.println("connected to Local Broker!");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  
  // ชี้เป้าไปที่ Local Broker พอร์ต 1883
  client.setServer(mqtt_server, 1883); 
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > interval) {
    lastMsg = now;

    // 1. สุ่มค่า 0-100 ให้พอดีกับหน้าปัด Gauge ใน Node-RED
    int sensorValue = random(0, 101);  

    // 2. แปลงเป็นข้อความ
    String payload = String(sensorValue);

    // 3. ส่งข้อมูลไปที่ Topic ที่ Node-RED รอรับอยู่
    client.publish("zen/sensor1", payload.c_str());

    // แสดงผลบนหน้าจอเพื่อเช็กการทำงาน
    Serial.print("Published to Local MQTT -> Topic: zen/sensor1 | Value: ");
    Serial.println(payload);
  }
}