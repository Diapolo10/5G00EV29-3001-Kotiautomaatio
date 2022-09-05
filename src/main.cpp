#include <Arduino.h>
#include <WiFi.h>
#include "temperature.hpp"

static const int ntc_pin{A0};
static const int vd_power_pin{2};
static const int sampling_rate{5};

static double adc_value{};  // Ohm

void setup() {
    Serial.begin(115200);

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    Serial.println("Setup done");

    for (int idx{}; idx < 10; ++idx) {
        Serial.println(maths::temperature_c(maths::temperature_resistance(1000 * idx)));
    }
}

void loop() {

}
