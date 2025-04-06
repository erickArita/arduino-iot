// Funcionalidad de la cámara para el login con video
document.addEventListener('DOMContentLoaded', () => {
    const video = document.getElementById('video');
    const startCameraBtn = document.getElementById('start-camera');
    const scanFaceBtn = document.getElementById('scan-face');
    const scanStatus = document.getElementById('scan-status');
    let stream = null;
    
    startCameraBtn.addEventListener('click', async () => {
        try {
            stream = await navigator.mediaDevices.getUserMedia({ 
                video: { 
                    width: { ideal: 1280 },
                    height: { ideal: 720 },
                    facingMode: 'user'
                } 
            });
            video.srcObject = stream;
            startCameraBtn.disabled = true;
            scanFaceBtn.disabled = false;
        } catch (err) {
            console.error('Error al acceder a la cámara:', err);
            alert('No se pudo acceder a la cámara. Por favor, verifica los permisos.');
        }
    });
    
    scanFaceBtn.addEventListener('click', () => {
        // Simulación de escaneo facial
        scanStatus.style.display = 'block';
        scanStatus.textContent = 'Escaneando rostro...';
        scanStatus.className = 'scan-status';
        
        // Simulamos un proceso de escaneo con un temporizador
        setTimeout(() => {
            // Simulación de resultado aleatorio (éxito/error)
            const success = Math.random() > 0.3; // 70% de probabilidad de éxito
            
            if (success) {
                scanStatus.textContent = '✅ Rostro reconocido correctamente';
                scanStatus.classList.add('scan-success');
            } else {
                scanStatus.textContent = '❌ No se pudo reconocer el rostro. Intente nuevamente.';
                scanStatus.classList.add('scan-error');
            }
        }, 2000);
    });
});