#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include "temperature.hpp"
#include "secrets.h"  // ssid, password, webhooks_key, event_name

static const int pot_pin{A2};
static const int ntc_pin{A3};
static const int vd_power_pin{2};  // 3.3V
static const int sampling_rate{5};
static const int target_temperature{38};  // C

int samples[sampling_rate];
int bath_temperature = 10;
int heated_temperature = 0;
int bath_timer{};
int heater_timer{};
bool send_emails{true};

String get_applet_address() {
    String address = "http://maker.ifttt.com/trigger/";
    address += event_name;
    address += "/with/key/";
    address += webhooks_key;
    return address;
}

void init_wifi(){
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to Wi-Fi...");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(1000);
    }
    Serial.print('\n');
    Serial.println(WiFi.localIP());
}

void send_email(String message, String measured_temps) {
    HTTPClient http;

    String url = get_applet_address();
    url += "?value1=" + message;
    url += "?value2=" + String(target_temperature);
    url += "?value3=" + measured_temps;

    http.begin(url.c_str());

    int http_response_code = http.GET();

    if (http_response_code > 0) {
        Serial.print("HTTP Response Code: ");
        Serial.println(http_response_code);
        String payload = http.getString();
        Serial.println(payload);
    }
    else {
        Serial.print("Error code: ");
        Serial.println(http_response_code);
    }

    http.end();
}

void send_heating_email(int duration, String measured_temps) {
    send_email(
        "Allasta on kuumennettu " + String(duration) + " minuuttia",
        measured_temps
    );
}

void send_firewood_email(String measured_temps) {
    send_email(
        "Polttopuuta tarvitaan",
        measured_temps
    );
}

void send_ready_email(String measured_temps) {
    send_email(
        "Allas on valmis",
        measured_temps
    );
}

void setup() {
    Serial.begin(115200);

    init_wifi();

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

double measure_temperature(int pin) {
    double average{};
    for (int idx{}; idx < sampling_rate; idx++) {
        average += analogRead(pin);
        delay(10);
    }
    average /= sampling_rate;
    return average;
}

String temperature_message() {
    String message = "Pool temperature: ";
    message += String(bath_temperature);
    message += "\nHeated water temperature: ";
    message += String(heated_temperature);
    return message;
}

void loop() {
    // int foo = analogRead(ntc_pin);
    // Serial.println(foo);
    // Serial.println(maths::calc_temp(foo));
    // double temp = foo * (3.3 / 4095) - 1.65;
    // Serial.println(temp);
    // Serial.println(205 - temp * (155. / 1.65));
    
    bath_temperature = maths::calc_temp(measure_temperature(ntc_pin));
    heated_temperature = maths::calc_temp(measure_temperature(pot_pin));

    Serial.print("Bath temperature [C]: ");
    Serial.println(bath_temperature);
    Serial.print("Heated water temperature [C]: ");
    Serial.println(heated_temperature);

    if (bath_temperature >= target_temperature) {
        String message = temperature_message();
        send_ready_email(message);
        send_emails = false;
        Serial.end();
    }
    else if (bath_timer == 0) {
        Serial.print("Temperature low; starting heating");
        send_heating_email(
            bath_timer,
            temperature_message()
        );
        bath_timer = millis() / 60000;
    }
    else if (bath_timer+10 <= millis() / 60000) {
        send_heating_email(
            bath_timer,
            temperature_message()
        );
        bath_timer = millis() / 60000;
    }

    double ntc_resistance = maths::calc_resistance(bath_temperature);
    double pot_resistance = maths::calc_resistance(heated_temperature, false);

    Serial.println(
      "NTC: " + String(ntc_resistance) + " Ω"
      + "\tPOT: " + String(pot_resistance) + " Ω"
    );
    Serial.println(
      "NTC: " + String(maths::temperature_c(ntc_resistance)) + " ℃"
      + "\tPOT: " + String(maths::temperature_c(pot_resistance)) + " ℃"
    );
    Serial.println(
      "NTC: " + String(maths::calc_temp(bath_temperature)) + " ℃"
      + "\tPOT: " + String(maths::calc_temp(heated_temperature, false)) + " ℃"
    );

    if (heater_timer == 0 || heater_timer+10 < millis() / 60000 && heated_temperature < 60) {
        send_firewood_email(
            temperature_message()
        );
        heater_timer = millis() / 60000;
    }
    // Serial.println(maths::temperature_c(6180));  // 38 C
    // Serial.println(maths::temperature_c(2980));  // 60 C
    // Serial.println(maths::temperature_c(18400));  // 10 C
    // Serial.println(maths::temperature_c(667828));  // -50 C
    // Serial.println(maths::temperature_c(587.31));  // 105 C

    delay(1000);
}
