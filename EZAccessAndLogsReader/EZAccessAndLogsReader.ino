#include <LiquidCrystal_I2C.h>
#include <ArduinoLowPower.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>
#include "Reader_Specs.h"
#include "BLE_CDevice.h"
#include "HashFunctions.h"

#define SS_PIN 7 // Define SDA and RST pins for MFRC522 connection
#define RST_PIN 6

volatile int length = 4800;
bool cardDetected = false;
const byte interruptPin = 0; // Define interrupt pin for button press
const byte ROWS = 4; // Number of rows on the keypad
const byte COLS = 4; // Number of columns on the keypad
unsigned long startTime;
unsigned long endTime;

char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {16, 17, 18, 19}; // Connect to the row pinouts of the keypad
byte colPins[COLS] = {2, 3, 4, 5};     // Connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.
LiquidCrystal_I2C lcd(0x27, 16, 2); // Create LCD instance.

void setup() {
    Serial.begin(9600);

    SPI.begin();             // Initiate SPI bus
    mfrc522.PCD_Init();      // Initiate MFRC522

    lcd.init();              // Initiate LCD
    lcd.clear();
    lcd.backlight();         // Make sure backlight is on
    lcd.setCursor(4, 0);
    lcd.print(reader_name);
    delay(5000);
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("EZ Access");
    lcd.setCursor(4, 1);
    lcd.print("And Logs");
}

void loop() {
    startTime = millis();

    for (int i = 0; i < length; i++) {
        UIDAccess(mfrc522, lcd);

        // Reset the loop counter if a card is detected
        if (cardDetected) {
            i = -1; // Reset to -1 as the loop increments it to 0 on next iteration
            cardDetected = false; // Reset the flag
        }
    }

    endTime = millis();

    unsigned long loopDuration = endTime - startTime;
    Serial.print("Loop duration (ms): ");
    Serial.println(loopDuration);
    lcd.clear();                  // Clear the display before sleep
    lcd.noBacklight();            // Backlight off
    mfrc522.PCD_SoftPowerDown();  // Power down RC522
    delay(100);
    LowPower.deepSleep();         // Enter deep sleep mode with interrupt pin
}

void UIDAccess(MFRC522 mfrc522, LiquidCrystal_I2C lcd) {
    // Look for new cards
    if (!mfrc522.PICC_IsNewCardPresent()) {
        return;
    }
    // Select one of the cards
    if (!mfrc522.PICC_ReadCardSerial()) {
        return;
    }
    cardDetected = true; 
    // Display UID in monitor and save to string
    Serial.print("Presented UID = ");
    String content = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
        content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
        content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    Serial.println();
    content.toUpperCase();

    // Get pin from keypad
    static char inputSequence[5]; // Array to store the entered characters
    int count = 0;                // Counter to keep track of entered characters

    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("Enter Pin:");

    while (count < 5) {
        char key = keypad.getKey(); // Get keypad input

        if (key != NO_KEY) {
            inputSequence[count] = key;
            count++;

            lcd.setCursor(count - 1, 1); // Update LCD with entered PIN
            lcd.print(key);              // Display entered digit on LCD
        }
    }

    String credential = content.substring(1) + inputSequence;
    String UIDHash = toHash(credential);
    if (BLELookForService(UIDHash)) { // Check if presented UID (content.substring(1)) is valid
        lcd.clear();
        lcd.setCursor(4, 0);
        lcd.print("Access");
        lcd.setCursor(4, 1);
        lcd.print("Granted");
        delay(5000);
        lcd.clear();
        lcd.setCursor(4, 0);
        lcd.print("EZ Access");
        lcd.setCursor(4, 1);
        lcd.print("And Logs");
    } else {
        lcd.clear();
        lcd.setCursor(4, 0);
        lcd.print("Access");
        lcd.setCursor(4, 1);
        lcd.print("Denied");
        delay(5000);
        lcd.clear();
        lcd.setCursor(4, 0);
        lcd.print("EZ Access");
        lcd.setCursor(4, 1);
        lcd.print("And Logs");
    }
}
