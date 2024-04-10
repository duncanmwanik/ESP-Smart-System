#include "def.h"

void enterDeepSleep()
{
    esp_sleep_enable_ext0_wakeup(PRESSURE_PIN, 1);
    Serial.println("Sleeping now...");
    esp_deep_sleep_start();
}
