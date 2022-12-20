from PIL import Image
import os

def _converter(path_dir: str):
    for _, _, fs in os.walk(path_dir):
        for f in fs:
            if not f.endswith(".jpg"):
                continue
            with Image.open(path_dir + f) as img:
                img.save(path_dir + f, 'JPEG', quality=95, optimize=True)

if __name__ == '__main__':

    path_dir = input("Please the file folder of the files to convert: ")
    _converter(path_dir)
