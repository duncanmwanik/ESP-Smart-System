#include <Arduino.h>
#include "BLEDevice.h"

#define LIGHTS_PIN 4

static BLEUUID bmeServiceUUID("6cc9c618-9f80-4147-bd7c-ac75e7de6ca5");
static BLEUUID pressureCharacteristicUUID("12339484-d62d-4a19-8175-28600ee5afc7");
static BLEAddress *pServerAddress;
static BLERemoteCharacteristic *pressureCharacteristic;

static bool startConnectionToSender = false;
static bool connectedToSender = false;

static bool isMatSteppedOn = false;

// Timing variables
// We restart the receiver if it has not connected to sender for some time
// Helps avoid failed reconnections
unsigned long lastTime = 0;
unsigned long idleTimeToRestart = 10000;

// Callback function that gets called, when another device's advertisement has been received
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice advertisedDevice)
    {
        // if we find the sender, we stop the scan and connect to it
        if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(bmeServiceUUID))
        {
            advertisedDevice.getScan()->stop();
            pServerAddress = new BLEAddress(advertisedDevice.getAddress());
            startConnectionToSender = true;
            Serial.println("Found sender. Connecting...");
        }
    }
};

class MyClientCallback : public BLEClientCallbacks
{
    void onConnect(BLEClient *pclient)
    {
        Serial.println("Connected to sender...");
        lastTime = millis(); // prevent idle restart since connection is present
    }

    void onDisconnect(BLEClient *pclient)
    {
        Serial.println("Disconnected from sender. Restarting...\n\n");
        ESP.restart();
    }
};

bool connectToSender(BLEAddress pAddress)
{
    BLEClient *pClient = BLEDevice::createClient();
    pClient->setClientCallbacks(new MyClientCallback());

    pClient->connect(pAddress);

    // get sender BLE service
    BLERemoteService *pRemoteService = pClient->getService(bmeServiceUUID);
    if (pRemoteService == nullptr)
    {
        Serial.print("Failed to find sender service. UUID: ");
        Serial.println(bmeServiceUUID.toString().c_str());
        return (false);
    }

    // get sender pressure characteristic
    pressureCharacteristic = pRemoteService->getCharacteristic(pressureCharacteristicUUID);
    if (pressureCharacteristic == nullptr)
    {
        Serial.println("Failed to find sender characteristic.");
        return false;
    }

    return true;
}

void setup()
{
    Serial.begin(115200);
    Serial.println("\n\nStarting BLE Receiver...");

    // set lights pin to output
    pinMode(LIGHTS_PIN, OUTPUT);

    // Start receiver BLE and scan for devices
    BLEDevice::init("ESP Receiver");
    BLEScan *pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->start(30);
}

void loop()
{
    if (startConnectionToSender == true)
    {
        if (connectToSender(*pServerAddress))
        {
            connectedToSender = true;
        }
        else
        {
            Serial.println("Failed to connect to sender; Restarting receiver...\n\n");
            ESP.restart();
        }

        startConnectionToSender = false;
    }

    if (connectedToSender)
    {
        String pressureValue = String(pressureCharacteristic->readValue().c_str());

        if (pressureValue == "1" || pressureValue == "0")
        {
            Serial.println("isMatSteppedOn: " + pressureValue);
            isMatSteppedOn = pressureValue == "1";

            if (isMatSteppedOn)
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

        // This delay prevents multiple reads of sender data on the same connection
        delay(500);
    }

    // This time will restart the receiver if a connection has not been made to sender after 10 seconds.
    if (((millis() - lastTime) > idleTimeToRestart) && !connectedToSender)
    {
        Serial.println("Restarting due to idleness...\n\n");
        ESP.restart();
    }
}