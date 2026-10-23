#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "Gas Detection"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_AUTH_TOKEN"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>

char ssid[] = "YOUR_WIFI_SSID";
char pass[] = "YOUR_WIFI_PASSWORD";

const int GAS_SENSOR_PIN = A0;
const int SERVO_PIN = D1;

const int THRESHOLD = 400;
const int CONFIRM_SAMPLES = 3;
const unsigned long SAMPLE_INTERVAL_MS = 1000;
const unsigned long WARMUP_MS = 20000;

const int SERVO_OPEN_ANGLE = 0;
const int SERVO_CLOSED_ANGLE = 90;

Servo myServo;
BlynkTimer timer;

int overThresholdCount = 0;
bool valveClosed = false;

void closeValve() {
  myServo.write(SERVO_CLOSED_ANGLE);
  valveClosed = true;
}

void onLeakDetected() {
  Serial.println("Gas Leakage Detected!");
  closeValve();
  Blynk.logEvent("gas_leak", "Gas leakage detected. Valve closed.");
}

void sampleGas() {
  int gasValue = analogRead(GAS_SENSOR_PIN);

  Serial.print("Gas Value: ");
  Serial.println(gasValue);
  Blynk.virtualWrite(V0, gasValue);

  if (millis() < WARMUP_MS || valveClosed) {
    return;
  }

  if (gasValue > THRESHOLD) {
    overThresholdCount++;
    if (overThresholdCount >= CONFIRM_SAMPLES) {
      onLeakDetected();
    }
  } else {
    overThresholdCount = 0;
  }
}

void setup() {
  Serial.begin(115200);

  myServo.attach(SERVO_PIN);
  myServo.write(SERVO_OPEN_ANGLE);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(SAMPLE_INTERVAL_MS, sampleGas);
}

void loop() {
  Blynk.run();
  timer.run();
}
