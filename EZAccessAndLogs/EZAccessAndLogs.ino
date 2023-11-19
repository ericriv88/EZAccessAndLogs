#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>
#include <SD.h>
#include <RTCZero.h>
#include "UIDFunctions.h"
#include "arduino_secrets.h"
#include "HTML.h"
#include "BLE_Peripheral.h"
#include "WiFiFunctions.h"


IPAddress ip;                       //global variable for ip address
WiFiServer server(80);              //server socket
WiFiClient client = server.available();


RTCZero rtc;                        //Create RTC instance
 
#define SS_PIN 7                    //Define SDA and RST pins for MFRC522 connection
#define RST_PIN 6
const int chipSelect = 1;           //Define chipselect pin for SD card module
const byte interruptPin = 0;        //Define interrupt pin for button press
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
LiquidCrystal_I2C lcd(0x27,16,2);   // Create LCD instance.


bool IPSetup = false;               //indicates if system has been setup by authorized client
bool Enterprise = false;            //indicates if using enterprise network

bool wifiSetup = false;
bool wifiOn = false;

bool BLESetup = false;
bool BLEOn = false;
 
void setup() 
{
  Serial.begin(9600);   // Initiate a serial communication
  delay(1000);           //delay for serial coms

  lcd.init();       //Initiate LCD
  lcd.clear();         
  lcd.backlight();      // Make sure backlight is on

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

  rtc.begin();            //set up RTC
  unsigned long epoch;    //get time and date from web
  epoch = WiFi.getTime();
  rtc.setEpoch(epoch);    //set RTC instance to proper start time and date

  enable_WiFi(lcd);       //reset wifi status and end connection
  WiFi.end();

  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("EZ Access");
  lcd.setCursor(4,1);
  lcd.print("And Logs");

  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), button, FALLING); //on button press show ip on LCD

  if(IPSetup)
    BLESetup = true;

}

void loop() 
{
  if(wifiSetup) 
    wifiStart();
  if(wifiOn) {
    client = server.available();
    printWEB(client, &IPSetup, lcd, ip, mfrc522, &BLESetup);       //Print HTML to client
  }
  if(BLESetup) {
    BLEStart();
  }
  if(BLEOn)
    BLE.poll();
  if (IPSetup) {    //Activate system if setup on internet has been completed 
    UIDAccess(mfrc522, lcd, rtc);
  }
}



////////////////////////////////////////////////////////////////////////////
//********************Button ISR Function********************************///
////////////////////////////////////////////////////////////////////////////
void button() {
  wifiSetup = true;
}

void wifiStart() {
  if(wifiOn) {  //dont do anything other than display ip if wifi is already on
    lcd.clear();          //show IP address on LCD
    lcd.setCursor(0,0);
    lcd.print(ip);
    return;
  }
  if(BLEOn) {
    BLE.end();
    BLEOn = false;
  }
  lcd.clear();
  lcd.setCursor(6,0);   //Set cursor to character 6 on line 0
  lcd.print("WiFi");
  lcd.setCursor(5,1);
  lcd.print("Set Up");

  enable_WiFi(lcd);      //Initiate WiFi connection and server setup
  connect_WiFi(Enterprise, lcd);
  server.begin();
  printWifiStatus(&ip);
  lcd.clear();          //show IP address on LCD
  lcd.setCursor(0,0);
  lcd.print(ip);

  wifiOn = true;
  wifiSetup = false;
}

void BLEStart() {
  if(wifiOn) {
    WiFi.end();
    wifiOn = false;
  }

  BLE_Peripheral_init();
  BLE_Peripheral_Reset_Services();

  BLEOn = true;
  BLESetup = false;

  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("EZ Access");
  lcd.setCursor(4,1);
  lcd.print("And Logs");
}

