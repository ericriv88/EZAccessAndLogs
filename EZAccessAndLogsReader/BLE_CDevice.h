#include <ArduinoBLE.h>

const int charCount = 64;

String readCharacteristicString(BLECharacteristic charac, int length) {
  uint8_t val[length];
  charac.readValue(val, length);
  String ret = "";
  for(int i = 0; i < length; i++) {
    ret += char(val[i]);
  }
  return ret;
}

bool BLELookForService(String UIDHash) {
  Serial.print("Starting BLE connection... ");

  if (!BLE.begin()) {
    Serial.println("failed!");
    return false;
  }
  else {
    Serial.println("success!");
    Serial.println("Scanning for \"EZALServer\"");
    BLE.scanForName("EZALServer");
    BLEDevice peripheral = BLE.available();
    int count = 0;
    while(!peripheral) {
      peripheral = BLE.available();
      delay(500);
      count++;
      if(count == 6) {
        BLE.stopScan();
        return false;
      }
    }
    BLE.stopScan();

    if(peripheral.connect()) {
      Serial.println("Connected to peripheral");
      if(peripheral.discoverAttributes()) {
        if(peripheral.hasService(default_uuid))
          Serial.println("Found default service");

          BLEService service = peripheral.service(default_uuid);
          Serial.print("Reading characteristic strings from service ");
          Serial.print(service.uuid());
          Serial.println(":");
          for(int j = 0; j < service.characteristicCount(); j++) {
            BLECharacteristic charac = service.characteristic(j);
            String mystring = readCharacteristicString(charac, charCount);
            if(UIDHash == mystring) {
              peripheral.disconnect();
              return true;
            }
          }
          Serial.println();
          peripheral.disconnect();
          return false;
      }
    }
  return false;
  }
}