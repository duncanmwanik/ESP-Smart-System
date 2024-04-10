#include "def.h"

int isMatSteppedOn()
{
    bool isSteppedOn = !digitalRead(PRESSURE_PIN);
    Serial.println(isSteppedOn ? "Pressure mat is stepped on." : "Pressure mat not stepped on.");
    return isSteppedOn;
}
