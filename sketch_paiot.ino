// 1. Identitas Blynk
#define BLYNK_TEMPLATE_ID   "TMPL6i04w4zgH"
#define BLYNK_TEMPLATE_NAME "PA IoT"
#define BLYNK_AUTH_TOKEN    "My6DIO-dzH-2Sx2SqOI4sOYaTe9zXyps"

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>         // <--- MENGGUNAKAN LIBRARY MQTT
#include <UniversalTelegramBot.h> 
#include <ArduinoJson.h>
#include "DHT.h"

// 2. Konfigurasi WiFi & Telegram
char ssid[] = "Vicky_Ahmad";     
char pass[] = "Anakkesayangan"; 

#define BOTtoken "8626055861:AAGe4TZBnHPuPWffVNk28aADgkqlf1Gk-LY" 
#define CHAT_ID  "5877859290" 

// --- Client Network ---
WiFiClientSecure client_secure;                     // Untuk Telegram (HTTPS)
UniversalTelegramBot bot(BOTtoken, client_secure);

WiFiClient espClient;                               // Untuk MQTT (TCP)
PubSubClient mqttClient(espClient);

// --- Konfigurasi MQTT Blynk ---
const char* mqtt_server = "blynk.cloud";
const int mqtt_port = 1883;

// --- Timer ---
unsigned long lastTimeBotRan = 0;
int delayBot = 1000; 

unsigned long lastTimeSensorRan = 0;
int delaySensor = 3000; // Pengganti delay(3000)

// 3. Definisi Pin Komponen
#define DHTPIN 13          
#define FLAME_PIN 25      
#define MQ2_PIN 34         
#define BZ1 26            
#define BZ2 27             
#define LED_RED 5          

#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

bool sistemAktif = true;

// ================= FUNGSI TELEGRAM =================
void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Halo " + from_name + ", Selamat Datang di Bot PA IoT.\n";
      welcome += "Daftar Perintah:\n/status : Cek kondisi sensor";
      bot.sendMessage(chat_id, welcome, "");
    }
    
    if (text == "/status") {
      if (sistemAktif) {
        float t = dht.readTemperature();
        int asap = analogRead(MQ2_PIN);
        String stat = "Suhu: " + String(t) + " C\nAsap: " + String(asap);
        bot.sendMessage(chat_id, stat, "");
      } else {
        bot.sendMessage(chat_id, "Sistem saat ini sedang dimatikan via Blynk.", "");
      }
    }
  }
}

// ================= FUNGSI MQTT CALLBACK (Pengganti BLYNK_WRITE) =================
// Fungsi ini berjalan otomatis ketika ada pesan masuk dari Blynk (misal tombol ditekan)
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  
  // Mengecek apakah pesan datang dari topik Datastream V0 (Tombol Aktif/Mati)
  if (String(topic) == "downlink/ds/V0") {
    sistemAktif = msg.toInt(); 
    Serial.print("Perintah Blynk Diterima! sistemAktif = ");
    Serial.println(sistemAktif);
    
    if (sistemAktif == 0) {
      digitalWrite(BZ1, HIGH); 
      digitalWrite(BZ2, HIGH); 
      digitalWrite(LED_RED, LOW);
    }
  }
}

// ================= FUNGSI KONEKSI MQTT =================
void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Menghubungkan ke MQTT Blynk...");
    // Connect menggunakan Username "device" dan Password "BLYNK_AUTH_TOKEN"
    if (mqttClient.connect("ESP32_PA_IoT", "device", BLYNK_AUTH_TOKEN)) {
      Serial.println("Berhasil!");
      // Wajib subscribe ke downlink untuk menerima perintah dari Blynk (V0)
      mqttClient.subscribe("downlink/#");
    } else {
      Serial.print("Gagal, status=");
      Serial.print(mqttClient.state());
      Serial.println(" Coba lagi 5 detik...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  // Setup WiFi
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");

  client_secure.setInsecure(); // Untuk Telegram
  
  // Setup MQTT
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(mqttCallback);
  
  dht.begin();
  
  pinMode(FLAME_PIN, INPUT);
  pinMode(BZ1, OUTPUT);
  pinMode(BZ2, OUTPUT); 
  pinMode(LED_RED, OUTPUT);

  digitalWrite(BZ1, HIGH);
  digitalWrite(BZ2, HIGH);
  digitalWrite(LED_RED, LOW);

  Serial.println("Sistem Siap! Monitoring Serial, MQTT Blynk & Telegram Aktif.");
}

void loop() {
  // Jaga koneksi MQTT
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();

  // Cek pesan Telegram masuk
  if (millis() > lastTimeBotRan + delayBot)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while(numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }

  // Siklus Baca Sensor & Publish MQTT (Berjalan setiap 3 detik tanpa menghentikan sistem)
  if (millis() > lastTimeSensorRan + delaySensor) {
    if (sistemAktif) {
      float t = dht.readTemperature();
      int asap = analogRead(MQ2_PIN);
      int api = digitalRead(FLAME_PIN);

      Serial.print("Suhu: "); Serial.print(t);
      Serial.print(" C | Asap: "); Serial.print(asap);
      Serial.print(" | Api: "); Serial.println(api == LOW ? "ADA API!" : "Aman");

      // PUBLISH ke MQTT Blynk (Pengganti Blynk.virtualWrite)
      mqttClient.publish("ds/V1", String(t).c_str());
      mqttClient.publish("ds/V2", String(asap).c_str());

      if (api == LOW || asap > 1800 || t > 50.0) {
        mqttClient.publish("ds/V3", "!!! BAHAYA !!!");
        bot.sendMessage(CHAT_ID, "PERINGATAN BAHAYA: Api atau Asap terdeteksi!", "");
        
        digitalWrite(BZ1, LOW);
        digitalWrite(BZ2, LOW); 
        digitalWrite(LED_RED, HIGH);
      } else {
        mqttClient.publish("ds/V3", "Aman");
        
        digitalWrite(BZ1, HIGH);
        digitalWrite(BZ2, HIGH); 
        digitalWrite(LED_RED, LOW);
      }
    } else {
      Serial.println("Sistem Dimatikan via Aplikasi");
      mqttClient.publish("ds/V1", "0");
      mqttClient.publish("ds/V2", "0");
      mqttClient.publish("ds/V3", "Sistem Mati");
    }
    
    lastTimeSensorRan = millis();
  }
}