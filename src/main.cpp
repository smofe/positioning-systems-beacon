#include "Arduino.h"
#include "sys/time.h"
#include "BLEDevice.h"
#include "BLEUtils.h"
#include "BLEServer.h"
#include "BLEBeacon.h"
#include "EEPROM.h"



#define EEPROM_SIZE 64
#define BEACON_UUID "87b99b2c-90fd-11e9-bc42-526af7764f64" 
#define SERVICE_UUID "aab96cca-3d21-4374-ba0d-28c9954f1221"
#define CHARACTERISTIC_NAME_UUID "eeec3b9b-3f42-46d1-b2bc-bb9ce1eaee35"

void initBLEDevice(std::string name);
void setBeacon();

BLEAdvertising *pAdvertising;   
BLECharacteristic *nameCharacteristic;
int addr = 0;


class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      Serial.println("Device connected");
    };

    void onDisconnect(BLEServer* pServer) {
      // saving the value of name characterisitic in eeprom storage and restarting the device.
      Serial.println("Device disconnected - Saving name and restarting...");
      // writing byte-by-byte to EEPROM
      for (int i = 0; i < EEPROM_SIZE; i++) {
          EEPROM.write(addr, nameCharacteristic->getValue().c_str()[i]);
          addr += 1;
      }
      EEPROM.commit();
      ESP.restart();
      
        
    }
};

void setBeacon() {

  BLEBeacon oBeacon = BLEBeacon();
  oBeacon.setManufacturerId(0x4C00); // fake Apple 0x004C LSB (ENDIAN_CHANGE_U16!)
  oBeacon.setProximityUUID(BLEUUID(BEACON_UUID));
  oBeacon.setMajor(0);
  oBeacon.setMinor(0);
  BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
  BLEAdvertisementData oScanResponseData = BLEAdvertisementData();

  oAdvertisementData.setFlags(0x04); // BR_EDR_NOT_SUPPORTED 0x04

  std::string strServiceData = "";

  strServiceData += (char)26;     // Len
  strServiceData += (char)0xFF;   // Type
  strServiceData += oBeacon.getData();
  oAdvertisementData.addData(strServiceData);

  pAdvertising->setAdvertisementData(oAdvertisementData);
  pAdvertising->setScanResponseData(oScanResponseData);
}

void initBLEDevice(std::string name) {
  BLEDevice::init(name);
  Serial.println(BLEDevice::toString().c_str());
  BLEDevice::setPower(ESP_PWR_LVL_P9, ESP_BLE_PWR_TYPE_ADV);
  BLEServer *pServer = BLEDevice::createServer(); 
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);
  nameCharacteristic = pService->createCharacteristic(CHARACTERISTIC_NAME_UUID,BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE); 
  nameCharacteristic->setValue(name);      
  pService->start();

  pAdvertising = BLEDevice::getAdvertising();
  BLEDevice::startAdvertising();
  setBeacon();

  pAdvertising->start();
}

void setup() {

  Serial.begin(115200);
  if (!EEPROM.begin(EEPROM_SIZE)) {
    Serial.println("failed to init EEPROM");
    delay(1000000);
  }


  // read the name of this beacon from EEPROM storage and init the BLE Device
  char id[64];
  id[0] = 0;
  // reading byte-by-byte from EEPROM
  for (int i = 0; i < EEPROM_SIZE; i++) {
    byte readValue = EEPROM.read(i);
    if (readValue == 0) {
        break;
    }
    char readValueChar = char(readValue);
    Serial.print(readValueChar);
    id[i] = readValueChar;
  }
  if (id[0] == 0) {
    Serial.println("Not yet configured. Choosing default name.");
    initBLEDevice("dPS Beacon");
  } else {
    initBLEDevice(id);
  }
  
 
  Serial.println("Setup completed. Advertising started...");
  delay(100);
}

void loop() {

}
