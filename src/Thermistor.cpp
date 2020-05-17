#include <Thermistor.h>
#include <Arduino.h>

Thermistor::Thermistor(uint8_t _pin) : simpleKalmanFilter(2, 2, 0.0005)
{
    pin = _pin;
    adcAttachPin(pin);
}

double Thermistor::read_temperature()
{
    Serial.print("Probe: ");
    Serial.print(pin);
    uint16_t reading = analogRead(pin);

    double estimated_value = (double)simpleKalmanFilter.updateEstimate(reading);

    Serial.print(" -- Reading: ");

    Serial.print(reading);
    Serial.print("; Avg Reading: ");
    Serial.print(estimated_value);

    double resistance = resistor * ((4096.0 / estimated_value) - 1.0);
    Serial.print("; R(Ω): ");
    Serial.print(resistance);

    double log_resistance = log(resistance);
    degrees_celcius = 1.0 / (probe_a + (probe_b * log_resistance) + (probe_c * log_resistance * log_resistance * log_resistance)) - kelvin;

    Serial.print("; T(°C): ");
    Serial.print(degrees_celcius);
    Serial.println(" ");
    return degrees_celcius;
}

double Thermistor::get_temperature()
{
    return degrees_celcius;
}