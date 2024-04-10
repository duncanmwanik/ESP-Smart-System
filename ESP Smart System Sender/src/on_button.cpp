#include "def.h"

bool isButtonOn()
{
    bool isOn = !digitalRead(ON_OFF_BUTTON_PIN);
    Serial.println(isOn ? "Sender is on." : "Sender is off.");
    return isOn;
}