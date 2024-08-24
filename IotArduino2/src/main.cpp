// código par la ESP8266
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <time.h>
#include <TZ.h>

const int ledPin = 2;             // Pin del LED (GPIO2 en ESP8266)
unsigned long previousMillis = 0; // Almacena el último tiempo en que el LED cambió de estado
const long interval = 1000;       // Intervalo de tiempo en milisegundos (1 segundo)

const char *ssid2 = "poco";
// si se necesita contraseña
// const char *password2 = "Hola1234";
// Credenciales proporcionada por un broker(Servidor encargado de manejar los sockets)
const char *mqtt_server = "cambiame.hivemq.cloud";
const char *clientId = "";
const char *username = "ardui";
const char *mqtt_password = "";

WiFiClientSecure espClient;
PubSubClient client;

void setup_wifi()
{
  delay(10);
  WiFi.begin(ssid2);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Conectando a WiFi...");
  }

  Serial.println("Conectado a WiFi");
}

void callback(char *topic, byte *payload, unsigned int length)
{
  String message;

  for (unsigned int i = 0; i < length; i++)
  {
    message += (char)payload[i];
  }

  Serial.println("Mensaje recibido desde MQTT: " + message);

  // Enviar el mensaje recibido desde MQTT al Arduino
  Serial.println(message); // El Arduino debería estar escuchando en este mismo puerto
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Conectando a MQTT...");
    if (client.connect("ESP8266Client-", username, mqtt_password))
    {
      Serial.println("Conectado");
      client.subscribe("testTopic");                     // Suscripción al tema MQTT
      client.subscribe("testChannel");                   // Suscripción al tema MQTT
      client.publish("testTopic", "Hola desde ESP8266"); // Publicación en el tema MQTT
    }
    else
    {
      Serial.print("Falló, rc=");
      Serial.print(client.state());
      Serial.println(" Intentando de nuevo en 5 segundos");
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(9600);
  setup_wifi();
  BearSSL::WiFiClientSecure *bear = new BearSSL::WiFiClientSecure();

  bear->setInsecure();

  client.setClient(*bear);
  client.setServer(mqtt_server, 8883);

  client.setCallback(callback);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  if (Serial.available())
  {
    String data = Serial.readStringUntil('\n'); // Cambiar a readStringUntil('\n')
    if (data.indexOf("publish") == 0)
    {

      client.publish("testChannel", data.c_str()); // Publicación en el tema MQTT
    }
  }
}
