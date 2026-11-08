#include <SPI.h>
#include <LoRa.h>
#include <math.h>

// ----------------- LoRa Setup -----------------
#define LORA_SCK 18
#define LORA_MISO 19
#define LORA_MOSI 23
#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 26
#define LORA_BAND 433E6

// ----------------- MQ Sensors -----------------
#define MQ4_PIN 34 // Methane (CH4)
#define MQ7_PIN 35 // Carbon Monoxide (CO)
#define MQ135_PIN 32 // Smoke, NH3

// Calibrated RO values
#define MQ4_RO 784.0
#define MQ7_RO 530.0
#define MQ135_RO 582.0

// Buzzer pin
#define BUZZER_PIN 13

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("✔ Starting LoRa Sender...");

  // LoRa pin setup
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(LORA_BAND)) {
    Serial.println("❌ LoRa init failed. Check wiring.");
    while (true);
  }
  Serial.println("✔ LoRa initialized!");

  // Buzzer pin
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
}

// Function to read and calculate ppm (basic approximation)
float getPPM(int analogValue, float RO, float cleanAirRatio = 10.0) {
  float RL = 10.0; // Load resistance in kilo ohms
  float sensorVoltage = analogValue * (3.3 / 4095.0);
  float RS = (3.3 - sensorVoltage) * RL / sensorVoltage;
  float ratio = RS / RO;
  float ppm = pow(10, ((-log10(ratio) + log10(cleanAirRatio)) * 1.0));
  return ppm;
}

void loop() {
  int rawCH4 = analogRead(MQ4_PIN);
  int rawCO = analogRead(MQ7_PIN);
  int rawSmoke = analogRead(MQ135_PIN);

  float ch4 = getPPM(rawCH4, MQ4_RO, 4.4); // Methane (MQ-4)
  float co = getPPM(rawCO, MQ7_RO, 9.8);   // CO (MQ-7)
  float nh3 = getPPM(rawSmoke, MQ135_RO, 3.6); // MQ-135 (NH3/smoke)

  // Trigger buzzer if danger
  if (ch4 > 200 || co > 200 || nh3 > 200) {
    digitalWrite(BUZZER_PIN, HIGH);
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }

  // Create JSON string
  String json = "{";
  json += "\"CH4\":\"" + String((int)ch4) + "\",";
  json += "\"CO\":\"" + String((int)co) + "\",";
  json += "\"NH3\":\"" + String((int)nh3);
  json += "}";

  // Send via LoRa
  LoRa.beginPacket();
  LoRa.print(json);
  LoRa.endPacket();

  Serial.println("↑ Sent: " + json);

  delay(5000);
}