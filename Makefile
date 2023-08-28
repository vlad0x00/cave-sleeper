all:
	$(MAKE) compile
	$(MAKE) upload
	$(MAKE) monitor

BOARD=arduino:avr:nano:cpu=atmega328old
PORT=/dev/ttyUSB0

compile:
	arduino-cli compile -b $(BOARD)

upload:
	arduino-cli upload -b $(BOARD) -p $(PORT)

monitor:
	arduino-cli monitor -b $(BOARD) -p $(PORT)

format:
	clang-format -i --style=Mozilla *.ino *.hpp *.cpp

deps:
	arduino-cli lib install SD
	arduino-cli lib install DS3231
	arduino-cli lib install "SparkFun High Precision Temperature Sensor TMP117 Qwiic"
