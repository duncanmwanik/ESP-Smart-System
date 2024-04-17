#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVICE_UUID "6cc9c618-9f80-4147-bd7c-ac75e7de6ca5"
BLECharacteristic pressureCharacteristic("12339484-d62d-4a19-8175-28600ee5afc7", BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor pressureDescriptor(BLEUUID((uint16_t)0x2902));

bool deviceConnected = false;
bool hasReceiverReadData = false;

// Callbacks for onConnect and onDisconnect receiver
class MyServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer)
    {
        deviceConnected = true;
        Serial.println("Receiver connected...");
    };
    void onDisconnect(BLEServer *pServer)
    {
        deviceConnected = false;
        Serial.println("Receiver disconnected...");
    }
};

class CharacteristicCallbacks : public BLECharacteristicCallbacks
{
    void onRead(BLECharacteristic *pCharacteristic)
    {
        hasReceiverReadData = true;
    };
};

void sendBLEData(int pressureValue)
{
    // Create the BLE Device
    BLEDevice::init("ESP Sender");
    // Create the BLE Server
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    // Create the BLE Service
    BLEService *pressureService = pServer->createService(SERVICE_UUID);
    // Add pressure value characteristic
    pressureService->addCharacteristic(&pressureCharacteristic);
    pressureCharacteristic.setCallbacks(new CharacteristicCallbacks());
    pressureCharacteristic.addDescriptor(&pressureDescriptor);
    pressureService->start();

    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pServer->getAdvertising()->start();

    // set BLE pressure data
    pressureCharacteristic.setValue(String(pressureValue).c_str());
    pressureCharacteristic.notify();

    Serial.println("Waiting for receiver to read data...");
}
