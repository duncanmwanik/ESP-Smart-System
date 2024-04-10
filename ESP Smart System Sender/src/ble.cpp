#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

bool deviceConnected = false;
bool hasReceiverReadData = false;

#define SERVICE_UUID "d40d0e2d-0f4c-4a82-b769-c83863fad3d1"
#define CHARACTERISTIC_UUID "e3e37507-a592-407a-904d-219cb1dc09ad"

BLECharacteristic dataCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
BLEDescriptor dataDescriptor(BLEUUID((uint16_t)0x2902));

// Setup callbacks onConnect and onDisconnect
class MyServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer)
    {
        Serial.println("Receiver connected...");
        deviceConnected = true;
    };
    void onDisconnect(BLEServer *pServer)
    {
        Serial.println("Receiver disconnected...");
        deviceConnected = false;
    };
};

class CharacteristicCallbacks : public BLECharacteristicCallbacks
{
    void onRead(BLECharacteristic *pCharacteristic)
    {
        hasReceiverReadData = true;
        Serial.println("Receiver has read the data...");
    };
};

void sendBLEData(int pressureValue)
{
    Serial.println("Sending BLE pressure value: " + String(pressureValue));

    // Create the BLE Device
    BLEDevice::init("ESP Smart");
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    BLEService *bleService = pServer->createService(SERVICE_UUID);
    bleService->addCharacteristic(&dataCharacteristic);
    dataCharacteristic.setCallbacks(new CharacteristicCallbacks());
    dataDescriptor.setValue("Pressure Mat Data");
    dataCharacteristic.addDescriptor(&dataDescriptor);
    bleService->start();

    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pServer->getAdvertising()->start();
    Serial.println("Waiting for receiver to connect...");

    while (!hasReceiverReadData)
    {
        if (deviceConnected)
        {
            // notify receiver of pressure value
            dataCharacteristic.setValue(String(pressureValue).c_str());
            dataCharacteristic.notify();
        }

        Serial.println("Waiting for receiver to read pressure value...");
    }

    hasReceiverReadData = false;
    Serial.println("Going back to sleep...");
}
