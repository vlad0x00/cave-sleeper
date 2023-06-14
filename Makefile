all:
	arduino-cli compile -b arduino:avr:uno .

format:
	clang-format -i --style=Mozilla *.ino *.hpp *.cpp
