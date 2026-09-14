
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial bt(10, 11); // RX, TX

// -------- PIN CONFIG --------
const int coinPin = 2;
const int relayPin = 7;
const int ledPin = 13;

// -------- COIN VARIABLES --------
volatile int pulseCount = 0;
unsigned long lastPulseTime = 0;
const unsigned long pulseTimeout = 800;

// -------- RELAY & TIME --------
float totalCredit = 0.0;
unsigned long relayEndTime = 0;
bool relayActive = false;
unsigned long lastSecondPrint = 0;

const unsigned long timePerRupee = 5000; // 5 sec per ₹1
// ---------- SETUP ----------
void setup() {
    Serial.begin(9600);
    bt.begin(9600);

    pinMode(coinPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(coinPin), coinISR, FALLING);

    pinMode(relayPin, OUTPUT);
    pinMode(ledPin, OUTPUT);

    // Relay OFF at startup (ACTIVE-LOW)
    digitalWrite(relayPin, HIGH);
    digitalWrite(ledPin, LOW);

    lcd.init();
    lcd.backlight();

    showIdle();

    Serial.println("=================================");
    Serial.println("Coin + Bluetooth System READY");
    Serial.println("BT Commands: A=1 B=2 C=5 D=10");
    Serial.println("=================================");
}
// ---------- LOOP ----------
void loop() {
    unsigned long now = millis();

    // -------- BLUETOOTH --------
    if (bt.available()) {
        char cmd = bt.read();
        cmd = toupper(cmd);
        handleBluetooth(cmd);
    }

    // -------- COIN DETECTION --------
    if (pulseCount > 0 && (now - lastPulseTime) > pulseTimeout) {
        int pulses = pulseCount;

        if (pulses == 1 || pulses == 2 || pulses == 5 || pulses == 10) {
            Serial.print("Coin Inserted: Rs");
            Serial.println(pulses);
            addCredit(pulses);
        } 
        else {
            Serial.println("Dummy Coin Rejected");

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(" DUMMY COIN! ");
            lcd.setCursor(0, 1);
            lcd.print(" REJECTED ");

            delay(2000);
            showIdle();
        }

        pulseCount = 0;
    }

    // -------- RELAY TIMEOUT --------
    if (relayActive && now >= relayEndTime) {
        digitalWrite(relayPin, HIGH);
        digitalWrite(ledPin, LOW);

        relayActive = false;
        totalCredit = 0;

        Serial.println("Relay OFF (Time Up)");

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(" Time's Up! ");
        lcd.setCursor(0, 1);
        lcd.print(" Insert Coin ");

        delay(2000);
        showIdle();
    }

    // -------- SERIAL COUNTDOWN --------
    if (relayActive && millis() - lastSecondPrint >= 1000) {
        lastSecondPrint = millis();

        unsigned long remainSec =
            (relayEndTime - millis()) / 1000;

        Serial.print("Remaining Time: ");
        Serial.print(remainSec);
        Serial.println(" seconds");
    }

    // -------- LCD COUNTDOWN --------
    if (relayActive) {
        updateCountdown(now);
    }
}
// ---------- BLUETOOTH HANDLER ----------
void handleBluetooth(char cmd) {

    if (cmd == 'A') {
        Serial.println("BT: Command A (Rs1)");
        addCredit(1);
    }
    else if (cmd == 'B') {
        Serial.println("BT: Command B (Rs2)");
        addCredit(2);
    }
    else if (cmd == 'C') {
        Serial.println("BT: Command C (Rs5)");
        addCredit(5);
    }
    else if (cmd == 'D') {
        Serial.println("BT: Command D (Rs10)");
        addCredit(10);
    }
    else if (cmd == 'E') {

        // RESET
        relayActive = false;
        totalCredit = 0;

        digitalWrite(relayPin, HIGH);
        digitalWrite(ledPin, LOW);

        lcd.clear();
        showIdle();

        Serial.println("BT: Command E received - System Reset");
    }
    else {
        Serial.print("BT: Invalid Command -> ");
        Serial.println(cmd);
    }
}
// ---------- ADD CREDIT ----------
void addCredit(int amount) {

    totalCredit += amount;

    unsigned long addedTime =
        (unsigned long)amount * timePerRupee;

    if (!relayActive) {
        relayActive = true;
        relayEndTime = millis() + addedTime;

        digitalWrite(relayPin, LOW);
        digitalWrite(ledPin, HIGH);
    }
    else {
        relayEndTime += addedTime;
    }

    Serial.print("Total Credit: Rs");
    Serial.println(totalCredit);

    Serial.print("Added Time: ");
    Serial.print(addedTime / 1000);
    Serial.println(" seconds");
}
// ---------- LCD COUNTDOWN ----------
void updateCountdown(unsigned long now) {

    unsigned long remaining =
        (relayEndTime - now) / 1000;

    lcd.setCursor(0, 0);
    lcd.print("Credit: Rs");
    lcd.print(totalCredit);
    lcd.print(" ");

    lcd.setCursor(0, 1);
    lcd.print("Time: ");
    lcd.print(remaining);
    lcd.print("s ");
}
// ---------- IDLE SCREEN ----------
void showIdle() {
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print(" Insert Coin ");

    lcd.setCursor(0, 1);
    lcd.print(" Welcome! ");
}
// ---------- COIN INTERRUPT ----------
void coinISR() {
    pulseCount++;
    lastPulseTime = millis();
}