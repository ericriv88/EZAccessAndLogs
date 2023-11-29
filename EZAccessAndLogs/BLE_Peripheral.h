////////////////////////////////////////////////////////////////////////////
//***********************BLE Functions***********************************///
////////////////////////////////////////////////////////////////////////////


#include "BLEService.h"
#include <ArduinoBLE.h>

#define default_uuid "09B10000-E8F2-537E-4F6C-D104768A1000"

String UIDHash = "";
bool validUID = false;

void BLE_Peripheral_init() {      //initialize BLE_Peripheral 
 Serial.print("Starting BLE peripheral... ");
  if (!BLE.begin()) {
    Serial.println("failed!");
    while (1);
  }
  Serial.println("success!");

  BLE.setLocalName("EZALServer");  // Set the local name advertised by the server

  BLE.advertise(); // Start advertising
  Serial.println("Now advertising");
}

void BLE_Peripheral_Add_Reader() {  //adds new reader by making new txt file for valid UID storage
  String RCount = readSDLine("RCOUNT.txt", 1);
  int ReaderCount = RCount.toInt();
  ReaderCount++;
  overWriteSD("RCOUNT.txt", String(ReaderCount));
  String fileName = "READERS/UID" + String(ReaderCount) + ".txt";
  createSDFile(fileName); //creates file to add valid UID hashes 
  String fileName1 = "READERS/KEYPAD" + String(ReaderCount) + ".txt";
  createSDFile(fileName1); //creates file to add valid UID hashes 
}

String intToHEX(int num) {   //takes int and makes it a 2 digit hex value
  String ret = String(num, HEX);
  while(ret.length() < 2)
    ret = "0" + ret;
  return ret;
}

String readCharacteristicString(BLECharacteristic charac, int length) {
  uint8_t val[length];
  charac.readValue(val, length);
  String ret = "";
  for(int i = 0; i < length; i++) {
    ret += char(val[i]);
  }
  return ret;
}

void writeHandle(BLEDevice central, BLECharacteristic characteristic) {   //handles event that a characteristic is written to
  if(UIDHash.length() != 64) {
    UIDHash += readCharacteristicString(characteristic, 16);    //adds the 16 byte chunk of the hash to a global string
    characteristic.writeValue("0000000000000000");              //overwrites characteristic to hide previously sent chunk 
  }
  else {
    UIDHash += readCharacteristicString(characteristic, 1);    //adds the single reader number byte to hash
    characteristic.writeValue("0");              //overwrites characteristic to hide previously sent chunk
  }        
  if(UIDHash.length() == 65) {  //when full hash value received plus reader number
    String readerNum = String(UIDHash.charAt(UIDHash.length() - 1));  //save reader number for file name
    String fileName = "READERS/UID" + readerNum + ".txt";
    UIDHash.remove((UIDHash.length() - 1));   //remove reader number from hash
    if(checkSDForString(fileName, UIDHash)) validUID = true;  //check if valid from file given reader number
    UIDHash = ""; //clear Hash string
  }
}

void readHandle(BLEDevice central, BLECharacteristic characteristic) {  //handles event that key characteristic is read
  if(validUID) {
    characteristic.writeValue((byte)0x88);  //send 0x88 if sent hash was valid
    validUID = false;       //reset boolean
  }
  else characteristic.writeValue((byte)0x41); //send 0x41 if not valid hash
}

void addCharacteristics(BLEService service) {  //creates characteristics for each 16 byte chunck of the Hash value, reader number, and key characteristic
  String tempServUUID = service.uuid();
  tempServUUID.remove((tempServUUID.length() - 1));
  tempServUUID.remove((tempServUUID.length() - 1));   //get service UUID excluding the last 2 HEX digits
  for(int i = 0; i < 4; i++) {
    String tempCharacUUID = tempServUUID + intToHEX(i); //define UUID for characterisitc that increments from the service UUID
    BLEStringCharacteristic customCharacteristic(tempCharacUUID.c_str(), BLEWrite, 16); // Define a custom characteristic
    customCharacteristic.writeValue("0000000000000000"); // Initialize the characteristic value
    customCharacteristic.setEventHandler(BLEWritten, writeHandle);    //set event handler for when the characteristics are written to
    service.addCharacteristic(customCharacteristic);  //add characteristic to service
  }

  String tempCharacUUID = tempServUUID + intToHEX(4); 
  BLEStringCharacteristic customCharacteristic(tempCharacUUID.c_str(), BLEWrite, 1); //create fifth characteristic to send the reader number to know which file to check against
  customCharacteristic.writeValue("0");
  customCharacteristic.setEventHandler(BLEWritten, writeHandle);
  service.addCharacteristic(customCharacteristic);

  tempServUUID += intToHEX(5);
  BLEByteCharacteristic keyCharacteristic(tempServUUID.c_str(), BLERead); //create a sixth characteristic that will act as the key characteristic signifying
  keyCharacteristic.writeValue((byte)0x41);                               //an access grant or deny
  keyCharacteristic.setEventHandler(BLERead, readHandle);
  service.addCharacteristic(keyCharacteristic);
}

void BLE_Peripheral_Reset_Services() {  //resets BLE and adds services and characteristics per reader
  BLE.end();
  BLE.begin();
  BLE.setLocalName("EZALServer");  // Set the local name advertised by the server

  String RCount = readSDLine("RCOUNT.txt", 1);
  int ReaderCount = RCount.toInt();
  for(int i = 1; i <= ReaderCount; i++) {   
    String readerUUID = default_uuid;
    readerUUID.remove(0,1);
    readerUUID = String(i, HEX) + readerUUID;
    BLEService service(readerUUID.c_str());   //create services for each reader
    addCharacteristics(service);              //create the characteristics for each service
    BLE.addService(service);                  //add service
  }

  BLE.advertise(); // Start advertising
  Serial.println("Service reset complete");
}
