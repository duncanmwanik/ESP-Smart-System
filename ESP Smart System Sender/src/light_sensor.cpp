#include "def.h"

bool isDark()
{
    bool isDark = analogRead(LIGHT_SENSOR_PIN) < LIGHT_THRESHOLD;
    Serial.println(isDark ? "It is dark." : "Not dark yet.");
    return isDark;
}
