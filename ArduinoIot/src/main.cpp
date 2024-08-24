#include <ArduinoJson.h>
#include <SoftwareSerial.h>

using namespace ArduinoJson;

int ledPins[] = {5, 6, 7, 8};                             // Array para los pines de los LEDs
const int numLeds = sizeof(ledPins) / sizeof(ledPins[0]); // Número de LEDs

int estadoAnteriorRele = -1; // Estado inicial desconocido

SoftwareSerial mySerial(2, 3); // RX, TX

void setup()
{
  Serial.begin(9600); // Para depuración
  mySerial.begin(9600);

  // Configurar los pines de los LEDs como salida y apagarlos inicialmente
  for (int i = 0; i < numLeds; i++)
  {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], HIGH);
  }
}

void loop()
{
  while (mySerial.available())
  {
    String input = mySerial.readStringUntil('\n');
    Serial.print("Received input: ");
    Serial.println(input); // tis is the message= Received input: Mensaje recibido desde MQTT: {"value":"ledOff"}

    // antes de convertirlo a json extraemos el mensaje
    int startIndex = input.indexOf("{");
    int endIndex = input.indexOf("}");
    String json = input.substring(startIndex, endIndex + 1);
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, json);
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    // Extraer el valor del campo "value"
    String value = doc["value"];

    if (value.length() == 0)
    {
      Serial.println("Error: Input is empty");
      return;
    }

    for (int i = 0; i < numLeds; i++)
    {

      String ledOn = "led" + String(ledPins[i]) + "On";
      String ledOff = "led" + String(ledPins[i]) + "Off";

      if (strcmp(value.c_str(), ledOn.c_str()) == 0)
      {
        digitalWrite(ledPins[i], HIGH);
        Serial.println(value);
      }
      else if (strcmp(value.c_str(), ledOff.c_str()) == 0)
      {
        digitalWrite(ledPins[i], LOW);
        Serial.println(input);
      }
      Serial.println(value);
      Serial.println("geenrates" + ledOn);
      Serial.println("geenrates" + ledOff);
    }
  }
  // Si hay datos disponibles en Serial, léelos y envíalos a mySerial
  if (Serial.available())
  {
    char c = Serial.read();

    mySerial.print(c + "/publish");
  }
}
