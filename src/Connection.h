#include <Credentials.h>
#include <WiFi.h>
#include <PubSubClient.h>

class Connection
{
    const char *ssid = WIFI_SSID;
    const char *password = WIFI_PASSWORD;
    const char *mqtt_server = MQTT_SERVER_HOST;

    WiFiClient espClient;
    PubSubClient client = PubSubClient(espClient);

public:
    void connectToWiFi();
    void connectToMqtt();
    bool connected();
    void mqtt(const char *topic, const char *payload);
    void loop();
};