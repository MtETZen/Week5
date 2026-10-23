#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// 1. ตั้งค่า Wi-Fi
const char* ssid = "Zen";
const char* password = "0954686204zen25";

// 2. ตั้งค่า MQTT Broker (Mosquitto)
// ⚠️ เปลี่ยนเป็น IP Address เครื่องคอมพิวเตอร์ของคุณ (ที่รัน Mosquitto)
const char* mqtt_server = "172.20.10.4"; 

WiFiClient espClient;
PubSubClient client(espClient);

// ตัวแปรสำหรับจัดการเวลา (ไม่ต้องใช้ delay)
unsigned long lastMsg = 0;
const long interval = 15000; // ส่งข้อมูลทุกๆ 15 วินาที (15000 มิลลิวินาที)

// ฟังก์ชันสำหรับเชื่อมต่อ Wi-Fi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
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

// ฟังก์ชันสำหรับเชื่อมต่อ MQTT ซ้ำกรณีหลุด
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-Zen";
    
    // พยายามเชื่อมต่อ (ถ้า Mosquitto ไม่ได้ตั้งรหัสผ่าน ปล่อยว่างไว้แบบนี้ได้เลย)
    if (client.connect(clientId.c_str())) {
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
  
  // ชี้เป้าไปที่เซิร์ฟเวอร์ Mosquitto (พอร์ตมาตรฐานคือ 1883)
  client.setServer(mqtt_server, 1883); 
}

void loop() {
  // ตรวจสอบว่า MQTT หลุดไหม ถ้าหลุดให้ต่อใหม่
  if (!client.connected()) {
    reconnect();
  }
  // ฟังก์ชันสำคัญ! ต้องมีเพื่อให้ไลบรารี MQTT ทำงานอยู่เบื้องหลังได้
  client.loop();

  // ระบบจับเวลา: ถ้าเวลาปัจจุบัน ลบ เวลาที่ส่งครั้งล่าสุด มากกว่า interval (5 วินาที)
  unsigned long now = millis();
  if (now - lastMsg > interval) {
    lastMsg = now;

    // สุ่มค่า 2 ค่า
    int sensorValue1 = random(10, 50);  
    int sensorValue2 = random(10, 50); 

    // แปลงค่าตัวเลขเป็น String ก่อนส่ง
    String msg1 = String(sensorValue1);
    String msg2 = String(sensorValue2);

    // ส่งค่า (Publish) ไปที่ Topic ที่กำหนด
    client.publish("zen/sensor1", msg1.c_str());
    client.publish("zen/sensor2", msg2.c_str());

    // แสดงผลบน Serial Monitor 
    Serial.print("Published -> Topic: zen/sensor1 | Value: ");
    Serial.println(msg1);
    Serial.print("Published -> Topic: zen/sensor2 | Value: ");
    Serial.println(msg2);
    Serial.println("-------------------------");
  }
}