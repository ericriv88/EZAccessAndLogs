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

uint8_t readCharacteristicByte(BLECharacteristic charac) {
  uint8_t val;
  charac.readValue(val);
  return val;
}

void writeCharacteristic(BLECharacteristic charac, String in) {
  if(charac.canWrite()) {
    charac.writeValue(in.c_str());
  }
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
        if(peripheral.hasService(default_uuid)) {
          Serial.println("Found default service");
          BLEService service = peripheral.service(default_uuid);
          for(int j = 0; j < service.characteristicCount(); j++) {
            BLECharacteristic charac = service.characteristic(j);
            if(charac.canRead()) {
              uint8_t check = readCharacteristicByte(charac);
              if(check == 0x88) {
                peripheral.disconnect();
                return true;
              }
            }
            if(j == 4) writeCharacteristic(charac, String(default_uuid[0]));
            else writeCharacteristic(charac, UIDHash.substring((16*j), (16*(j+1))));
          }
          Serial.println();
          peripheral.disconnect();
          return false;
        }
      }
      peripheral.disconnect();
    }
  return false;
  }
}
