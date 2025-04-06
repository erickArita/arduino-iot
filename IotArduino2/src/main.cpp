#include <SoftwareSerial.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

const char *ssid = "ERICKLAP";              // Nombre de la red Wi-Fi
const char *password = "12345678";          // Contraseña Wi-Fi
const char *mqtt_server = "154.38.167.137"; // Dirección del broker MQTT
const int mqtt_port = 1883;                 // Puerto del broker MQTT

WiFiClient espClient;
PubSubClient client(espClient);

SoftwareSerial mySerial(2, 3); // RX, TX

int ledPins[] = {5, 6, 7, 8}; // Pines de los LEDs
const int numLeds = sizeof(ledPins) / sizeof(ledPins[0]);

// Funciones de declaración
void reconnect();
void mqttCallback(char *topic, byte *payload, unsigned int length);

void processMessage(String message);
void logToMqtt(String message);

void setup()
{
  Serial.begin(9600);
  mySerial.begin(9600);

  // Configuración Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Conectando a Wi-Fi...");
  }
  Serial.println("Conectado a Wi-Fi");

  // // Configuración del cliente MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqttCallback);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  while (mySerial.available())
  {
    String input = mySerial.readString();
    mySerial.println("Mensaje recibido: " + input);
    processMessage(input);
  }

  while (Serial.available())
  {
    String input = Serial.readString();
    Serial.println("Mensaje recibido: serial " + input);
    processMessage(input);
  }
}

void reconnect()
{
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Conectando a Wi-Fi...");
  }
  while (!client.connected())
  {
    Serial.print("Intentando conectar a MQTT...");
    if (client.connect("ArduinoClient", "ReWoogPyowieaqFP", "9PF3uRvqzd5ThH4zruWuYlAQ3SNhgXca"))
    {
      Serial.println("Conectado al broker MQTT");
      client.subscribe("arduino/control");
      client.publish("arduino/status", "Conectado al broker MQTT");
    }
    else
    {
      Serial.print("Fallo en conexión MQTT, estado: ");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  String message;
  for (unsigned int i = 0; i < length; i++)
  {
    message += (char)payload[i];
  }

  Serial.println(message);

  processMessage(message);
}

void logToMqtt(String message)
{
  // Publicar el mensaje en el topic "arduino/log"
  client.publish("arduino/log", message.c_str());
}

void processMessage(String message)
{
  Serial.println("esp-" + message);
  logToMqtt("Mensaje recibido: " + message);

  // Si el mensaje viene del broker MQTT (hacia Arduino)
  if (message.startsWith("{"))
  {
    // Enviar el mensaje al Arduino a través de la conexión serial
    mySerial.println(message);
    logToMqtt("Mensaje enviado al Arduino: " + message);
    return;
  }

  // Si el mensaje viene del Arduino con formato "topic:json"
  int separatorIndex = message.indexOf(":");
  if (separatorIndex > 0)
  {
    String topic = message.substring(0, separatorIndex);
    String payload = message.substring(separatorIndex + 1);

    mySerial.println("esp-" + topic + ":" + payload);
    
    // Verificar si el payload es válido
    if (payload.startsWith("{") && payload.endsWith("}"))
    {
      // Publicar directamente en MQTT con el topic especificado
      client.publish(topic.c_str(), payload.c_str());

      logToMqtt("Mensaje enviado a MQTT: " + topic + ":" + payload);
    }
    else
    {
      logToMqtt("Formato de payload inválido: " + payload);
    }
  }
  else
  {
    logToMqtt("Formato de mensaje inválido: " + message);
  }
}
