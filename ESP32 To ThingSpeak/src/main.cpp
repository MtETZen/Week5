#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// 1. ตั้งค่า Wi-Fi 
const char* ssid = "Zen";
const char* password = "0954686204zen25";

// 2. ตั้งค่า ThingSpeak MQTT
const char* mqtt_server = "mqtt3.thingspeak.com";
// ⚠️ ก๊อปปี้รหัส 3 บรรทัดนี้มาจากเว็บ ThingSpeak ตอนสร้าง Device MQTT
const char* mqtt_client_id = "JD0iKxUSOhUUGzgtERUwDAo";
const char* mqtt_username  = "JD0iKxUSOhUUGzgtERUwDAo";
const char* mqtt_password  = "Rn8lOuX1hrFtseY4bRPRlYpZ";

// ⚠️ ใส่เลข Channel ID ของ ThingSpeak
String channel_ID = "3433774"; 

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
const long interval = 20000; // ส่งทุกๆ 20 วินาที (ThingSpeak บังคับห้ามส่งถี่กว่า 15 วิ)

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
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting ThingSpeak MQTT connection...");
    
    // เชื่อมต่อโดยใช้ Client ID, Username และ Password ของ ThingSpeak
    if (client.connect(mqtt_client_id, mqtt_username, mqtt_password)) {
      Serial.println("connected");
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
  
  // ชี้เป้าไปที่เซิร์ฟเวอร์ ThingSpeak พอร์ต 1883
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

    // 1. สุ่มค่า 2 ค่า
    int sensorValue1 = random(1, 20);  
    int sensorValue2 = random(1, 20); 

    // 2. จัดรูปแบบ Topic สำหรับส่งขึ้น ThingSpeak
    // รูปแบบคือ: channels/<channelID>/publish
    String topic = "channels/" + channel_ID + "/publish";

    // 3. จัดรูปแบบข้อความ (Payload) สำหรับส่งหลาย Field พร้อมกัน
    // รูปแบบคือ: field1=ค่าที่1&field2=ค่าที่2
    String payload = "field1=" + String(sensorValue1) + "&field2=" + String(sensorValue2);

    // 4. สั่งส่งข้อมูล
    client.publish(topic.c_str(), payload.c_str());

    // แสดงผลบนหน้าจอเพื่อเช็กการทำงาน
    Serial.println("=================================");
    Serial.println("Data Published to ThingSpeak!");
    Serial.print("Topic: ");
    Serial.println(topic);
    Serial.print("Payload: ");
    Serial.println(payload);
    Serial.println("=================================");
  }
}