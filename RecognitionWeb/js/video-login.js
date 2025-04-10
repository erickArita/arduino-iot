const video = document.getElementById("video");
const startCameraBtn = document.getElementById("start-camera");
const scanStatus = document.getElementById("scan-status");
const retryBtn = document.getElementById("retry-button");
const videoContainer = document.getElementById("video-container");

let stream = null;
let ws = null;
let frameInterval = null;

startCameraBtn.addEventListener("click", async (event) => {
  startCameraBtn.disabled = true;
  event.preventDefault();
  try {
    stream = await navigator.mediaDevices.getUserMedia({
      video: {
        width: { ideal: 1280 },
        height: { ideal: 720 },
        facingMode: "user",
      },
    });
    video.srcObject = stream;
    startCameraBtn.disabled = true;
    scanStatus.style.display = "block";
    scanStatus.textContent = "🔍 Iniciando reconocimiento facial...";
    scanStatus.className = "scan-status";

    startRecognition();
  } catch (err) {
    alert("No se pudo acceder a la cámara. Verifica los permisos.");
  }
});

function startRecognition() {
  ws = new WebSocket("ws://localhost:8000/ws/verify");

  ws.onopen = () => {
    frameInterval = setInterval(sendFrame, 1000); // cada 1 segundo
  };

  ws.onmessage = (event) => {
    const data = JSON.parse(event.data);
    if (data.match) {
      scanStatus.textContent = "✅ Acceso autorizado. Rostro reconocido.";
      scanStatus.classList.add("scan-success");
      startCameraBtn.style.display = "none";
      showSuccessfulImage();
      stopRecognition();
    } else if (data.match === false) {
      scanStatus.textContent =
        "❌ No se pudo reconocer el rostro. Intente nuevamente.";
      scanStatus.classList.add("scan-error");
      showErrorImage();
      showRetryButton();
      stopRecognition();
    } else if (data.error) {
      scanStatus.textContent = "⚠️ Error en el backend.";
      scanStatus.classList.add("scan-error");
      stopRecognition();
    }
  };

  ws.onerror = (err) => {
    scanStatus.textContent = "⚠️ Error de conexión.";
    scanStatus.classList.add("scan-error");
    stopRecognition();
  };
}

function sendFrame() {
  try {
    if (video.videoWidth === 0 || video.videoHeight === 0) {
      return;
    }

    const canvas = document.createElement("canvas");
    const context = canvas.getContext("2d");

    if (!context) {
      return;
    }

    canvas.width = video.videoWidth;
    canvas.height = video.videoHeight;
    context.drawImage(video, 0, 0, canvas.width, canvas.height);

    canvas.toBlob(
      (blob) => {
        if (!blob) {
          return;
        }

        const reader = new FileReader();
        reader.onloadend = () => {
          const base64 = reader.result.split(",")[1];
          if (ws.readyState === WebSocket.OPEN) {
            ws.send(base64);
          }
        };
        reader.readAsDataURL(blob);
      },
      "image/jpeg",
      0.8
    );
  } catch (e) {}
}

function stopRecognition() {
  if (frameInterval) clearInterval(frameInterval);
  if (ws && ws.readyState === WebSocket.OPEN) ws.close();

  if (stream) {
    stream.getTracks().forEach((track) => track.stop());
    video.srcObject = null;
  }

  startCameraBtn.disabled = false;
}

function showRetryButton() {
  startCameraBtn.style.display = "none";
  retryBtn.style.display = "inline-block";
}

function showSuccessfulImage() {
  const img = document.createElement("img");
  img.src = "assets/mono.jpeg";
  img.alt = "Acceso autorizado";
  img.style.width = "100%";
  img.style.height = "auto";

  videoContainer.innerHTML = "";
  videoContainer.appendChild(img);
}

function showErrorImage() {
  const img = document.createElement("img");
  img.src = "assets/gato.jpg";
  img.alt = "Acceso denegado";
  img.style.width = "100%";
  img.style.height = "auto";

  videoContainer.innerHTML = "";
  videoContainer.appendChild(img);
}

retryBtn.addEventListener("click", () => {
  location.reload();
});
