#include <Arduino.h>
#include <Credentials.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Thermistor.h>

#define PROBE_COUNTS 2

long transmissions = 0;

long previousMillis;
long transmission_delay_sec = 2;
long transmission_delay = transmission_delay_sec * 1000;

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;
const char *mqtt_server = MQTT_SERVER_HOST;

Thermistor probe1(35);
Thermistor probe2(34);

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi()
{
    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void reconnect()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.print("Connecting to ");
        Serial.println(ssid);
    }

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        if (client.connect("ESP-32-SMOKER-V1"))
        {
            Serial.println("connected");
            client.subscribe("smoker/command/#");
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(1000);
        }
    }
}

void setup()
{
    WiFi.mode(WIFI_STA);
    Serial.begin(9600);

    analogReadResolution(12);
    analogSetWidth(12);
    analogSetCycles(8);
    analogSetSamples(1);
    analogSetClockDiv(1);
    analogSetAttenuation(ADC_11db);

    setup_wifi();
    client.setServer(mqtt_server, 1883);
}

void loop()
{
    if (!client.connected())
    {
        reconnect();
    }

    probe1.read_temperature();
    probe2.read_temperature();

    if (millis() > previousMillis + transmission_delay)
    {
        previousMillis = millis();

        Serial.println("MQTT Start Publish.");
        client.publish("smoker/temperature", String("{ \"message_id\":" + String(transmissions++) + ",   \"probe_0\":" + String(probe1.get_temperature()) + ",\"probe_1\":" + String(probe1.get_temperature()) + "}").c_str());
        Serial.println("MQTT End Publish.");

        client.loop();
    }
}