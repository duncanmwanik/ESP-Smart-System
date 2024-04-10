#include <Arduino.h>
#include "BLEDevice.h"

#define LIGHTS_PIN 4

static BLEUUID senderUUID("d40d0e2d-0f4c-4a82-b769-c83863fad3d1");
static BLEUUID senderCharacteristicUUID("e3e37507-a592-407a-904d-219cb1dc09ad");

bool turnOnLights = false;

static bool doConnect = false;
static bool connected = false;

static BLEAddress *pServerAddress; // the sender address
static BLERemoteCharacteristic *senderCharacteristic;

class MyClientCallback : public BLEClientCallbacks
{
    void onConnect(BLEClient *pclient)
    {
    }

    void onDisconnect(BLEClient *pclient)
    {
        connected = false;
        Serial.println("Disconnected from sender...");
    }
};

// Connect to the BLE Server that has the name, Service, and Characteristics
bool connectToServer()
{
    Serial.println("Connecting to sender...");
    BLEClient *pClient = BLEDevice::createClient();
    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to sender
    pClient->connect(*pServerAddress);

    BLERemoteService *pRemoteService = pClient->getService(senderUUID);
    if (pRemoteService == nullptr)
    {
        Serial.print("Failed to find our service UUID: ");
        Serial.println(senderUUID.toString().c_str());
        pClient->disconnect();
        return (false);
    }

    senderCharacteristic = pRemoteService->getCharacteristic(senderCharacteristicUUID);

    if (senderCharacteristic == nullptr)
    {
        Serial.print("Failed to find our characteristic UUID");
        pClient->disconnect();
        return false;
    }

    return true;
}

// Callback function that gets called, when another device's advertisement has been received
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice advertisedDevice)
    {
        Serial.print("BLE Advertised Device found: ");
        Serial.println(advertisedDevice.toString().c_str());

        // if we find the sender, we stop the scan and connect to it
        if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(senderUUID))
        {
            BLEDevice::getScan()->stop();
            pServerAddress = new BLEAddress(advertisedDevice.getAddress());
            doConnect = true;
        }
    }
};

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting receiver...");

    // set lights pin to output
    pinMode(LIGHTS_PIN, OUTPUT);

    // Init BLE device
    BLEDevice::init("");
    BLEScan *pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->start(30);
}

void loop()
{
    if (doConnect == true)
    {
        if (connectToServer())
        {
            Serial.println("Connected to sender...");
            connected = true;
        }
        else
        {
            Serial.println("Failed to connect to the sender...");
            connected = false;
        }

        doConnect = false;
    }

    if (connected)
    {
        String pressureData = String(senderCharacteristic->readValue().c_str());

        turnOnLights = pressureData == "1";

        if (turnOnLights)
        {
            digitalWrite(LIGHTS_PIN, HIGH);
            Serial.println("Turned on lights!");
        }
        else
        {
            digitalWrite(LIGHTS_PIN, LOW);
            Serial.println("Turned off lights!");
        }
    }
    else
    {
        // start scanning for the sender device again
        BLEDevice::getScan()->start(0);
    }
}