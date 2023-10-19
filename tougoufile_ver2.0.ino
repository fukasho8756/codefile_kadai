#define InputPin 32
#define echoPin 33
#define trigPin 17
#define bzPIN 13
#define ledPin 26
#define on HIGH
#define off LOW

#define DO 261.6
#define MI 329.63

#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "AP01-01";
const char* password = "1qaz2wsx";

const String endpoint = "https://maker.ifttt.com/trigger/"; // IFTTTのエンドポイントURL
const String eventName = "SchooMyIoT"; // IFTTTのイベント名
const String conn = "/with/key/";
const String Id = "jX0-XczR5iRUlvn0m2uDaUdoi5nu0iN1-TI9cf9iVPk"; // あなたのIFTTTのキー
const String value = "?value1="; // パラメータの値を指定（必要に応じてvalue2, value3を追加）


double Duration = 0;
double Distance = 0;
byte val;
int inp;

void playmusic() {
    ledcWriteTone(0, MI);
    delay(500);
    ledcWriteTone(0, DO);
    delay(500);
    ledcWriteTone(0, 0); // no sound
    delay(100);
    //音楽再生。
}

bool isPlaying = false;

void stopMusic() {
    ledcWriteTone(0, 0); // no sound
    delay(100);
    isPlaying = false;
    //音楽停止
}

void setup() {
    pinMode(ledPin, OUTPUT);
    pinMode(ledPin, on);
    pinMode(echoPin, INPUT);
    pinMode(trigPin, OUTPUT);
    ledcSetup(0, 12000, 8);
    ledcAttachPin(bzPIN, 0);
    pinMode(InputPin, INPUT);
    Serial.begin(115200);

    // WiFi接続
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
}
int SensAve(){
  int i;
  int sval=0;
  int ave=0;
  for (i = 0; i < 10000; i++){
    sval = sval + digitalRead(InputPin);
  }
  ave = sval / 100;
  return ave;
  //衝撃検知センサーのセット
}

void loop() {
  {
  inp = SensAve();
  Serial.println(inp);
  if (inp >= 80 && inp <= 99) { 
    sendIFTTTMessage();
    //衝撃検知センサー起動。
  }else{}
  delay(100); 
}

{
    digitalWrite(ledPin, on);//振動センサー待機状態
    delay(100);
    if (Serial.available() > 0) {//ラズパイ側から送られてきている信号の受け取り。
        val = Serial.read();//変数valにラズパイからの信号を代入
        if (val == 'A') {//変数valが'A'と同値だった場合以下プログラムの開始
        while(true){
            Serial.println("ラズパイ検知");
            digitalWrite(trigPin, LOW);
            delayMicroseconds(2);
            digitalWrite(trigPin, HIGH);
            delayMicroseconds(10);
            digitalWrite(trigPin, LOW);
            Duration = pulseIn(echoPin, HIGH);

            if (Duration > 0) {
                Duration = Duration / 2;
                Distance = Duration * 340 * 100 / 1000000;
                Serial.print("Distance:");
                Serial.print(Distance);
                Serial.println("cm");

                if (Distance >= 20 && Distance <= 40) {
                    digitalWrite(ledPin, on);
                    playmusic();
                    digitalWrite(ledPin, off);
                    delay(1000); 
                    stopMusic();
                    isPlaying = true;
                    break;
                } else {
                    digitalWrite(ledPin, on);
                    stopMusic();
                    isPlaying = true;
                }
            }
            digitalWrite(ledPin, on);
            delay(100);
            }
        } else {
            Serial.println("ラズパイ検知なし");
            digitalWrite(ledPin, on);
            stopMusic();
            isPlaying = true;
        }
    }
}
}

void sendIFTTTMessage() {
   if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(endpoint + eventName + conn + Id + value); //URLを指定

    http.addHeader("Content-Type", "application/x-www-form-urlencoded"); //必要なヘッダーを追加
    int httpCode = http.GET();  //GETリクエストを送信

    if (httpCode == 200) { //返答がある場合
      Serial.println("IFTTT Message Sent: 200 OK");
    } else {
      Serial.println("Error on HTTP request");
    }
    http.end(); //Free the resources
  }
}
