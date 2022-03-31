/*
  Based on https://www.instructables.com/ESP32-BLE-Android-App-Arduino-IDE-AWESOME/
  Edited and added DHT support for testing by Aapo Pihlajaniemi
*/
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <DHT.h>
#define DHT_SENSOR_TYPE DHT22
#define DHT_SENSOR_PIN  21
#define SERVICE_UUID           "02A7A862-DA15-474E-977B-2B5C5CC6BCDE" 
#define CHARACTERISTIC_UUID_RX "B0E65BA5-6B42-4434-BDCA-3F323A693E13"
#define CHARACTERISTIC_UUID_TX "C8A50DE1-0D11-4272-8A44-22E00ABDB981"
DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);
BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
float txValue = 0;


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
        Serial.println("*********");
        Serial.print("Received Value: ");

        for (int i = 0; i < rxValue.length(); i++) {
          Serial.print(rxValue[i]);
        }

        Serial.println();

        if (rxValue.find("A") != -1) { //Later connect to arduino board to sent that signal
          Serial.print("Opening solenoid");
        }
        else if (rxValue.find("B") != -1) {
          Serial.print("Closing solenoid");
        }
        else if (rxValue.find("C") != -1) {
          Serial.print("Turning automatic watering on");
        }
        else if (rxValue.find("D") != -1) {
          Serial.print("Turning automatic watering off");
        }

        Serial.println();
        Serial.println("*********");
      }
    }
};

void setup() {
  Serial.begin(9600);
  
  dht_sensor.begin();
 
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
  Serial.println("Waiting a client connection to notify...");
}

void loop() {
  
  txValue = dht_sensor.readHumidity(); 
  Serial.print("Humidity Value: ");
  Serial.println(txValue);
  
  if (deviceConnected) {
   
    pCharacteristic->setValue(txValue);
    pCharacteristic->notify(); // Send the value to the app!
    
  }
  delay(1000);
}
