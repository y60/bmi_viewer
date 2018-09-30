import glob
import zipfile
for file in glob.glob("*.zip"):
    with zipfile.ZipFile(file) as existing_zip:
        end=len(file)-4
        existing_zip.extractall(file[0:end])