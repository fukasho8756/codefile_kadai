#define echoPin 33
#define trigPin 17
#define bzPIN 32
#define ledPin 26
#define BEAT 300
#define on HIGH
#define off LOW

#define DO 261.6
#define MI 329.63
#define octDO 523.251

double Duration = 0;
double Distance = 0;
byte val;

void playmusic() {
    ledcWriteTone(0, MI);
    delay(BEAT);
    ledcWriteTone(0, DO);
    delay(BEAT);
    ledcWriteTone(0, MI);
    delay(BEAT);
    ledcWriteTone(0, DO);
    delay(BEAT);
    ledcWriteTone(0, MI);
    delay(BEAT);
    ledcWriteTone(0, DO);
    delay(BEAT);
    ledcWriteTone(0, 0); // no sound
    delay(BEAT);
}

bool isPlaying = false;

void stopMusic() {
    ledcWriteTone(0, 0); // no sound
    delay(BEAT);
    isPlaying = false;
}

void setup() {
    pinMode(ledPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(trigPin, OUTPUT);
    ledcSetup(0, 12000, 8);
    ledcAttachPin(bzPIN, 0);
    Serial.begin(115200);
}

void loop() {
    digitalWrite(ledPin, on);
    delay(100);
    if (Serial.available() > 0) {
        val = Serial.read();
        if (val == 'A') {
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

                if (Distance >= 40 && Distance <= 100) {
                    digitalWrite(ledPin, on);
                    playmusic();
                    digitalWrite(ledPin, off);
                    delay(2000); 
                    stopMusic();
                    isPlaying = true;
                } else {
                    digitalWrite(ledPin, on);
                    stopMusic();
                    isPlaying = true;
                }
            }
        } else {
            Serial.println("ラズパイ検知なし");
            digitalWrite(ledPin, on);
            stopMusic();
            isPlaying = true;
        }
    }
}
