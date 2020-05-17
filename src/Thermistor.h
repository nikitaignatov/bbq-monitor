#include <Arduino.h>
#include <SimpleKalmanFilter.h>

class Thermistor
{
    uint8_t pin;
    double degrees_celcius = 0.0;
    double kelvin = 273.15;
    double resistor = 20480;

    double probe_a = 0.7615340707E-03;
    double probe_b = 2.092182066E-04;
    double probe_c = 0.7446076678E-07;

    SimpleKalmanFilter simpleKalmanFilter;

public:
    Thermistor(uint8_t _pin);
    double read_temperature();
    double get_temperature();
};