/*
 * ============================================================
 *  EcoSort — Smart Trash Sorting Bin
 *  Firmware ESP32
 * ============================================================
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

// konfigurasi WiFi dan MQTT
const char* WIFI_SSID     = "NAMA_WIFI_KAMU";
const char* WIFI_PASSWORD = "PASSWORD_WIFI_KAMU";

const char* MQTT_SERVER   = "broker.hivemq.com";
const int   MQTT_PORT     = 1883;
const char* MQTT_CLIENT   = "EcoSort-ESP32-001"

// ============================================================
//  PIN MAPPING
// ============================================================
const int PIN_TRIG  = 5;
const int PIN_ECHO  = 18;
const int PIN_SOIL  = 34;
const int PIN_SERVO = 13;

// ============================================================
//  PARAMETER SISTEM
// ============================================================
const int  ULTRA_DISTANCE  = 15;
const int  HUMIDITY_THRESH = 30;
const int  SERVO_TENGAH    = 90;
const int  SERVO_ORGANIK   = 170;
const int  SERVO_ANORGANIK = 10;
const long SERVO_HOLD_MS   = 3000;

// ============================================================
//  TOPIK MQTT
// ============================================================
const char* TOPIC_SENSOR = "ecosort/sensor";
const char* TOPIC_SERVO  = "ecosort/servo";
const char* TOPIC_STATUS = "ecosort/status";

// ============================================================
//  OBJEK GLOBAL
// ============================================================
WiFiClient   espClient;
PubSubClient mqtt(espClient);
Servo        servo1;

// ============================================================
//  FEATURE: NETWORK — WiFi + MQTT
// ============================================================
void setup_wifi() {
  Serial.println("\n[WiFi] Menghubungkan ke: " + String(WIFI_SSID));
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempt = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    attempt++;
    if (attempt > 40) {
      Serial.println("\n[WiFi] GAGAL! Restart...");
      ESP.restart();
    }
  }

  Serial.println();
  Serial.println("[WiFi] Terhubung!");
  Serial.print("[WiFi] IP Address: ");
  Serial.println(WiFi.localIP());
}

void reconnect_mqtt() {
  int attempt = 0;
  while (!mqtt.connected()) {
    Serial.print("[MQTT] Menghubungkan...");

    if (mqtt.connect(MQTT_CLIENT)) {
      Serial.println(" Terhubung!");
      mqtt.publish(TOPIC_STATUS, "{\"status\":\"online\"}");
    } else {
      Serial.print(" Gagal (rc=");
      Serial.print(mqtt.state());
      Serial.println(") Coba lagi 3 detik...");
      delay(3000);
      attempt++;
      if (attempt > 10) ESP.restart();
    }
  }
}