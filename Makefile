all: compile

compile:
	arduino-cli compile -b arduino:avr:uno

upload:
	arduino-cli upload -b arduino:avr:uno -p /dev/ttyACM0

monitor:
	arduino-cli monitor -b arduino:avr:uno -p /dev/ttyACM0

format:
	clang-format -i --style=Mozilla *.ino *.hpp *.cpp

lint:
	clang-tidy *.ino *.hpp *.cpp -- -std=gnu++11
