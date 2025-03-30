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
  // // Esperar a que haya datos disponibles en serial
  // while (!Serial.available())
  // {
  //   mySerial.println(Serial.readStringUntil('\n'));
  //   Serial.println("Esperando datos en Serial...");
  // }

  while (mySerial.available())
  {
    String input = mySerial.readString();

    // antes de convertirlo a json extraemos el mensaje
    int startIndex = input.indexOf("{");
    int endIndex = input.indexOf("}");
    String json = input.substring(startIndex, endIndex + 1);
    Serial.println("json=" + json); // Imprimir el JSON extraído para depuración
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, json);
    if (error.code() != DeserializationError::Ok)
    {
      Serial.println(F("deserializeJson() failed: "));
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
      }
      else if (strcmp(value.c_str(), ledOff.c_str()) == 0)
      {
        digitalWrite(ledPins[i], LOW);
      }
    }
    Serial.println("value=" + value);
  }
  // Si hay datos disponibles en Serial, léelos y envíalos a mySerial
  if (Serial.available())
  {
    char c = Serial.read();

    mySerial.print(c + "/publish");
  }
}
