#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include "Reader_Specs.h"
#include "BLE_CDevice.h"
#include "HashFunctions.h"

#define SS_PIN 7                    //Define SDA and RST pins for MFRC522 connection
#define RST_PIN 6

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

LiquidCrystal_I2C lcd(0x27,16,2);   // Create LCD instance.

void setup() {
  Serial.begin(9600);

  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522

  lcd.init();       //Initiate LCD
  lcd.clear();         
  lcd.backlight();      // Make sure backlight is on
  lcd.setCursor(4, 0);
  lcd.print(reader_name);
  delay(5000);
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("EZ Access");
  lcd.setCursor(4,1);
  lcd.print("And Logs");
}

void loop() {
  UIDAccess(mfrc522, lcd);
}


void UIDAccess(MFRC522 mfrc522, LiquidCrystal_I2C lcd)
{
// Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Display UID in monitor and save to string
  Serial.print("Presented UID = ");
  String content= "";
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  content.toUpperCase();
  String UIDHash = toHash(content.substring(1));
  if(BLELookForService(UIDHash)) {    //Check if presented UID (content.substring(1)) is valid
    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print("Access");
    lcd.setCursor(4,1);
    lcd.print("Granted");
    delay(5000);
    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print("EZ Access");
    lcd.setCursor(4,1);
    lcd.print("And Logs");
  }
  else {
    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print("Access");
    lcd.setCursor(4,1);
    lcd.print("Denied");
    delay(5000);
    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print("EZ Access");
    lcd.setCursor(4,1);
    lcd.print("And Logs");
  }
  
}

