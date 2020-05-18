#include <Connection.h>

void Connection::connectToWiFi()
{
    WiFi.mode(WIFI_STA);
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

    client.setServer(mqtt_server, 1883);
}

void Connection::connectToMqtt()
{
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

void Connection::loop()
{
    client.loop();
}

void Connection::mqtt(const char *topic, const char *payload)
{
    Serial.println("MQTT Start Publish.");
    client.publish(topic, payload);
    Serial.println("MQTT End Publish.");
}

bool Connection::connected()
{
    return client.connected();
}