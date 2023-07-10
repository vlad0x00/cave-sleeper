all: compile

BOARD=arduino:avr:nano
PORT=/dev/ttyUSB0

compile:
	arduino-cli compile -b $(BOARD)

upload:
	arduino-cli upload -b $(BOARD) -p $(PORT)

monitor:
	arduino-cli monitor -b $(BOARD) -p $(PORT)

format:
	clang-format -i --style=Mozilla *.ino *.hpp *.cpp
