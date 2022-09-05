#include <Arduino.h>
#include <WiFi.h>
#include "temperature.hpp"

static const int pot_pin{A2};
static const int ntc_pin{A3};
static const int vd_power_pin{2};  // 3.3V
static const int sampling_rate{5};
static const String ssid{""};
static const String password{""};

int samples[sampling_rate];

void setup() {
    Serial.begin(115200);

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    Serial.println("Setup done");

    for (int idx{}; idx < 10; ++idx) {
        Serial.println(maths::temperature_c(maths::temperature_resistance(1000 * idx)));
    }

    pinMode(vd_power_pin, OUTPUT);
    pinMode(ntc_pin, INPUT);
    Serial.print("Set ");
    Serial.print(vd_power_pin);
    Serial.println(" as output pin");
    Serial.print("Set ");
    Serial.print(ntc_pin);
    Serial.println(" as input pin");
}

void loop() {
    double ntc_average = 0;
    double pot_average = 0;
    for (int idx{}; idx < sampling_rate; idx++) {
        ntc_average += analogRead(ntc_pin);
        pot_average += analogRead(pot_pin);
        delay(10);
    }
    ntc_average /= sampling_rate;
    pot_average /= sampling_rate;

    double ntc_resistance = maths::calc_resistance(ntc_average);
    double pot_resistance = maths::calc_resistance(pot_average, false);

    Serial.println(
      "NTC: " + String(ntc_resistance) + " Ω"
      + "\tPOT: " + String(pot_resistance) + " Ω"
    );
    Serial.println(
      "NTC: " + String(maths::temperature_c(ntc_resistance)) + " ℃"
      + "\tPOT: " + String(maths::temperature_c(pot_resistance)) + " ℃"
    );
    Serial.println(
      "NTC: " + String(maths::calc_temp(ntc_average)) + " ℃"
      + "\tPOT: " + String(maths::calc_temp(pot_average, false)) + " ℃"
    );

    // Serial.println(maths::temperature_c(6180));  // 38 C
    // Serial.println(maths::temperature_c(2980));  // 60 C
    // Serial.println(maths::temperature_c(18400));  // 10 C
    // Serial.println(maths::temperature_c(667828));  // -50 C
    // Serial.println(maths::temperature_c(587.31));  // 105 C

    delay(1000);
}
