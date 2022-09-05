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
    // int foo = analogRead(ntc_pin);
    // Serial.println(foo);
    // Serial.println(maths::calc_temp(foo));
    // double temp = foo * (3.3 / 4095) - 1.65;
    // Serial.println(temp);
    // Serial.println(205 - temp * (155. / 1.65));
    double average = 0;
    for (int idx{}; idx < sampling_rate; idx++) {
        average += analogRead(ntc_pin);
        delay(10);
    }
    average /= sampling_rate;

    double resistance = maths::calc_resistance(average);
    Serial.println(resistance);
    Serial.println(maths::temperature_c(resistance));
    Serial.println(maths::calc_temp(average));
    // Serial.println(maths::temperature_c(6180));  // 38 C
    // Serial.println(maths::temperature_c(2980));  // 60 C
    // Serial.println(maths::temperature_c(18400));  // 10 C
    // Serial.println(maths::temperature_c(667828));  // -50 C
    // Serial.println(maths::temperature_c(587.31));  // 105 C
    
    delay(1000);
}
