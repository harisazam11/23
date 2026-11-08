#include <SPI.h>
#include <LoRa.h>

// ----------------- LoRa Setup -----------------
#define LORA_SCK 18
#define LORA_MISO 19
#define LORA_MOSI 23
#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 26
#define LORA_BAND 433E6

// Buzzer pin
#define BUZZER_PIN 13

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("✔ Starting LoRa Receiver...");

  // Setup LoRa pins
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(LORA_BAND)) {
    Serial.println("❌ LoRa init failed. Check wiring.");
    while (true);
  }
  Serial.println("✔ LoRa Receiver ready!");

  // Buzzer pin setup
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize > 0) {
    String incoming = "";
    while (LoRa.available()) {
      incoming += (char)LoRa.read();
    }

    Serial.println(incoming); // ✔ This prints the actual received JSON to Serial
  }

  // Try parsing the JSON
  int ch4 = -1, co = -1, nh3 = -1;

  // Extract values using simple string parsing
  int ch4Index = incoming.indexOf("\"CH4\":\"");
  int coIndex = incoming.indexOf("\"CO\":\"");
  int nh3Index = incoming.indexOf("\"NH3\":\"");

  if (ch4Index != -1 && coIndex != -1 && nh3Index != -1) {
    ch4 = incoming.substring(ch4Index + 6, incoming.indexOf("\",", ch4Index)).toInt();
    co = incoming.substring(coIndex + 5, incoming.indexOf("\",", coIndex)).toInt();
    nh3 = incoming.substring(nh3Index + 6, incoming.indexOf("\"}", nh3Index)).toInt();

    Serial.printf("CH4: %d ppm, CO: %d ppm, NH3: %d ppm\n", ch4, co, nh3);

    // Trigger buzzer if dangcrous levels
    if (ch4 > 200 || co > 200 || nh3 > 200) {
      digitalWrite(BUZZER_PIN, HIGH);
      Serial.println("🚨 Alert: Gas level exceeded!");
    } else {
      digitalWrite(BUZZER_PIN, LOW);
    }
  } else {
    Serial.println("⚠ Invalid JSON format.");
  }
}