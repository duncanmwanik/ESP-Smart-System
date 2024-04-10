#include "def.h"

//
// Check def.h for definitions.
//

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting device...");

    // set pin modes
    pinMode(PRESSURE_PIN, INPUT_PULLUP);
    pinMode(ON_OFF_BUTTON_PIN, INPUT_PULLUP);

    if (isButtonOn())
    {
        if (isDark())
        {
            sendBLEData(isMatSteppedOn());
        }
    }

    enterDeepSleep();
}

void loop()
{
}