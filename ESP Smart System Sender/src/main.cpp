#include <Arduino.h>
#include "ble.h"

#define PRESSURE_MAT_PIN GPIO_NUM_33
#define ON_OFF_BUTTON_PIN 16
#define LIGHT_SENSOR_PIN 4
#define LIGHT_THRESHOLD 500

bool isSenderOn()
{
    bool isOn = digitalRead(ON_OFF_BUTTON_PIN);
    Serial.println(isOn ? "Sender is on." : "Sender is off.");
    return isOn;
}

bool isDark()
{
    bool isDark = analogRead(LIGHT_SENSOR_PIN) < LIGHT_THRESHOLD;
    Serial.println(isDark ? "It is dark." : "Not dark yet.");
    return isDark;
}

int isMatSteppedOn()
{
    bool isSteppedOn = digitalRead(PRESSURE_MAT_PIN);
    Serial.println(isSteppedOn ? "Pressure mat is stepped on." : "Pressure mat not stepped on.");
    return isSteppedOn;
}

void goToSleep()
{
    Serial.println("Going to sleep...\n\n");
    // Wake up if the pressure mat pin is HIGH (1)
    esp_sleep_enable_ext0_wakeup(PRESSURE_MAT_PIN, 1);
    esp_deep_sleep_start();
}

void setup()
{
    Serial.begin(115200);
    Serial.println("\n\nStarting device...");

    // set pin modes
    pinMode(PRESSURE_MAT_PIN, INPUT_PULLDOWN);
    pinMode(ON_OFF_BUTTON_PIN, INPUT_PULLDOWN);

    if (isSenderOn())
    {
        if (isDark())
        {
            sendBLEData(isMatSteppedOn());
        }
        else
        {
            goToSleep();
        }
    }
    else
    {
        goToSleep();
    }
}

void loop()
{
    if (hasReceiverReadData)
    {
        Serial.println("Receiver has read the data...");
        // This delay allows receiver enough time to read the data
        delay(500);
        goToSleep();
    }
}
