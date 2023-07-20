#include <M5Atom.h>
#include "AtomMotion.h"
#include <WiFi.h>
#include <ArduinoOSCWiFi.h>

#define SPEED 60

const char* ssid = "IODATA-34a3cc-2G";   // SSID
const char* password = "DALtn76754562";  // PW
float i;
int m_speed;

AtomMotion Atom;

String ipToString(uint32_t ip) {
  String result = "";

  result += String((ip & 0xFF), 10);
  result += ".";
  result += String((ip & 0xFF00) >> 8, 10);
  result += ".";
  result += String((ip & 0xFF0000) >> 16, 10);
  result += ".";
  result += String((ip & 0xFF000000) >> 24, 10);

  return result;
}

void setup() {
  M5.begin(true, false, true);
  Atom.Init();

  // シリアルコンソールの開始　Start serial console.
  Serial.begin(115200);
  delay(500);

  // Wi-Fi接続 We start by connecting to a WiFi network
  Serial.println();  // シリアルポート経由でPCのシリアルモニタに出力
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);  // Wi-Fi接続開始
  // Wi-Fi接続の状況を監視（WiFi.statusがWL_CONNECTEDになるまで繰り返し
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Wi-Fi接続結果をシリアルモニタへ出力
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  OscWiFi.subscribe(54414, "/motor_on", i);  // 受信したOSCパケットを直接変数にバインドします
  OscWiFi.subscribe(54414, "/speed", m_speed);
}

void loop() {
  OscWiFi.update();  // 自動的に送受信するために必須
  int speed;

  M5.update();

  delay(100);

  if (M5.BtnA.wasPressed()) {
    OscWiFi.send("192.168.0.2", 54415, "/" + ipToString(WiFi.localIP()), 1);
  }

  if (M5.BtnA.wasReleased()) {
    OscWiFi.send("192.168.0.2", 54415, "/" + ipToString(WiFi.localIP()), 0);
  }

  speed = m_speed;

  if (i == 1) {
    OscWiFi.send("192.168.0.2", 54415, "/speed_check", speed);
    Atom.SetMotorSpeed(1, speed);
  } else {
    Atom.SetMotorSpeed(1, 0);
  }
}