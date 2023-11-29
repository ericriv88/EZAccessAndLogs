////////////////////////////////////////////////////////////////////////////
//**********************UID/KEYPAD Functions*****************************///
////////////////////////////////////////////////////////////////////////////

#include "SDFunctions.h"
#include "RTCFunctions.h"
#include "Keypad.h"

const byte ROWS = 4; // Number of rows on the keypad
const byte COLS = 4; // Number of columns on the keypad

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {16, 17, 18, 19}; // Connect to the row pinouts of the keypad
byte colPins[COLS] = {2, 3, 4, 5}; // Connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

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


    static char inputSequence[5]; // Array to store the entered characters 
    static int count = 0; // Counter to keep track of entered characters

    lcd.clear();
    lcd.setCursor(3,0);
    lcd.print("Enter Pin:");

    while (count < 5) {
        char key = keypad.getKey(); // Get keypad input

        if (key != NO_KEY) {
            inputSequence[count] = key;
            count++;
            Serial.print("Pressed: ");
            Serial.println(key);
            
            lcd.setCursor(count - 1,1); // Update LCD with entered PIN
            lcd.print(key); // Display entered digit on LCD
        }
    }

  //Check if presented UID and PIN are valid
  if (checkSDForString("UID.txt", toHash(content.substring(1))) && checkSDForString("KEYPAD.txt", toHash(inputSequence))) //if UID & KEYPAD are valid
  {
    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print("Access");
    lcd.setCursor(4,1); 
    lcd.print("Granted");
    //Log the access
    writeSDLine("LOGS.txt", (DateandTime(rtc) + " -- " + readSDLine("NAME.txt", findSDStringLine("UID.txt", toHash(content.substring(1)))) + " -- Hub Reader -- Access Granted"));
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
    //Log the access
    if(checkSDForString("UID.txt", toHash(content.substring(1))))
      writeSDLine("LOGS.txt", (DateandTime(rtc) + " -- " + readSDLine("NAME.txt", findSDStringLine("UID.txt", toHash(content.substring(1)))) + " -- Hub Reader -- Access Denied"));
    else
      writeSDLine("LOGS.txt", (DateandTime(rtc) + " -- Unknown User -- Hub Reader -- Access Denied"));
    delay(5000);
    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print("EZ Access");
    lcd.setCursor(4,1);
    lcd.print("And Logs");
  }
    count = 0;
    memset(inputSequence, 0, sizeof(inputSequence)); // Clear the array
}

String newCardRead(MFRC522 mfrc522, LiquidCrystal_I2C lcd, bool* CardRegister) {
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
      return "0";
    }
  }

  String content= "";                //get UID from sensor and place in string
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  if(checkSDForString("UID.txt", toHash(content.substring(1)))) {
    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print("Already");
    lcd.setCursor(4,1); 
    lcd.print("Registered");
    delay(3000);
    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print("EZ Access");
    lcd.setCursor(4,1);
    lcd.print("And Logs");
    return "0";
  }
  writeSDHashLine("UID.txt", content.substring(1));
  *CardRegister = true;    //ensures proper page is shown on web
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("EZ Access");
  lcd.setCursor(4,1);
  lcd.print("And Logs");
  return content.substring(1);
}

void newCardRegister(String UID, String KEYPAD, LiquidCrystal_I2C lcd, int readerNumber) 
{
  String UIDFile;           //file name to add UID hash to
  UIDFile = "READERS/UID" + String(readerNumber) + ".txt";

  if(!checkSDForString(UIDFile, toHash(UID))) {  //write UID to SD if not already there
    writeSDHashLine(UIDFile, UID);
  }
  String KEYPADFile;           //file name to add KEYPAD hash to
  KEYPADFile = "READERS/KEYPAD" + String(readerNumber) + ".txt";

  if(!checkSDForString(KEYPADFile, toHash(KEYPAD))) {  //write PIN to SD if not already there
    writeSDHashLine(KEYPADFile, KEYPAD);
  }
}