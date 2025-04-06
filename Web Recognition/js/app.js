// Archivo principal para inicialización y funcionalidad global
document.addEventListener('DOMContentLoaded', () => {
    console.log('Sistema de Control de Acceso inicializado correctamente');
    
    // Aquí puedes agregar cualquier funcionalidad global o inicialización adicional
    
    // Ejemplo: Detectar si la aplicación está en modo móvil
    const isMobile = window.innerWidth <= 768;
    if (isMobile) {
        console.log('Ejecutando en dispositivo móvil');
        // Ajustes específicos para móviles si es necesario
    }
});