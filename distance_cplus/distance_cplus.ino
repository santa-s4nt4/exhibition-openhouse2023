#include <M5StickCPlus.h>
#include <WiFi.h>  // Wifi制御用ライブラリ
#include <ArduinoOSCWiFi.h>
#include <Unit_Sonic.h>

const char* ssid = "IODATA-34a3cc-2G";   // SSID
const char* password = "DALtn76754562";  // PW
float i;                                 //OSCのデータ格納用変数

SONIC_IO sensor;

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

// ------------------------------------------------------------
// Setup 関数　Setup function.
// ------------------------------------------------------------
void setup() {
  M5.begin();              // Init M5StickC.
  M5.Axp.ScreenBreath(8);  // 画面の明るさ
  M5.Lcd.setRotation(3);   // Rotating display.

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
  OscWiFi.subscribe(54414, "/test", i);  // 受信したOSCパケットを直接変数にバインド

  sensor.begin(32, 33);
  M5.Lcd.setCursor(0, 0, 4);  // Set the cursor at (0,0) and set the font to a 4 point font.
  M5.Lcd.println(WiFi.localIP());
  M5.Lcd.println("distance02");
}

void loop() {
  M5.update();
  OscWiFi.update();  // 自動的に送受信するために必須

  if (M5.BtnA.wasPressed()) {
    OscWiFi.send("192.168.0.2", 54415, "/" + ipToString(WiFi.localIP()), 1);
  }

  if (M5.BtnA.wasReleased()) {
    OscWiFi.send("192.168.0.2", 54415, "/" + ipToString(WiFi.localIP()), 0);
  }

  static float distance = 0;
  distance = sensor.getDistance();
  if (distance > 20) {
    M5.Lcd.setCursor(0, 50, 4);
    M5.Lcd.printf("%.2fmm", distance);
    OscWiFi.send("192.168.0.2", 54415, "/d" + ipToString(WiFi.localIP()), distance);
  }
  delay(100);
}
