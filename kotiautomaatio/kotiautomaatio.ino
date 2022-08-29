static double adc_value{};
static const int resistor{10_000};  // ohm

double temperature_resistance(double adc) {
  return resistor / (1023/adc - 1);
}


void setup() {

}

void loop() {

}
