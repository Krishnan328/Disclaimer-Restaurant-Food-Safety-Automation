/**
 * @file Ultrasonic_HC_SR04.ino
 * @brief Interfacing HC-SR04 Ultrasonic Distance Sensor with timeout protection & spike filtering.
 * 
 * LIBRARIES:
 * - Native (No external library required, but compatible with "NewPing")
 * 
 * WIRING:
 * - VCC  -> 5V
 * - GND  -> GND
 * - TRIG -> Arduino Pin 4
 * - ECHO -> Arduino Pin 5
 * 
 * CRITICAL EMBEDDED NOTE:
 * Standard pulseIn() can block for 1 full second if no echo is returned!
 * Always specify a timeout: pulseIn(ECHO_PIN, HIGH, 30000) (limits wait to 30ms = ~5 meters max).
 */

const uint8_t PIN_TRIG = 4;
const uint8_t PIN_ECHO = 5;

// Simple 3-sample median filter to eliminate ultrasonic sonar dropouts
float filterSpike(float sample) {
    static float s1 = 0, s2 = 0, s3 = 0;
    s1 = s2;
    s2 = s3;
    s3 = sample;
    // Return middle value
    if ((s1 <= s2 && s2 <= s3) || (s3 <= s2 && s2 <= s1)) return s2;
    if ((s2 <= s1 && s1 <= s3) || (s3 <= s1 && s1 <= s2)) return s1;
    return s3;
}

float readUltrasonicCm() {
    // Clear trigger
    digitalWrite(PIN_TRIG, LOW);
    delayMicroseconds(2);

    // Send 10us HIGH pulse
    digitalWrite(PIN_TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(PIN_TRIG, LOW);

    // Read echo with 30,000 microsecond timeout (~5 meters max)
    unsigned long duration = pulseIn(PIN_ECHO, HIGH, 30000UL);

    if (duration == 0) {
        return -1.0; // Out of range or timed out
    }

    // Speed of sound: 343 m/s = 0.0343 cm/us -> Distance = (duration * 0.0343) / 2
    float rawCm = duration * 0.01715f;
    return filterSpike(rawCm);
}

unsigned long lastSonarRead = 0;
const unsigned long SONAR_INTERVAL_MS = 60; // 60ms between pings to let ultrasonic reflections settle

void setup() {
    Serial.begin(115200);
    pinMode(PIN_TRIG, OUTPUT);
    pinMode(PIN_ECHO, INPUT);
    Serial.println(F("[INFO] HC-SR04 Ultrasonic Initialized."));
}

void loop() {
    if (millis() - lastSonarRead >= SONAR_INTERVAL_MS) {
        lastSonarRead = millis();

        float distance = readUltrasonicCm();
        if (distance >= 0) {
            Serial.print(F("Distance: "));
            Serial.print(distance, 1);
            Serial.println(F(" cm"));
        } else {
            Serial.println(F("Distance: [Out of Range / > 400cm]"));
        }
    }
}
