// Funcionalidad para cargar imágenes
document.addEventListener("DOMContentLoaded", () => {
  const fileInput = document.getElementById("file-input");
  const imageGallery = document.getElementById("image-gallery");

  fileInput.addEventListener("change", () => {
    const files = fileInput.files;

    if (files.length > 0) {
      for (let i = 0; i < files.length; i++) {
        const file = files[i];

        // Verificar que sea una imagen
        if (!file.type.startsWith("image/")) {
          continue;
        }

        const reader = new FileReader();

        reader.onload = (e) => {
          const imageItem = document.createElement("div");
          imageItem.className = "image-item";

          const img = document.createElement("img");
          img.src = e.target.result;
          img.alt = "Imagen cargada";

          const deleteBtn = document.createElement("button");
          deleteBtn.className = "delete-btn";
          deleteBtn.innerHTML = "×";
          deleteBtn.addEventListener("click", () => {
            imageItem.remove();
          });

          imageItem.appendChild(img);
          imageItem.appendChild(deleteBtn);
          imageGallery.appendChild(imageItem);
        };

        reader.readAsDataURL(file);
      }

      // Limpiar el input para permitir cargar los mismos archivos nuevamente
      fileInput.value = "";
    }
  });
});
