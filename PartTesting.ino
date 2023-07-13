#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h"
#include "HTML.h"

char def_ssid[] = secret_ssid;         //network SSID
char def_pass[] = secret_pass;       //network password
int keyIndex = 0;                   //network key Index number (needed only for WEP)
int status = WL_IDLE_STATUS;        //connection status
WiFiServer server(80);              //server socket
IPAddress ip;                       //global variable for ip address

WiFiClient client = server.available();
 
#define SS_PIN 7  //Define SDA and RST pins for MFRC522 connection
#define RST_PIN 6
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
LiquidCrystal_I2C lcd(0x27,16,2);   // Create LCD instance.
String ValidUID = blue_uid;         //Define Valid UID string from arduino_secrets.h
bool IPConnect = false;             //indicates when authorized client connected to server
bool IPSetup = false;               //indicates if system has been setup by authorized client
 
void setup() 
{
  Serial.begin(9600);   // Initiate a serial communication

  lcd.init();       //Initiate LCD
  lcd.clear();         
  lcd.backlight();      // Make sure backlight is on
  lcd.setCursor(6,0);   //Set cursor to character 4 on line 0
  lcd.print("WiFi");
  lcd.setCursor(5,1);
  lcd.print("Set Up");

  enable_WiFi();      //Initiate WiFi connection and server setup
  connect_WiFi();
  server.begin();
  printWifiStatus();

  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522

  lcd.clear();          //show IP address on LCD
  lcd.setCursor(0,0);
  lcd.print(ip);
}

void loop() 
{
  client = server.available();
  printWEB();       //Print HTML to client
  if (IPSetup) {    //Activate system if setup on internet has been completed 
    UIDAccess();
  }
}

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
  byte letter;
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
  if (content.substring(1) == ValidUID) //if UID is valid
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
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(def_ssid, def_pass);

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
              if (postBody == key) {    //activate system if POST contents matches the key defined in secrets header
                IPConnect = true;
                IPSetup = true;
                lcd.clear();
                lcd.setCursor(4,0); 
                lcd.print("EZ Access");
                lcd.setCursor(4,1);
                lcd.print("And Logs");
              }
            }

            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
           
            //create the buttons
            if (IPConnect) {  //if the system is activated
              client.print(HTML_active);  //send HTML code to choose wich tag is valid
            }

            else {      //if system is not activated
              client.print(HTML_inactiveA);   //send longin HTML code
              client.print(ip);               //which requires ip address
              client.print(HTML_inactiveB);
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

        if (currentLine.endsWith("GET /H") && IPConnect) {
          ValidUID = blue_uid; //Define Valid UID string from arduino_secrets.h       
        }
        if (currentLine.endsWith("GET /L") && IPConnect) {
          ValidUID = white_uid; //Define Valid UID string from arduino_secrets.h      
        }
        if (currentLine.endsWith("GET /S") && IPConnect) {
          IPConnect = false;    //logout so that no changes can be made to access list
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
    Serial.println();
  }
}
