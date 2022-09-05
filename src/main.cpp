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

double calibratedAnalogRead(uint8_t pin) {
  double calibration_value = 0.15 / (3.3 / 4095);

  int value = analogRead(pin);
  if (value > 0)
    return value + calibration_value;

  return value;
}

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
    //double calibration_value = 0.15 / (3.3 / 4095);
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

    double ntc_value_in = 0;
    double pot_value_in = 0;

    ntc_value_in += calibratedAnalogRead(ntc_pin);
    pot_value_in += calibratedAnalogRead(pot_pin);

    Serial.println("\nanalogRead(ntc_pin): " + String(ntc_value_in * (3.3 / 4095)) + " V (" + String(ntc_value_in) + ")");
    Serial.println("analogRead(pot_pin): " + String(pot_value_in * (3.3 / 4095)) + " V (" +  String(pot_value_in) + ")");

    //Serial.println("\nNTC reading: " + String(ntc_average * (3.3 / 4095)) + " V (" + String(ntc_average) + ")");
    //Serial.println("POT reading: " + String(pot_average * (3.3 / 4095)) + " V (" + String(pot_average) + ")");

    /*Serial.println(
      "\nNTC: " + String(ntc_resistance) + " Ω"
      + "\tPOT: " + String(pot_resistance) + " Ω"
    );
    Serial.println(
      "NTC: " + String(maths::temperature_c(ntc_resistance)) + " ℃"
      + "\tPOT: " + String(maths::temperature_c(pot_resistance)) + " ℃"
    );
    Serial.println(
      "NTC: " + String(maths::calc_temp(ntc_average)) + " ℃"
      + "\tPOT: " + String(maths::calc_temp(pot_average, false)) + " ℃"
    );*/

    // Serial.println(maths::temperature_c(6180));  // 38 C
    // Serial.println(maths::temperature_c(2980));  // 60 C
    // Serial.println(maths::temperature_c(18400));  // 10 C
    // Serial.println(maths::temperature_c(667828));  // -50 C
    // Serial.println(maths::temperature_c(587.31));  // 105 C

    delay(1000);
}
