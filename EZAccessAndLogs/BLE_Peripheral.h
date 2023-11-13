////////////////////////////////////////////////////////////////////////////
//***********************BLE Functions***********************************///
////////////////////////////////////////////////////////////////////////////


#include "BLEService.h"
#include <ArduinoBLE.h>

#define default_uuid "09B10000-E8F2-537E-4F6C-D104768A1200"

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
}

String intToHEX(int num) {   //takes int and makes it a 2 digit hex value
  String ret = String(num, HEX);
  while(ret.length() < 3)
    ret = "0" + ret;
  return ret;
}

void addCharacteristicsFromFile(BLEService service, String fileName) {  //creates characteristics for each line in UID.txt file and adds them to service with incrementing UUIDs
  String tempServUUID = service.uuid();
  tempServUUID.remove((tempServUUID.length() - 1));
  tempServUUID.remove((tempServUUID.length() - 1));   //get service UUID excluding the last 2 HEX digits
  for(int i = 0; i < SDLineCount(fileName); i++) {
    String tempCharacUUID = tempServUUID + intToHEX(i); //define UUID for characterisitc that increments from the service UUID
    BLEStringCharacteristic customCharacteristic(tempCharacUUID.c_str(), BLERead, 64); // Define a custom characteristic
    customCharacteristic.writeValue(readSDLine(fileName, (i + 1))); // Initialize the characteristic value
    service.addCharacteristic(customCharacteristic);  //add characteristic to service
  }
}

void BLE_Peripheral_Reset_Services() {  //resets BLE and adds file contents to new services 
  BLE.end();
  BLE.begin();
  BLE.setLocalName("EZALServer");  // Set the local name advertised by the server

  String RCount = readSDLine("RCOUNT.txt", 1);
  int ReaderCount = RCount.toInt();
  for(int i = 1; i <= ReaderCount; i++) {
    String readerUUID = default_uuid;
    readerUUID.remove(0,1);
    readerUUID = String(i, HEX) + readerUUID;
    String fileName = "READERS/UID" + String(i) + ".txt";
    BLEService service(readerUUID.c_str());
    addCharacteristicsFromFile(service, fileName);
    BLE.addService(service);
  }

  BLE.advertise(); // Start advertising
  Serial.println("Service reset complete");
}

