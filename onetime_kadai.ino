#include <Wire.h>
#include <SSD1306.h>
#include <WiFi.h>
#include <HTTPClient.h>

SSD1306 display(0x3c, 4, 13); // OLEDディスプレイの設定

#define switchPin 32
#define on HIGH
#define off LOW
int swState = 0;

const char* ssid = "xxxx";
const char* password = "xxxx";

const String endpoint = "https://maker.ifttt.com/trigger/";
const String eventName = "SchooMyIoT";
const String conn = "/with/key/";
const String Id = "xxxxxxxxxxxxxx";
const String value = "?value1=";

long randomNumber;
long inputNumber = -1; // シリアルからの入力を保持

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  pinMode(switchPin, INPUT_PULLUP);

  // OLEDディスプレイの初期化
  display.init();
  display.flipScreenVertically(); // 必要に応じてディスプレイを反転
  display.setFont(ArialMT_Plain_24); // フォントを選択
}

void loop() {
  int switchState = digitalRead(switchPin);
  if (switchState == LOW) {
    generateRandomValue();

    if (sendDataToIFTTT(randomNumber)) {
      Serial.println("成功");
    } else {
      Serial.println("失敗");
    }

    display.clear();
    display.display();

    delay(5000);
  }

  // シリアルからの入力を処理
  if (Serial.available() > 0) {
    inputNumber = Serial.parseInt();
  }

  // 入力された乱数と生成された乱数が一致した場合に "login" を表示
  if (inputNumber == randomNumber) {
    display.clear();
    display.drawString(0, 0, "login");
    display.display();
    inputNumber = -1; // 一致したらリセット
  }
}

void generateRandomValue() {
  randomNumber = random(1000, 9999);
}

bool sendDataToIFTTT(long sensorValue) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = endpoint + eventName + conn + Id + value + String(sensorValue);

    http.begin(url);
    int httpCode = http.GET();

    if (httpCode == 200) {
      Serial.println("200.OK");
      http.end();
      return true;
    } else {
      Serial.println("Error on HTTP request");
      http.end();
      return false;
    }
  } else {
    Serial.println("WiFi not connected");
    return false;
  }
}