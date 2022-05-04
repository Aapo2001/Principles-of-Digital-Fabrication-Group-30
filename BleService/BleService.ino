/*
  Based on https://www.instructables.com/ESP32-BLE-Android-App-Arduino-IDE-AWESOME/
  Edited and added DHT support for testing by Aapo Pihlajaniemi
*/
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#define SERVICE_UUID           "02A7A862-DA15-474E-977B-2B5C5CC6BCDE" 
#define CHARACTERISTIC_UUID_RX "B0E65BA5-6B42-4434-BDCA-3F323A693E13"
#define CHARACTERISTIC_UUID_TX "C8A50DE1-0D11-4272-8A44-22E00ABDB981"
#define RXD2 16
#define TXD2 17

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
int txValue = 0;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();
      if (rxValue.length() > 0) {
        for (int i = 0; i < rxValue.length(); i++) {
          Serial2.write(rxValue[i]);
        }
      }
    }
};

void setup() {
  Serial.begin(57600);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  BLEDevice::init("ESP32"); 

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_TX,
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
                      
  pCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID_RX,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
 
}

void loop() {
  
  
  txValue = analogRead(35);
  Serial.println(txValue);
  if(txValue > 100){
    Serial2.write("A");
    delay(1000);
    Serial2.write("B");
  }
  
  if (deviceConnected) {
   
    pCharacteristic->setValue(txValue);
    pCharacteristic->notify(); // Send the value to the app!
    
  }
  delay(1500);
}
