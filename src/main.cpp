// #include <Arduino.h>
// #include <BLEDevice.h>
// #include <BLEUtils.h>
// #include <BLEServer.h>
// #include <BLEBeacon.h>

// #define BEACON_UUID "87b99b2c-90fd-11e9-bc42-526af7764f64" 

// BLEAdvertising *pAdvertising;   // BLE Advertisement type

// void setup() {
//   Serial.begin(9600);
//   Serial.println("Setting up beacon!");

//   BLEDevice::init("ESP32 Beacon");
//   BLEServer *pServer = BLEDevice::createServer();
  
//   pAdvertising = BLEDevice::getAdvertising();
//   BLEDevice::startAdvertising();
  
//   BLEBeacon beacon = BLEBeacon();
//   beacon.setManufacturerId(0x4C00); // fake Apple 0x004C LSB (ENDIAN_CHANGE_U16!)
//   beacon.setProximityUUID(BLEUUID(BEACON_UUID));
//   beacon.setMajor((0xFFFF0000) >> 16);
//   beacon.setMinor(0xFFFF);
//   BLEAdvertisementData advertisementData = BLEAdvertisementData();
//   BLEAdvertisementData scanResponseData = BLEAdvertisementData();
//   advertisementData.setFlags(0x04); // BR_EDR_NOT_SUPPORTED 0x04
//   advertisementData.addData("patient-AZ5A");
//   pAdvertising->setAdvertisementData(advertisementData);
//   //pAdvertising->setScanResponse(true);
//   pAdvertising->setScanResponseData(scanResponseData);

//   pAdvertising->start();
  
//   Serial.println("Advertising started");

// }

// void loop() {
//   pAdvertising->start();
//   Serial.println("Advertizing started...");
//   delay(100);
//   pAdvertising->stop();
// }

#include "Arduino.h"
#include "sys/time.h"
#include "BLEDevice.h"
#include "BLEUtils.h"
#include "BLEServer.h"
#include "BLEBeacon.h"

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
BLEAdvertising *pAdvertising;   // BLE Advertisement type

#define BEACON_UUID "87b99b2c-90fd-11e9-bc42-526af7764f64" // UUID 1 128-Bit (may use linux tool uuidgen or random numbers via https://www.uuidgenerator.net/)

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

void setup() {

  Serial.begin(115200);
 
  // Create the BLE Device
  BLEDevice::init("patient-4");
  BLEDevice::setPower(ESP_PWR_LVL_P9, ESP_BLE_PWR_TYPE_ADV);
  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer(); // <-- no longer required to instantiate BLEServer, less flash and ram usage
  pAdvertising = BLEDevice::getAdvertising();
  BLEDevice::startAdvertising();
  setBeacon();
   // Start advertising
  pAdvertising->start();
 
  Serial.println("Advertising started...");
  delay(100);
}

void loop() {
 
}
