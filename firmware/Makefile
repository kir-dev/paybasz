all:
	@echo "cleanup compile build status serial"

cleanup:
	@echo "[#] Cleanup, status"
	rm output/* -f
	~/arduino/bin/arduino-cli board list

compile: cleanup
	@echo "[#] Compile"
	~/arduino/bin/arduino-cli compile --fqbn esp32:esp32:esp32 src/paybasz.ino -v --output output/

build: compile
	@echo "[#] Uploading"
	~/arduino/bin/arduino-cli upload --fqbn esp32:esp32:esp32 -v --port /dev/ttyUSB0 --input output/
	@echo "[#] UPLOADED"

serial:
	minicom -b 115200

status:
	~/arduino/bin/arduino-cli board list

# Deprecated: The dependencies are not new enough in the global lib repo, install them manually
installib:
	~/arduino/bin/arduino-cli lib install "lcdgfx" # 1.0.3 is not new enough, import from git master if 1.0.4 is not released yet
	~/arduino/bin/arduino-cli lib install "MFRC522" # Use 1.5.1 from here https://github.com/MakerSpaceLeiden/rfid
