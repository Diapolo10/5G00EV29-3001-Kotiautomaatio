
#include <WiFi.h>

static double adc_value{};  // ohm
static const int adc_resistance{10000};  // ohm
static const int resistor{};  // ohm

double temperature_resistance(double adc) {
  return (adc_resistance + resistor) / (1023/adc - 1);
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println("Setup done");
}

void loop() {

}
