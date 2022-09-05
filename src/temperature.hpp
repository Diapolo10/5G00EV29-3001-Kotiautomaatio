#pragma once

#include <math.h>

namespace maths {

    static const int   adc_beta{3435};                   // Kelvin
    static const float adc_calibration_temperature{25};  // Celsius
    static const int   adc_resistance{10000};            // Ohm
    static const float adc_resistance_tolerance{0.01};   // Percentage

    static const int resistor{};              // Ohm
    static const float zero_celsius{273.15};  // Kelvin

    double temperature_resistance(double adc) {
        /**
         * @brief Calculates the change in resistance caused by temperature shifts
         * adc is up to 10 bits + 1, or 1024
         */
        return (adc_resistance + resistor) / (1023/adc - 1);
    }

    double celsius_to_kelvin(double temp) {
        /**
         * @brief Converts Celsius to Kelvin
         * 
         */
        return temp + zero_celsius;
    }

    double kelvin_to_celsius(double temp) {
        /**
         * @brief Converts Kelvin to Celsius
         * 
         */
        return temp - zero_celsius;
    }

    double temperature_c(double res) {
        /**
         * @brief Converts resistances to known temperatures
         * 
         * Some known values:
         * 38 C -> 6180 +- 60 ohm
         * 60 C -> 2980 +- 30 ohm
         * 10 C -> 1840 +- 20 ohm
         */

        return kelvin_to_celsius(
            1 / (
                std::log(
                    res/adc_resistance
                ) / adc_beta
                + 1/(
                    zero_celsius
                    + adc_calibration_temperature
                )
            )
        );
    }
}
