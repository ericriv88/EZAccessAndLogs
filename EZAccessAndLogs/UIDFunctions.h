////////////////////////////////////////////////////////////////////////////
//***********************UID Functions***********************************///
////////////////////////////////////////////////////////////////////////////

#include "SDFunctions.h"
#include "RTCFunctions.h"

void UIDAccess(MFRC522 mfrc522, LiquidCrystal_I2C lcd, RTCZero rtc)
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
  //Check if presented UID is valid
  if (checkSDForString("UID.txt", content.substring(1))) //if UID is valid
  {
    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print("Access");
    lcd.setCursor(4,1); 
    lcd.print("Granted");

    writeSDLine("LOGS.txt", (DateandTime(rtc) + " -- " + readSDLine("NAME.txt", findSDStringLine("UID.txt", content.substring(1))) + " -- Access Granted"));

    delay(5000);
    lcd.clear();
    lcd.setCursor(4,0); 
    lcd.print("EZ Access");
    lcd.setCursor(4,1);
    lcd.print("And Logs");
  }
 
 else   {
    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print("Access");
    lcd.setCursor(4,1);
    lcd.print("Denied");
    
    writeSDLine("LOGS.txt", (DateandTime(rtc) + " -- Unknown User -- Access Denied"));

    delay(5000);
    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print("EZ Access");
    lcd.setCursor(4,1);
    lcd.print("And Logs");
  }
}

void newCardRegister(MFRC522 mfrc522, LiquidCrystal_I2C lcd, bool* CardRegister) {
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("Present");
  lcd.setCursor(4,1); 
  lcd.print("New Card");

  while (!mfrc522.PICC_IsNewCardPresent()); //wait for new card to be presented
  while (!mfrc522.PICC_ReadCardSerial()) {
    if (!mfrc522.PICC_IsNewCardPresent()) { //fail case triggered
      lcd.clear();
      lcd.setCursor(4,0);
      lcd.print("Register");
      lcd.setCursor(4,1); 
      lcd.print("Failed");
      delay(4000);
      lcd.clear();
      lcd.setCursor(4,0);
      lcd.print("EZ Access");
      lcd.setCursor(4,1);
      lcd.print("And Logs");
      return;
    }
  }

  String content= "";                //get UID from sensor and place in string
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();

  if(!checkSDForString("UID.txt", content.substring(1))) {  //write UID to SD if not already there
    writeSDLine("UID.txt", content.substring(1));
    *CardRegister = true;    //ensures proper page is shown on web
    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print("New Card");
    lcd.setCursor(4,1); 
    lcd.print("Registered");
    delay(4000);
    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print("EZ Access");
    lcd.setCursor(4,1);
    lcd.print("And Logs");
  }
  else {                    //indicate registered failed on LCD
    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print("Already");
    lcd.setCursor(4,1); 
    lcd.print("Registered");
    delay(4000);
    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print("EZ Access");
    lcd.setCursor(4,1);
    lcd.print("And Logs");
  }

}