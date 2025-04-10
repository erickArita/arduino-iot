from pathlib import Path
from deepface import DeepFace
import cv2
import os


REFERENCE_DIR = Path("reference_faces")


def is_image(filename: str) -> bool:
    allowed_extensions = ["jpg", "jpeg", "png"]
    return filename.lower().split(".")[-1] in allowed_extensions


def get_reference_images():
    return (
        list(REFERENCE_DIR.glob("*.jpg"))
        + list(REFERENCE_DIR.glob("*.jpeg"))
        + list(REFERENCE_DIR.glob("*.png"))
    )


def verify_with_references(frame, reference_paths) -> bool:
    frame_path = "temp_frame.jpg"
    cv2.imwrite(frame_path, frame)

    try:
        for ref_path in reference_paths:
            try:
                result = DeepFace.verify(
                    img1_path=frame_path,
                    img2_path=str(ref_path),
                    enforce_detection=False,
                )
                if result.get("verified"):
                    return True
            except Exception as e:
                print(f"Error comparing with {ref_path.name}: {e}")
                continue
    finally:
        if os.path.exists(frame_path):
            os.remove(frame_path)

    return False
