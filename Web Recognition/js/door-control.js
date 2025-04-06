// Funcionalidad para control de puerta
document.addEventListener('DOMContentLoaded', () => {
    const openDoorBtn = document.getElementById('open-door');
    const closeDoorBtn = document.getElementById('close-door');
    const doorStatus = document.getElementById('door-status');
    let isDoorOpen = false;
    
    openDoorBtn.addEventListener('click', () => {
        if (!isDoorOpen) {
            fetch('https://n8n-prod.solarium.dev/webhook/abrirpuerta', { method: 'GET' })
                .then(response => response.json())
                .then(data => {
                    if (data.success) {
                        console.log('Puerta abierta exitosamente');
                    } else {
                        console.error('Error al abrir la puerta:', data.error);
                    }
                })
                .catch(error => console.error('Error en la solicitud:', error));
            isDoorOpen = true;
            doorStatus.textContent = 'Estado: Puerta Abierta';
            doorStatus.className = 'door-status status-open';
            
            // Deshabilitar el botón de abrir y habilitar el de cerrar
            openDoorBtn.disabled = true;
            closeDoorBtn.disabled = false;
        }
    });
    
    closeDoorBtn.addEventListener('click', () => {
        if (isDoorOpen) {
            fetch('https://n8n-prod.solarium.dev/webhook/cerrarpuerta', { method: 'GET' })
                .then(response => response.json())
                .then(data => {
                    if (data.success) {
                        console.log('Puerta cerrada exitosamente');
                    } else {
                        console.error('Error al cerrar la puerta:', data.error);
                    }
                })
                .catch(error => console.error('Error en la solicitud:', error));

            isDoorOpen = false;
            doorStatus.textContent = 'Estado: Puerta Cerrada';
            doorStatus.className = 'door-status status-closed';
            
            // Deshabilitar el botón de cerrar y habilitar el de abrir
            closeDoorBtn.disabled = true;
            openDoorBtn.disabled = false;
        }
    });
    
    // Inicializar el estado de los botones de la puerta
    closeDoorBtn.disabled = true;
});