#include <Arduino.h>
#include <Thermistor.h>
#include <Connection.h>

long transmissions = 0;

long previousMillis;
long transmission_delay_sec = 2;
long transmission_delay = transmission_delay_sec * 1000;

Thermistor probe1(35);
Thermistor probe2(34);
Connection connection{};

void setup()
{
    connection.connectToWiFi();
    Serial.begin(9600);

    analogReadResolution(12);
    analogSetWidth(12);
    analogSetCycles(8);
    analogSetSamples(1);
    analogSetClockDiv(1);
    analogSetAttenuation(ADC_11db);
}

void loop()
{
    if (!connection.connected())
    {
        connection.connectToMqtt();
    }

    probe1.read_temperature();
    probe2.read_temperature();

    if (millis() > previousMillis + transmission_delay)
    {
        previousMillis = millis();

        connection.mqtt("smoker/temperature", String("{ \"message_id\":" + String(transmissions++) + ",   \"probe_0\":" + String(probe1.get_temperature()) + ",\"probe_1\":" + String(probe1.get_temperature()) + "}").c_str());
        
        connection.loop();
    }
}