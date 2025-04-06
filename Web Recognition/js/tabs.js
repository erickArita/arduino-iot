// Funcionalidad de las pestañas
document.addEventListener("DOMContentLoaded", () => {
  const tabs = document.querySelectorAll(".tab");
  const tabContents = document.querySelectorAll(".tab-content");

  tabs.forEach((tab) => {
    tab.addEventListener("click", () => {
      const tabId = tab.getAttribute("data-tab");

      // Desactivar todas las pestañas y contenidos
      tabs.forEach((t) => t.classList.remove("active"));
      tabContents.forEach((content) => content.classList.remove("active"));

      // Activar la pestaña seleccionada y su contenido
      tab.classList.add("active");
      document.getElementById(tabId).classList.add("active");
    });
  });
});
