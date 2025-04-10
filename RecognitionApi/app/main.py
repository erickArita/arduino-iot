import base64
import shutil
import uuid
from pathlib import Path

import cv2
import httpx
import numpy as np
from fastapi import (
    FastAPI,
    File,
    HTTPException,
    UploadFile,
    WebSocket,
    WebSocketDisconnect,
)
from fastapi.middleware.cors import CORSMiddleware
from fastapi.responses import JSONResponse

from app.utils import get_reference_images, is_image, verify_with_references

app = FastAPI()

UPLOAD_DIR = Path("reference_faces")
UPLOAD_DIR.mkdir(parents=True, exist_ok=True)

app.add_middleware(
    CORSMiddleware,
    allow_origins=["http://localhost:3000"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)


@app.post("/upload-image/")
async def upload_image(files: list[UploadFile] = File(...)):
    saved_files = []

    for file in files:
        if not is_image(file.filename):
            raise HTTPException(
                status_code=400,
                detail=f"El archivo {file.filename} no es una imagen válida",
            )

        ext = file.filename.split(".")[-1]
        filename = f"{uuid.uuid4()}.{ext}"
        filepath = UPLOAD_DIR / filename

        with open(filepath, "wb") as buffer:
            shutil.copyfileobj(file.file, buffer)

        saved_files.append({"filename": filename, "path": str(filepath)})

    return JSONResponse({"message": "Imágenes guardadas correctamente"})


@app.websocket("/ws/verify")
async def verify_stream(websocket: WebSocket):
    await websocket.accept()
    try:
        reference_images = get_reference_images()
        max_attempts = 5
        attempts = 0

        while True:
            data = await websocket.receive_text()

            try:
                frame_bytes = base64.b64decode(data)
                np_arr = np.frombuffer(frame_bytes, np.uint8)
                frame = cv2.imdecode(np_arr, cv2.IMREAD_COLOR)

                if frame is None:
                    await websocket.send_json({"error": "Frame inválido"})
                    continue

            except Exception:
                await websocket.send_json({"error": "Decodificación fallida"})
                continue

            is_verified = verify_with_references(frame, reference_images)

            if is_verified:
                try:
                    async with httpx.AsyncClient(timeout=5.0) as client:
                        response = await client.get(
                            "https://n8n-prod.solarium.dev/webhook/abrirpuerta"
                        )
                    if response.status_code == 200:
                        print("Verificación exitosa - puerta abierta")
                        await websocket.send_json({"match": True})
                    else:
                        print("Verificación exitosa - puerta cerrada")
                        await websocket.send_json({"match": False})

                except Exception:
                    await websocket.send_json(
                        {"error": "Error al contactar con el servidor externo"}
                    )
                await websocket.close()
                return

            attempts += 1
            print(f"Intentos: {attempts}")
            if attempts >= max_attempts:
                await websocket.send_json({"match": False})
                break

    except WebSocketDisconnect:
        pass
    except Exception as e:
        await websocket.send_json({"error": str(e)})
        await websocket.close()
