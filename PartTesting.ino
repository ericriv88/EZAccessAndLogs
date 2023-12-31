#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiNINA.h>
#include <SD.h>
#include "arduino_secrets.h"
#include "HTML.h"

char def_ssid[] = secret_ssid;         //network SSID
char def_pass[] = secret_pass;       //network password
char def_user[] = secret_user;       //user name for enterprise
int keyIndex = 0;                   //network key Index number (needed only for WEP)
int status = WL_IDLE_STATUS;        //connection status
WiFiServer server(80);              //server socket
IPAddress ip;                       //global variable for ip address

WiFiClient client = server.available();
 
#define SS_PIN 7                    //Define SDA and RST pins for MFRC522 connection
#define RST_PIN 6
const int chipSelect = 1;           //Define chipselect pin for SD card module
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
LiquidCrystal_I2C lcd(0x27,16,2);   // Create LCD instance.
bool IPConnect = false;             //indicates when authorized client connected to server
bool IPSetup = false;               //indicates if system has been setup by authorized client
bool CredChange = false;            //indicates if credential change is occuring
bool CardRegister = false;          //indicates that a card is being registered
bool CardManage = false;            //indicates card management mode
bool Enterprise = true;            //indicates if using enterprise network
 
void setup() 
{
  Serial.begin(9600);   // Initiate a serial communication

  lcd.init();       //Initiate LCD
  lcd.clear();         
  lcd.backlight();      // Make sure backlight is on
  lcd.setCursor(6,0);   //Set cursor to character 6 on line 0
  lcd.print("WiFi");
  lcd.setCursor(5,1);
  lcd.print("Set Up");

  enable_WiFi();      //Initiate WiFi connection and server setup
  connect_WiFi();
  server.begin();
  printWifiStatus();

  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522

  Serial.print("Initializing SD card...");    //Initialize SD card 
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!"); //If failed to initalize, send message to serial and lcd
    lcd.clear();         
    lcd.setCursor(5,0);
    lcd.print("SD");
    lcd.setCursor(4,1);
    lcd.print("Failed");
    while(!SD.begin(chipSelect)) {
      delay(5000);
    }
  }
  Serial.println("initialization done.");

  IPSetup = readSDBool("SET.txt"); //read file on SD card for IPSetup value

  if(IPSetup) {
    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print("EZ Access");
    lcd.setCursor(4,1);
    lcd.print("And Logs");
  }
  else {
    lcd.clear();          //show IP address on LCD
    lcd.setCursor(0,0);
    lcd.print(ip);
  }
}

void loop() 
{
  client = server.available();
  printWEB();       //Print HTML to client
  if (IPSetup) {    //Activate system if setup on internet has been completed 
    UIDAccess();
  }
}

////////////////////////////////////////////////////////////////////////////
//***********************UID Functions***********************************///
////////////////////////////////////////////////////////////////////////////

void UIDAccess()
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
    delay(4000);
    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print("EZ Access");
    lcd.setCursor(4,1);
    lcd.print("And Logs");
  }
}

