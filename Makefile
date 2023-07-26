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
