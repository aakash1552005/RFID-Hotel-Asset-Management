# 🏨 Ultra Low-Cost RFID Anti-Theft & Asset Management System for Hotels

![ESP32](https://img.shields.io/badge/ESP32-IoT-blue?style=for-the-badge&logo=espressif)
![RFID](https://img.shields.io/badge/RFID-RC522-green?style=for-the-badge)
![Google Sheets](https://img.shields.io/badge/Google%20Sheets-Cloud-34A853?style=for-the-badge&logo=googlesheets)
![Arduino](https://img.shields.io/badge/Arduino-IDE-00979D?style=for-the-badge&logo=arduino)
![IEEE](https://img.shields.io/badge/IEEE-Publication-00629B?style=for-the-badge)

> **Minor Project II** — Design & Development of an Ultra Low-Cost Active Miniature RFID Sticker-Type Self-Powered Anti-Theft and Asset Management Device for Hotel Items

---

## 📌 Project Overview

This project presents a **IoT-based RFID system** that protects hotel assets (kettles, TV remotes, etc.) from theft and enables real-time asset management through Google Sheets cloud integration.

When a hotel item is removed without authorisation:
- 🔴 **Buzzer alarm** triggers immediately
- 📊 **Google Sheets** logs the event with product name, ID, room number, date, time and status
- ✅ When item is returned — **"Recovered"** status is logged automatically

---

## 🎯 Key Features

| Feature | Description |
|---|---|
| **Passive RF Energy Harvesting** | Tags powered by RC522 reader field — zero battery per tag |
| **Real-time Cloud Logging** | Every scan logged to Google Sheets via WiFi instantly |
| **Bidirectional Status Tracking** | Both "Missing" and "Recovered" events logged with timestamps |
| **NTP Time Sync** | Accurate IST timestamps on every log entry |
| **Zero Middleware Cost** | Google Apps Script replaces expensive enterprise RFID middleware |
| **Edge Security** | UID resolved locally on ESP32 — only human-readable data sent to cloud |
| **Sub-₹500 Total Cost** | Entire system hardware under ₹500 |
| **LCD Status Display** | Live status shown on 16x2 LCD |
| **Audible Alerts** | Active buzzer for instant theft notification |

---

## 🔬 IEEE Research Contribution

This project addresses gaps identified in existing IEEE RFID literature:

| # | Existing Papers Lack | Our System Provides |
|---|---|---|
| 1 | Per-tag battery dies → asset unprotected | Passive tags — zero battery maintenance |
| 2 | Only theft detection, no recovery log | Bidirectional Missing + Recovered tracking |
| 3 | Expensive proprietary middleware required | Free Google Sheets + Apps Script |
| 4 | Raw UIDs transmitted to cloud | UID resolved locally — privacy preserved |
| 5 | System cost ₹2000+ | Sub-₹500 full system |
| 6 | Cloud latency never measured | End-to-end latency < 3 seconds measured |

---

## 🛠️ Hardware Requirements

| Component | Specification | Qty |
|---|---|---|
| ESP32 Dev Board | Dual-core 240MHz, WiFi 802.11 b/g/n, BT 4.2 | 1 |
| RFID RC522 Module | 13.56 MHz, SPI interface, 3.3V | 1 |
| Blue RFID Keyfob | MIFARE Classic 1K, ISO 14443-A | 1 |
| White RFID Card | MIFARE Classic 1K, credit-card size | 1 |
| Active Buzzer | 5V, 85dB | 1 |
| LCD 16x2 + I2C Module | HD44780, I2C backpack | 1 |
| 3000mAh Li-ion Battery | 3.7V rechargeable with holder | 1 |
| Breadboard | 830 tie-point | 1 |
| Jumper Wires | Male-female, 20cm | 20+ |

---

## 📐 Circuit Wiring

### RC522 → ESP32
| RC522 Pin | ESP32 Pin |
|---|---|
| 3.3V | 3V3 |
| GND | GND |
| SDA | G15 |
| SCK | G18 |
| MOSI | G23 |
| MISO | G19 |
| RST | G4 |

### Other Components → ESP32
| Component | ESP32 Pin |
|---|---|
| Buzzer (+) | G33 |
| Buzzer (−) | GND |
| LCD VCC | 3V3 |
| LCD GND | GND |
| LCD SDA | G21 |
| LCD SCL | G22 |

---

## 💻 Software Requirements

| Software | Purpose |
|---|---|
| Arduino IDE 2.x | Code editor and ESP32 flasher |
| ESP32 Board Package (Espressif) | ESP32 hardware support |
| MFRC522 Library (GithubCommunity) | RC522 RFID reader driver |
| LiquidCrystal I2C (Frank de Brabander) | LCD display driver |
| Google Sheets (free) | Cloud asset log database |
| Google Apps Script (free) | Web API endpoint |

---

## 🚀 Setup Instructions

### Step 1 — Install Arduino IDE & Libraries
```
1. Download Arduino IDE 2.x from arduino.cc
2. Add ESP32 board URL in Preferences:
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
3. Tools → Boards Manager → search "esp32" → install
4. Tools → Manage Libraries → install MFRC522
5. Tools → Manage Libraries → install LiquidCrystal I2C
```

### Step 2 — Find Your Tag UIDs
Upload `uid_scanner.ino` from the `/scanner` folder.
Open Serial Monitor at 115200 baud and scan each tag.
Note down both UIDs.

### Step 3 — Google Sheets Setup
```
1. Create new Google Sheet
2. Add headers: DATE | TIME | PRODUCT_ID | PRODUCT_NAME | ROOM_NO | STATUS
3. Extensions → Apps Script
4. Paste code from /google_apps_script/Code.gs
5. Run testPost() to authorize permissions
6. Deploy → New Deployment → Web App → Anyone → Deploy
7. Copy the Web App URL
```

### Step 4 — Configure Main Code
Open `RFID_Hotel_System.ino` and update:
```cpp
const char* ssid     = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
const char* scriptURL = "YOUR_APPS_SCRIPT_URL";

Asset assets[] = {
  {"YOUR_TAG1_UID", "Electric Kettle", "P001", "Room 204", false},
  {"YOUR_TAG2_UID", "TV Remote",       "P002", "Room 204", false}
};
```

### Step 5 — Upload & Test
```
1. Select Board: ESP32 Dev Module
2. Select Port: your COM port
3. Click Upload — hold BOOT button when "Connecting..." appears
4. Open Serial Monitor at 115200 baud
5. Press EN/RST button
6. Scan tags and watch Google Sheet update live!
```

---

## 📊 Google Sheet Output

| DATE | TIME | PRODUCT_ID | PRODUCT_NAME | ROOM_NO | STATUS |
|---|---|---|---|---|---|
| 22/04/2026 | 14:32:05 | P001 | Electric Kettle | Room 204 | 🔴 Missing |
| 22/04/2026 | 15:10:18 | P001 | Electric Kettle | Room 204 | 🟢 Recovered |
| 22/04/2026 | 15:45:22 | P002 | TV Remote | Room 204 | 🔴 Missing |

---

## 🔄 System Flow

```
RFID Tag Scanned
      ↓
ESP32 reads UID
      ↓
UID → Product Name, ID, Room (local lookup)
      ↓
Get IST Date & Time from NTP server
      ↓
Determine Status (Missing / Recovered)
      ↓
Trigger Buzzer + Show on LCD
      ↓
HTTP POST → Google Apps Script
      ↓
Google Sheet row appended with colour coding
```

---

## 📁 Repository Structure

```
RFID-Hotel-Asset-Management/
│
├── RFID_Hotel_System/
│   └── RFID_Hotel_System.ino    ← Main production code
│
├── scanner/
│   └── uid_scanner.ino          ← Find your tag UIDs
│
├── google_apps_script/
│   └── Code.gs                  ← Paste this in Apps Script
│
├── docs/
│   ├── circuit_diagram.png      ← Wiring diagram
│   └── ieee_paper.pdf           ← Published IEEE paper
│
└── README.md
```

---

## 📈 Measured Performance Results

| Metric | Result |
|---|---|
| Tag detection latency | < 200 ms |
| Cloud log latency | < 3 seconds |
| RFID read range | 3–5 cm |
| False alarm rate | 0% |
| System battery life | > 8 hours |
| Total hardware cost | < ₹500 |

---

## 🏗️ Architecture

```
┌─────────────┐     ┌──────────────┐     ┌─────────────────────┐
│  RFID Tags  │────▶│    ESP32     │────▶│   Google Sheets     │
│ (Passive)   │     │  (WiFi SoC)  │     │  (Cloud Database)   │
└─────────────┘     └──────────────┘     └─────────────────────┘
                           │
                    ┌──────┴──────┐
                    │             │
               ┌────▼────┐  ┌────▼────┐
               │  Buzzer │  │   LCD   │
               │ (Alarm) │  │(Display)│
               └─────────┘  └─────────┘
```

---

## 👨‍💻 Author

**AAKASH S S**
- 🎓 Computer Science and Engineering
- 🏫 Veltech University, Chennai, Tamil Nadu
- 📧 aakash1552005@gmail.com
- 🔗 [LinkedIn](https://www.linkedin.com/in/aakash-s-s)

---

## 🙏 Acknowledgements

- Espressif Systems — ESP32 platform
- GithubCommunity — MFRC522 library
- Google — Sheets and Apps Script platform
- IEEE — Research publication platform

---

⭐ **If this project helped you, please give it a star!**
