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

set-rtc:
	sed -i 's/INIT_RTC_TIME = false/INIT_RTC_TIME = true/' config.hpp
	$(MAKE) compile
	$(MAKE) upload
	sleep 15;
	sed -i 's/INIT_RTC_TIME = true/INIT_RTC_TIME = false/' config.hpp
	$(MAKE) compile
	$(MAKE) upload

format:
	clang-format -i --style=Mozilla *.ino *.hpp *.cpp

deps:
	arduino-cli lib install SD RTClib "SparkFun High Precision Temperature Sensor TMP117 Qwiic"
