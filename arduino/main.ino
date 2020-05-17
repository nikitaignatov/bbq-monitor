#include <Credentials.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <SimpleKalmanFilter.h>

SimpleKalmanFilter simpleKalmanFilter1(2, 2, 0.0005);
SimpleKalmanFilter simpleKalmanFilter2(2, 2, 0.0005);

#define       THERMISTOR_COUNTS 100
#define       PROBE_COUNTS      2

int           probe1 = 35;
int           probe2 = 34;

double        kelvin = 273.15;
double        resistor = 20480;

double        probe_a = 0.7615340707E-03;
double        probe_b = 2.092182066E-04;
double        probe_c = 0.7446076678E-07;

double        degrees_celcius[PROBE_COUNTS] = {0.0, 0.0};

double        average[PROBE_COUNTS] = {0.0, 0.0};
int           all_readings[PROBE_COUNTS][THERMISTOR_COUNTS];
int           reading_index = 0;
long          transmissions = 0;

long          previousMillis;
long          transmission_delay_sec = 2;
long          transmission_delay = transmission_delay_sec * 1000;


const char*   ssid =        WIFI_SSID;
const char*   password =    WIFI_PASSWORD;
const char*   mqtt_server = MQTT_SERVER_HOST;

WiFiClient    espClient;
PubSubClient  client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.println(ssid);
  }

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP-32-SMOKER-V1")) {
      Serial.println("connected");
      client.subscribe("smoker/command/#");
    } else {
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
  adcAttachPin(probe1);
  adcAttachPin(probe2);

  for (int i = 0; i < THERMISTOR_COUNTS; i ++)
  {
    all_readings[0][i] = analogRead(probe1);
    all_readings[1][i] = analogRead(probe2);
    delay(20);
  }

  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void  loop()
{
  if (!client.connected()) {
    reconnect();
  }

  reading_index++;
  ReadTemperature(0, probe1);
  ReadTemperature(1, probe2);

  if (millis() > previousMillis + transmission_delay) {
    previousMillis = millis();

    Serial.println("MQTT Start Publish.");
    client.publish("smoker/temperature", String("{ \"message_id\":" + String(transmissions++) + ",   \"probe_0\":" + String(degrees_celcius[0]) + ",\"probe_1\":" + String(degrees_celcius[1]) + "}").c_str());
    Serial.println("MQTT End Publish.");

    client.loop();
  }
}

void  ReadTemperature(int probe, int pin)
{
  Serial.print("Probe: ");
  Serial.print(pin);
  uint16_t reading = analogRead(pin);

  double estimated_value = (double)(probe == 0 ? simpleKalmanFilter1 : simpleKalmanFilter2).updateEstimate(reading);

  reading_index %= THERMISTOR_COUNTS;
  Serial.print(" -- Reading: ");

  Serial.print(reading);
  Serial.print("; Avg Reading: ");
  Serial.print(estimated_value);

  double resistance = resistor * ((4096.0 / estimated_value) - 1.0);
  Serial.print("; R(Ω): ");
  Serial.print(resistance);

  double log_resistance = log(resistance);
  degrees_celcius[probe] = 1.0 / (probe_a + (probe_b * log_resistance) + (probe_c * log_resistance * log_resistance * log_resistance)) - kelvin;

  Serial.print("; T(°C): ");
  Serial.print(degrees_celcius[probe]);
  Serial.println(" ");
}
