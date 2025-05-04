#include <Wire.h>
#include <LiquidCrystal.h>
#include <RTClib.h>
#include <SPI.h>
#include <MFRC522.h>

// LCD pins
const int rs = 26, en = 25, d4 = 14, d5 = 27, d6 = 33, d7 = 32;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// RTC setup
RTC_DS3231 rtc;

String timeStr = "";

// RFID setup
#define SS_PIN 5
#define RST_PIN 22
MFRC522 rfid(SS_PIN, RST_PIN);

// Buzzer and LEDs
const int redLed = 15;
const int greenLed = 2;
const int buzzerPin = 4;

// UID database
struct User {
  String uid;
  String name;
};

User users[] = {
  {"BF 4B 59 1F", "Bharath"},
  {"56 48 71 5F", "Phanith"},
  {"C6 13 5B 5F", "Uday"},
  {"D6 AA 18 4E", "Avinash"},
  {"B6 F2 76 5F", "Manoj"},
};
const int userCount = sizeof(users) / sizeof(users[0]);
void setup() {
  Serial.begin(115200);

  // RTC
  Wire.begin();

  Serial.begin(115200);
  lcd.begin(16, 2);
  lcd.print("Waiting time...");

  // LCD setup
  lcd.begin(16, 2);
  lcd.print("Initializing...");

  if (!rtc.begin()) {
    lcd.clear();
    lcd.print("RTC Failed");
    while (1);
  }

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));


  // LEDs & buzzer
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  // RFID
  SPI.begin();
  rfid.PCD_Init();

  delay(1000);
  lcd.clear();
  lcd.print("Ready");
}

void loop() {
  DateTime now = rtc.now();
  if (Serial.available()) {
    timeStr = Serial.readStringUntil('\n');
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(timeStr);
  

  lcd.setCursor(0, 1);
  lcd.print("Scan your card");
  }
  else{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Scan your card");
  }
  // Look for new RFID cards
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    delay(1000);
    return;
  }

  // Read UID
  String content = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    content += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
    content += String(rfid.uid.uidByte[i], HEX);
    if (i < rfid.uid.size - 1) content += " ";
  }
  content.toUpperCase();

  lcd.clear();
  lcd.setCursor(0, 0);
  bool found = false;
  String userName = "Unknown";

  for (int i = 0; i < userCount; i++) {
    if (content == users[i].uid) {
      userName = users[i].name;
      found = true;
      break;
    }
  }
  String dateStr = String(now.day()) + "/" + String(now.month()) + "/" + String(now.year());
  String timeStr = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());

  if (found) {
    digitalWrite(greenLed, HIGH);
    tone(buzzerPin, 1500, 700);
    lcd.clear();
    lcd.print("Welcome " + userName);
    delay(1000);
    digitalWrite(greenLed, LOW);
  } else {
    digitalWrite(redLed, HIGH);
    tone(buzzerPin, 1500, 1000);
    lcd.clear();
    lcd.print("Access Denied");
    delay(1000);
    digitalWrite(redLed, LOW);
  }

  // Serial logging
  Serial.print("UID: ");
  Serial.print(content);
  Serial.print(", Name: ");
  Serial.print(userName+"\n");
  // Serial.print(", Date: ");
  // Serial.print(dateStr);
  // Serial.print(", Time: ");
  // Serial.println(timeStr);


  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  delay(1500);
  lcd.clear();
}
void print2digits(int num) {
  if (num < 10) lcd.print("0");
  lcd.print(num);
}
