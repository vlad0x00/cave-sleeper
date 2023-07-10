all: compile

BOARD=nano

compile:
	arduino-cli compile -b arduino:avr:$(BOARD)

upload:
	arduino-cli upload -b arduino:avr:$(BOARD) -p /dev/ttyACM0

monitor:
	arduino-cli monitor -b arduino:avr:$(BOARD) -p /dev/ttyACM0

format:
	clang-format -i --style=Mozilla *.ino *.hpp *.cpp
