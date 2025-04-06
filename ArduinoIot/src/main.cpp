#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <Servo.h>

using namespace ArduinoJson;

int ledPins[] = {5, 6, 7, 8};                             // Array para los pines de los LEDs
const int numLeds = sizeof(ledPins) / sizeof(ledPins[0]); // Número de LEDs

// Pines para sensores (ejemplo)
const int sensorTemperatura = A0; // Sensor de temperatura
const int sensorHumedad = A1;     // Sensor de humedad

// Configuración del servo
const int servoPin = 9;     // Pin para el servo
Servo puertaServo;          // Objeto Servo para controlar la puerta
int anguloActual = 0;       // Ángulo actual del servo
bool puertaAbierta = false; // Estado de la puerta (cerrada por defecto)

unsigned long lastSendTime = 0;          // Último tiempo de envío
const unsigned long sendInterval = 5000; // Intervalo de envío (5 segundos)

SoftwareSerial mySerial(2, 3); // RX, TX

// Agregar una variable para el control del parpadeo del pin 8
unsigned long lastBlinkTime = 0; // Último tiempo de parpadeo
const unsigned long blinkInterval = 1000; // Intervalo de parpadeo (1 segundo)
bool led8State = false; // Estado actual del LED en el pin 8

// Función para enviar datos al ESP8266
void enviarDatosAlESP(String topic, JsonDocument &doc)
{
  String jsonString;
  serializeJson(doc, jsonString);

  // Formato: "topic:json"
  String mensaje = topic + ":" + jsonString;
  mySerial.println(mensaje);
  Serial.println("arduino: " + mensaje);
}

// Función para controlar la apertura/cierre de la puerta
void controlPuerta(int angulo)
{
  // Limitar el ángulo entre 0 y 180 grados
  angulo = constrain(angulo, 0, 180);

  // Mover el servo a la posición deseada
  puertaServo.write(angulo);
  anguloActual = angulo;

  // Actualizar el estado de la puerta
  puertaAbierta = (angulo > 15); // Consideramos puerta abierta si ángulo > 15 grados

  // Enviar confirmación de movimiento
  JsonDocument docPuerta;
  docPuerta["angulo"] = anguloActual;
  docPuerta["estado"] = puertaAbierta ? "abierta" : "cerrada";
  docPuerta["accion"] = "movimiento";

  enviarDatosAlESP("arduino/puerta", docPuerta);
}

void setup()
{
  Serial.begin(9600); // Para depuración
  mySerial.begin(9600);

  // Configurar los pines de los LEDs como salida y encenderlos inicialmente
  for (int i = 0; i < numLeds; i++)
  {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], HIGH);
  }

  // Configurar los pines de los sensores
  pinMode(sensorTemperatura, INPUT);
  pinMode(sensorHumedad, INPUT);

  // Configurar el servo
  puertaServo.attach(servoPin);
  puertaServo.write(0); // Iniciar en posición cerrada (0 grados)
  anguloActual = 0;
  puertaAbierta = false;
}

void loop()
{
  // Parpadeo del pin 8
  unsigned long currentTime = millis();
  if (currentTime - lastBlinkTime >= blinkInterval)
  {
    lastBlinkTime = currentTime;
    led8State = !led8State; // Cambiar el estado del LED
    digitalWrite(8, led8State ? HIGH : LOW); // Encender o apagar el LED
  }

  // Recibir datos del ESP
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

    // Control de LEDs
    for (int i = 0; i < numLeds; i++)
    {
      String ledOn = "led" + String(ledPins[i]) + "On";
      String ledOff = "led" + String(ledPins[i]) + "Off";
      if (strcmp(value.c_str(), ledOn.c_str()) == 0)
      {
        digitalWrite(ledPins[i], HIGH);

        // Enviar confirmación al ESP inmediatamente
        JsonDocument docConfirmacion;
        docConfirmacion["pin"] = ledPins[i];
        docConfirmacion["accion"] = "encendido";
        docConfirmacion["estado"] = "completado";

        enviarDatosAlESP("arduino/confirmacion", docConfirmacion);
      }
      else if (strcmp(value.c_str(), ledOff.c_str()) == 0)
      {
        digitalWrite(ledPins[i], LOW);

        // Enviar confirmación al ESP inmediatamente
        JsonDocument docConfirmacion;
        docConfirmacion["pin"] = ledPins[i];
        docConfirmacion["accion"] = "apagado";
        docConfirmacion["estado"] = "completado";

        enviarDatosAlESP("arduino/confirmacion", docConfirmacion);
      }
    }

    // Control de la puerta (servo)
    if (value.startsWith("puerta"))
    {
      // Comandos posibles: "puertaAbrir", "puertaCerrar", "puertaAngulo:X"
      if (value == "puertaAbrir")
      {
        controlPuerta(90); // Abrir la puerta a 90 grados
      }
      else if (value == "puertaCerrar")
      {
        controlPuerta(0); // Cerrar la puerta (0 grados)
      }
      else if (value.startsWith("puertaAngulo:"))
      {
        // Extraer el ángulo del comando (formato: "puertaAngulo:X")
        int angulo = value.substring(13).toInt();
        controlPuerta(angulo);
      }
    }

    Serial.println("value=" + value);
  }
}
