const fileInput = document.getElementById("file-input");
const imageGallery = document.getElementById("image-gallery");
const uploadButton = document.getElementById("upload-button");
const uploadContainer = document.getElementById("upload-container");

fileInput.addEventListener("change", () => {
  const files = fileInput.files;

  if (files.length > 0) {
    imageGallery.innerHTML = "";

    for (let i = 0; i < files.length; i++) {
      const file = files[i];

      if (!file.type.startsWith("image/")) {
        continue;
      }

      const reader = new FileReader();

      reader.onload = (e) => {
        const imageItem = document.createElement("div");
        imageItem.className = "image-item";

        const img = document.createElement("img");
        img.src = e.target.result;
        img.alt = "Uploaded image";

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

    uploadButton.style.display = "inline-block";
  }
});

uploadButton.addEventListener("click", async () => {
  const files = fileInput.files;
  const formData = new FormData();

  for (let i = 0; i < files.length; i++) {
    const file = files[i];
    formData.append("files", file);
  }

  try {
    const response = await fetch("http://localhost:8000/upload-image/", {
      method: "POST",
      body: formData,
    });

    console.log("Response:", await response.json());

    if (response.ok) {
      alert("Imágenes subidas correctamente");
      location.reload();
    }
  } catch (err) {
    console.error("Error in file upload:", err);
  }
});
