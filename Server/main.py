import threading
import paho.mqtt.client as paho
from paho import mqtt
from fastapi import FastAPI
from fastapi.responses import HTMLResponse

# import cv2
# import face_recognition
# import numpy as np
from fastapi.staticfiles import StaticFiles

app = FastAPI()

# Callbacks para diferentes eventos MQTT
def on_connect(client, userdata, flags, rc, properties=None):
    print("CONNACK received with code %s." % rc, client, userdata, flags, properties)

def on_publish(client, userdata, mid, properties=None):
    print("mid: " + str(mid))

def on_subscribe(client, userdata, mid, granted_qos, properties=None):
    print("client", client, "Subscribed: " + str(mid) + " " + str(granted_qos), "UserData", userdata)

def on_message(client, userdata, msg):
    print("on message")
    print("      ", msg.topic + " " + str(msg.qos) + " " + str(msg.payload))

# Configuración del cliente MQTT
client = paho.Client(client_id="", userdata=None, protocol=paho.MQTTv5)
client.tls_set(tls_version=mqtt.client.ssl.PROTOCOL_TLS)
client.username_pw_set("hivemq.webclient.1724232086785", "s*.JT9CzpSV,Fv75x3<b")
client.on_connect = on_connect
client.on_publish = on_publish
client.on_subscribe = on_subscribe
client.on_message = on_message


# Conectar al broker MQTT
client.connect("6783779c24aa4559960fad8c072dcc2e.s1.eu.hivemq.cloud", 8883)

# Iniciar el bucle del cliente MQTT en un hilo separado
def start_mqtt():
    # client.subscribe("testTopic", qos=1)
    client.subscribe("testChannel", qos=1)
    client.subscribe("releChannel", qos=1)
    client.subscribe("testTopic", qos=1)
    client.loop_forever()

mqtt_thread = threading.Thread(target=start_mqtt)

# Función para publicar mensajes MQTT
def publish_mqtt_message(topic: str, payload: str):
    client.publish(topic, payload=payload, qos=1)
mqtt_thread.start()

# # Base de datos de rostros conocidos
# known_face_encodings = []
# known_face_names = []

# # Cargar y registrar rostros
# def load_known_faces():
#     # Cargar la imagen de un usuario conocido y codificar su rostro
#     image = face_recognition.load_image_file("Images/Gaby.jpg")
#     face_encoding = face_recognition.face_encodings(image)[0]

#     # Agregar el rostro y el nombre a la base de datos
#     known_face_encodings.append(face_encoding)
#     known_face_names.append("Gaby")

# load_known_faces()

# # Endpoint WebSocket para recibir video en tiempo real
# @app.websocket("/ws/video")
# async def websocket_video(websocket: WebSocket):
#     await websocket.accept()
#     print("WebSocket conectado para recepción de video en tiempo real")

#     while True:
#         try:
#             # Recibir un cuadro de video
#             frame_data = await websocket.receive_bytes()

#             # Convertir el cuadro a una imagen de OpenCV
#             np_arr = np.frombuffer(frame_data, np.uint8)
#             frame = cv2.imdecode(np_arr, cv2.IMREAD_COLOR)

#             # Procesar el cuadro para reconocimiento facial
#             face_locations = face_recognition.face_locations(frame)
#             face_encodings = face_recognition.face_encodings(frame, face_locations)

#             for face_encoding in face_encodings:
#                 matches = face_recognition.compare_faces(known_face_encodings, face_encoding)
#                 name = "Desconocido"

#                 # Si se encuentra una coincidencia con un rostro conocido
#                 if True in matches:
#                     first_match_index = matches.index(True)
#                     name = known_face_names[first_match_index]

#                 # Enviar resultado a través de MQTT
#                 publish_mqtt_message("user/recognized", f"Usuario {name} reconocido")

#                 # Dibujar un rectángulo alrededor del rostro reconocido
#                 for (top, right, bottom, left) in face_locations:
#                     cv2.rectangle(frame, (left, top), (right, bottom), (0, 255, 0), 2)
#                     cv2.putText(frame, name, (left + 6, bottom - 6), cv2.FONT_HERSHEY_DUPLEX, 0.5, (255, 255, 255), 1)

#             # Mostrar el cuadro procesado (opcional, puede omitirse en producción)
#             cv2.imshow("Video en tiempo real", frame)
#             cv2.waitKey(1)

#         except Exception as e:
#             print("Error al procesar el video:", e)
#             break

#     await websocket.close()


@app.get("/", response_class=HTMLResponse)
async def read_root():
    # read the html file
    with open("templates/iot.html", "r",encoding="utf-8") as file:
        html = file.read()
    return HTMLResponse(content=html, status_code=200)

@app.get("/{channel}/{message}")
async def test(channel:str,message: str):
    client.publish(channel, payload=message, qos=1)
    return {"channel":channel,"message":message}


    

# Servir archivos estáticos (HTML, CSS, JS)
app.mount("/templates", StaticFiles(directory="templates"), name="static")


if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000)
