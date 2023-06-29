import cv2
import numpy as np

def ppm_to_exr(input_path, output_path):
    # Read the PPM image using OpenCV
    input_img = cv2.imread(input_path, cv2.IMREAD_ANYCOLOR | cv2.IMREAD_ANYDEPTH)

    # Convert the image to OpenEXR format
    output_img = np.float32(input_img) / 255.0  # Scale to 0-1 range
    output_img = cv2.cvtColor(output_img, cv2.COLOR_BGR2RGB)  # Convert BGR to RGB
    cv2.imwrite(output_path, output_img)

ppm_file_path = "C:\\Users\\sleim\\Desktop\\MyImage.ppm"
exr_file_path = "C:\\Users\\sleim\\Desktop\\EXR.exr"

ppm_to_exr(ppm_file_path, exr_file_path)
