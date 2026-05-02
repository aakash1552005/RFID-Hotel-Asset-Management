#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include <time.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// WiFi
const char* ssid     = "aakash";
const char* password = "12345678";

// Google Script URL
const char* scriptURL = "https://script.google.com/macros/s/AKfycbxddxiQTB_1B7YWmOdIH8d6O3-ChoUDU5Aru8njhtvlsLmghvMDs_nz_dCqYZ4ZpsD-Vg/exec";

// RC522
#define SS_PIN  15
#define RST_PIN 4
MFRC522 rfid(SS_PIN, RST_PIN);

// Buzzer
#define BUZZER 33

// LCD - try 0x27 first, if blank change to 0x3F
LiquidCrystal_I2C lcd(0x27, 16, 2);

// IST offset
const long utcOffset = 19800;

// Assets
struct Asset {
  String uid;
  String name;
  String id;
  String room;
  bool   isPresent;
};

Asset assets[] = {
  {"BF 71 70 1F", "Electric Kettle", "P001", "Room 204", false},
  {"F4 9B 7A 00", "TV Remote",       "P002", "Room 204", false}
};
const int ASSET_COUNT = 2;

String getDate() {
  struct tm t;
  if (!getLocalTime(&t)) return "--/--/----";
  char buf[12];
  strftime(buf, sizeof(buf), "%d/%m/%Y", &t);
  return String(buf);
}

String getTimeNow() {
  struct tm t;
  if (!getLocalTime(&t)) return "--:--:--";
  char buf[10];
  strftime(buf, sizeof(buf), "%H:%M:%S", &t);
  return String(buf);
}

String getUID() {
  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) uid += "0";
    uid += String(rfid.uid.uidByte[i], HEX);
    if (i < rfid.uid.size - 1) uid += " ";
  }
  uid.toUpperCase();
  return uid;
}

int findAsset(String uid) {
  for (int i = 0; i < ASSET_COUNT; i++)
    if (assets[i].uid == uid) return i;
  return -1;
}

void sendToSheets(String name, String id, String room,
                  String date, String t, String status) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Error!");
    return;
  }
  HTTPClient http;
  http.begin(scriptURL);
  http.addHeader("Content-Type", "application/json");
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

  String payload = "{";
  payload += "\"date\":\""         + date   + "\",";
  payload += "\"time\":\""         + t      + "\",";
  payload += "\"product_id\":\""   + id     + "\",";
  payload += "\"product_name\":\"" + name   + "\",";
  payload += "\"room_no\":\""      + room   + "\",";
  payload += "\"status\":\""       + status + "\"";
  payload += "}";

  int code = http.POST(payload);
  Serial.println("Sheet response: " + String(code));
  http.end();
}

void alarm() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(BUZZER, HIGH); delay(300);
    digitalWrite(BUZZER, LOW);  delay(200);
  }
}

void successBeep() {
  digitalWrite(BUZZER, HIGH); delay(100);
  digitalWrite(BUZZER, LOW);  delay(100);
  digitalWrite(BUZZER, HIGH); delay(100);
  digitalWrite(BUZZER, LOW);
}

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER, OUTPUT);

  // LCD init
  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("RFID Hotel Sys");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");
  delay(1500);

  // RC522 init
  SPI.begin(18, 19, 23, 15);
  rfid.PCD_Init();

  // Startup beep
  digitalWrite(BUZZER, HIGH);
  delay(200);
  digitalWrite(BUZZER, LOW);

  // Connect WiFi
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi");
  Serial.print("Connecting WiFi");
  WiFi.begin(ssid, password);
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 30) {
    delay(500);
    Serial.print(".");
    tries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Connected!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Connected!");
    successBeep();
    delay(1000);
  } else {
    Serial.println("\nWiFi Failed!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Failed!");
    alarm();
    delay(1000);
  }

  // Time sync
  configTime(utcOffset, 0, "pool.ntp.org");
  delay(5000);
  Serial.println("Time: " + getDate() + " " + getTimeNow());

  // Ready
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scan your tag");
  lcd.setCursor(0, 1);
  lcd.print("System Ready!");
  Serial.println("System Ready!");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() ||
      !rfid.PICC_ReadCardSerial()) {
    delay(100);
    return;
  }

  String uid = getUID();
  Serial.println("Scanned: " + uid);

  int idx = findAsset(uid);

  if (idx == -1) {
    Serial.println("UNKNOWN TAG!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("UNKNOWN TAG!");
    lcd.setCursor(0, 1);
    lcd.print("ALERT!!!");
    alarm();
    sendToSheets("UNKNOWN", "N/A", "N/A",
      getDate(), getTimeNow(), "ALERT-Unknown");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Scan your tag");
    lcd.setCursor(0, 1);
    lcd.print("System Ready!");
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    return;
  }

  String name   = assets[idx].name;
  String id     = assets[idx].id;
  String room   = assets[idx].room;
  String date   = getDate();
  String t      = getTimeNow();
  String status;

  if (!assets[idx].isPresent) {
    assets[idx].isPresent = true;
    status = "Recovered";
    successBeep();
    Serial.println("RECOVERED: " + name);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(name.substring(0, 16));
    lcd.setCursor(0, 1);
    lcd.print("Recovered OK!");
  } else {
    assets[idx].isPresent = false;
    status = "Missing";
    alarm();
    Serial.println("MISSING: " + name);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(name.substring(0, 16));
    lcd.setCursor(0, 1);
    lcd.print("MISSING! ALERT!");
  }

  Serial.println(name + " | " + id + " | " +
    room + " | " + date + " | " + t + " | " + status);

  sendToSheets(name, id, room, date, t, status);

  delay(3000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scan your tag");
  lcd.setCursor(0, 1);
  lcd.print("System Ready!");

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  delay(1000);
}