void newCardRegister() {
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
    CardRegister = true;    //ensures proper page is shown on web
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

////////////////////////////////////////////////////////////////////////////
//**********************WiFi Functions***********************************///
////////////////////////////////////////////////////////////////////////////

void printWifiStatus() {
  //print the SSID of the network attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  //print the board's IP address:
  ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  //print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");

  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

void enable_WiFi() {
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    lcd.clear();      //Notify with LCD of failure
    lcd.setCursor(6,0);
    lcd.print("WiFi");
    lcd.setCursor(4,1); 
    lcd.print("Failed");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }
}

void connect_WiFi() {
  lcd.clear();          //notify connecting by LCD 
  lcd.setCursor(6,0);
  lcd.print("WiFi");
  lcd.setCursor(3,1); 
  lcd.print("Connecting");
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(def_ssid);
    // Connect to network differently depending on Enterprise connection
    if (! Enterprise) status = WiFi.begin(def_ssid, def_pass);
    else status = WiFi.beginEnterprise(def_ssid, def_user, def_pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
}

void printWEB() {

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    bool postData = false;
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {

            //if there is post data then serial print it
            if (postData) {
              String postBody = client.readString();
              if(!CredChange && !CardRegister) {
                if (postBody == readSDLine("LOGIN.txt", 1)) {    //activate system if POST contents matches the credentials in SD card
                  IPConnect = true;
                  IPSetup = true;
                  overWriteSDBool("SET.txt", true); //overwrite file in SD card 
                  lcd.clear();
                  lcd.setCursor(4,0); 
                  lcd.print("EZ Access");
                  lcd.setCursor(4,1);
                  lcd.print("And Logs");
                }
              }
              else if(CredChange){      //indicates a credential change
                overWriteSD("LOGIN.txt", postBody); //overwrite credential in SD
                CredChange = false;
              }
              else {        //indicates card register
                String nickName;
                for(int i = 5; i < postBody.length(); i++) {    //get only the nickname from the postBody
                  nickName += postBody[i];
                }
                //*** NEED TO CHECK FOR REAPEATS ***///
                writeSDLine("NAME.txt", nickName);  //write nickname to SD
                CardRegister = false;
              }
            }

            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
           
            //create the buttons
            if (IPConnect) {  //if the system is activated send either credential change or home page HTML code
              if (CredChange) {
                client.print(HTML_CredChangeA);   //send credential change HTML code
                client.print(ip);               //which requires ip address
                client.print(HTML_CredChangeB);
              }
              else if (CardRegister) {
                client.print(HTML_CardRegisterA);   //send card register HTML code
                client.print(ip);                  //which requires ip address
                client.print(HTML_CardRegisterB);
              }
              else if (CardManage) {
                client.print(HTML_CardManageA);   //send card manage HTML code
                for(int i = 1; i <= SDLineCount("NAME.txt"); i++) {    //which requires all nicknames be printed between as anchors giving option to delete
                client.print("<li><a href=\"/");
                client.print(i);
                client.print("\">Delete ");
                client.print(readSDLine("NAME.txt", i));
                client.print("</a></li>");
                }
                client.print(HTML_CardManageB);
              }
              else {
                client.print(HTML_HomePage);  //send HTML of home page if logged in
              }
            }

            else {      //if system is not activated
              client.print(HTML_LoginA);   //send longin HTML code
              client.print(ip);               //which requires ip address
              client.print(HTML_LoginB);
            }
            
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          }
          else if (currentLine.startsWith("Content-Length:")) { // Check if the request contains POST data
            postData = true; 
          }
          currentLine = "";   //clear current line
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        //Check for all anchor text presses
        if (currentLine.endsWith("GET /Register") && IPConnect) {
          newCardRegister();  //Register new card and make it the ValidUID      
        }
        if (currentLine.endsWith("GET /Manage") && IPConnect) {
          CardManage = true;    //indicates card management request
        }
        if (currentLine.endsWith("GET /Delete") && IPConnect) {
          whipeSDFile("UID.txt");  //Delete all UIDs and nicknames from the SD  
          whipeSDFile("NAME.txt");   
        }
        if (currentLine.endsWith("GET /Reset") && IPConnect) {
          overWriteSDBool("SET.txt", false); //Reset all settings and logout
          overWriteSD("LOGIN.txt", default_login);
          whipeSDFile("UID.txt");
          whipeSDFile("NAME.txt");
          IPSetup = false;
          IPConnect = false;
          lcd.clear();          //show IP address on LCD
          lcd.setCursor(0,0);
          lcd.print(ip);    
        }
        if (currentLine.endsWith("GET /Logout") && IPConnect) {
          IPConnect = false;    //logout so that no changes can be made to access list
        }
        if (currentLine.endsWith("GET /Change") && IPConnect) {
          CredChange = true;    //indicates credential change
        }
        if(IPConnect && CardManage) {
          for(int i = 1; i <= SDLineCount("NAME.txt"); i++) {   //check if any of the cards are trying to be deleted
            String cardDelete = "GET /" + String(i);
            if(currentLine.endsWith(cardDelete)) {
              deleteSDLine("NAME.txt", i);    //delete nickanme and UID from files
              deleteSDLine("UID.txt", i);
            }
          }
        }
        if (currentLine.endsWith("GET /Menu") && IPConnect) {   //need this for going back to main page from card management
          CardManage = false;     
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
    Serial.println();
  }
}

////////////////////////////////////////////////////////////////////////////
//************************SD Functions***********************************///
////////////////////////////////////////////////////////////////////////////

bool readSDBool(String fileName) {
  File myFile;                        //file varriable for SD card use
  myFile = SD.open(fileName);        //opens .txt file specified by fileName
  String temp = "";
  while (myFile.available()) {
    char k = myFile.read();
    if(k == '\r') {
      if(temp == "true") {  //if string is "true" then close file and return true
        myFile.close();
        return true;
      }
    }
    else {
      if(k != '\n')
        temp += k;
    }
  }
  myFile.close();   //close file and return false
  return false; 
}

void overWriteSDBool(String fileName, bool set) {
  File myFile;          //file varriable for SD card use
  SD.remove(fileName);  //delete file fr overwrite
  myFile = SD.open(fileName, FILE_WRITE);
  //write true or false to file depending on set
  if(set) { 
    myFile.println("true");
  }
  else {
    myFile.println("false");
  }
  myFile.close();
}

String readSDLine(String fileName, int line) {
  File myFile;                //file variable declaration
  myFile = SD.open(fileName); //opens .txt file specified by fileName
  String temp = "";
  int lineCount = 1;
  while (myFile.available()) {
    char k = myFile.read();
    if(k == '\r') {
      if(lineCount == line) {   //if on correct line, close file and return temp
        myFile.close();
        return temp;
      }
      temp = "";  //clear temp for next line
      lineCount += 1; //increment lineCount
    }
    else {
      if(k != '\n')
        temp += k;
    }
  }
  myFile.close();   //close file
  return temp;      //return the empty string if defined line is empty
}

void deleteSDLine(String fileName, int line) {
  File myFile;                //file variable declaration
  myFile = SD.open(fileName); //opens .txt file specified by fileName
  String temp = "";
  String SDret;            //initialize string that will be written to SD
  int lineCount = 1;
  while (myFile.available()) {
    char k = myFile.read();
    if(k == '\r') {
      if(lineCount == line) {   //if on line to delete, dont add line to return string
        temp = "";  //clear temp for next line
        lineCount += 1; //increment lineCount
      }
      else {
        SDret += temp;    //add line to return string with carage return
        temp = "";  //clear temp for next line
        if(line == SDLineCount(fileName) && lineCount == SDLineCount(fileName) -1) {    //gets rid of extra line when deleting last line
          //dont add any end of line
        }
        else if(lineCount != SDLineCount(fileName)) {   //add end of line unless on last line 
          SDret += '\r';
          SDret += '\n';
        }
        lineCount += 1; //increment lineCount
      }
      
    }
    else {
      if(k != '\n')
        temp += k;
    }
  }
  myFile.close();   //close file
  overWriteSD(fileName, SDret); //overwrite file with new string that has deleted the line
}

int SDLineCount(String fileName) {
  File myFile;                //file variable declaration
  myFile = SD.open(fileName); //opens .txt file specified by fileName
  int lineCount = 0;          //initialize linecount
  while (myFile.available()) {
    char k = myFile.read();
    if(k == '\r') {
      lineCount += 1; //increment lineCount
    }
  }
  myFile.close();   //close file
  return lineCount;      //retrn line ocunt
}

bool checkSDForString(String fileName, String givenString) {
  File myFile;                //file variable declaration
  myFile = SD.open(fileName); //opens .txt file specified by fileName
  String temp = "";
  while (myFile.available()) {
    char k = myFile.read();
    if(k == '\r') {
      if(temp == givenString) {   //if line on SD equals the given string, close file and return true
        myFile.close();
        return true;
      }
      temp = "";  //clear temp for next line
    }
    else {
      if(k != '\n')
        temp += k;
    }
  }
  myFile.close();   //close file
  return false;      //retuern the empty string if defined line is empty
}

void overWriteSD(String fileName, String content)
{
  File myFile;          //file varriable for SD card use
  SD.remove(fileName);  //delete file for overwrite
  myFile = SD.open(fileName, FILE_WRITE);
  myFile.println(content);
  myFile.close();
}

void writeSDLine(String fileName, String content)
{
  File myFile;          //file varriable for SD card use
  myFile = SD.open(fileName, FILE_WRITE);
  myFile.println(content);
  myFile.close();
}

void whipeSDFile(String fileName) {
  File myFile;          //file varriable for SD card use
  SD.remove(fileName);  //delete file 
  myFile = SD.open(fileName, FILE_WRITE);   //recreate file
  myFile.close();       //close file
}
