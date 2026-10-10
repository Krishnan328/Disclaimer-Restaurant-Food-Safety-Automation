#include <Arduino.h>
#define PIN_LDR A0

int mindark = 50;
int maxbright = 950;

unsigned long lastReadTime = 0;

void setup() {
    Serial.begin(9600);
    Serial.println(F("[INFO] LDR Light Sensor Initialized."));
}

void loop() {
    if (millis() - lastReadTime >= 100) {
        lastReadTime = millis();

        // Double read to charge ADC sample capacitor
        analogRead(PIN_LDR);
        int rawVal = analogRead(PIN_LDR);

        // Convert to percentage (0% = Pitch black, 100% = Direct flashlight)
        int lightPercent = map(rawVal, mindark, maxbright, 0, 100);
        lightPercent = constrain(lightPercent, 0, 100);

        Serial.print(F("Raw ADC: "));
        Serial.print(rawVal);
        Serial.print(F(" | Light Level: "));
        Serial.print(lightPercent);
        Serial.print(F("% "));

        // Simple threshold classification
        if (lightPercent < 20) {
            Serial.println(F("[DARK - Night Mode]"));
        } else if (lightPercent > 80) {
            Serial.println(F("[BRIGHT - Direct Light]"));
        } else {
            Serial.println(F("[NORMAL - Ambient Room]"));
        }
    }
}
