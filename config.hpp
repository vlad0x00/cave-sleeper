#ifndef CAVE_SLEEPER_CONFIG_HPP
#define CAVE_SLEEPER_CONFIG_HPP

namespace cvslpr {

// The RTC should send a falling edge interrupt when the alarm expires
// to the following pin.
constexpr inline int RTC_INTERRUPT_PIN = 2;

// The bluetooth module should send a rising edge interrupt when it
// connects to a device to the following pin. The pin is likely named
// "STATE" on the bluetooth module.
constexpr inline int BLUETOOTH_INTERRUPT_PIN = 3;

// The SD card chip select pin.
constexpr inline int SD_CS_PIN = 4;

constexpr inline int BLUETOOTH_RX_PIN = 5;
constexpr inline int BLUETOOTH_TX_PIN = 6;

// The following pin might be named "EN" on the bluetooth module.
constexpr inline int BLUETOOTH_KEY_PIN = 7;

// If true, a test is run for whether the bluetooth module is
// working properly. If false, the bluetooth module is assumed
// to be working properly. Requires BLUETOOTH_ON_PIN to be used.
constexpr inline bool BLUETOOTH_TEST = false;

// The microcontroller will set this pin to HIGH when it wants to
// power on the bluetooth module and LOW when it wants to power it off.
// This requires some sort of setup (e.g. a transistor) to actually
// power the bluetooth module.
constexpr inline int BLUETOOTH_ON_PIN = 8;

// The microcontroller will set this pin to HIGH when it wants to
// turn on the LED.
constexpr inline int LED_PIN = 9;

constexpr inline long BLUETOOTH_SERIAL_BAUD_RATE_CMD = 38400;
constexpr inline long BLUETOOTH_SERIAL_BAUD_RATE_DATA = 9600;

// Change to true to print debug messages to serial port.
constexpr inline bool PRINT_DEBUG = true;

constexpr inline long SERIAL_BAUD_RATE = 9600;

// How long to wait after booting before running any code. Useful in order to
// avoid code execution on boot in case you instead want to upload different
// code.
constexpr inline unsigned STARTUP_DELAY = 5000;

// Set to true to resume the previous log found on the SD card, if any.
// If there isn't one, a new file is created. If disabled,
// the previous log deleted, if present, and a new one is started.
constexpr inline bool RESUME_PREVIOUS_LOG = false;

// Offset from UTC for the computer this code is compiled on.
// This is needed in order to determine the current UTC time
// to upload it to the RTC.
constexpr inline int COMPILATION_TIMEZONE = -7;

constexpr inline int SLEEP_DURATION = 15 * 60; // In seconds

// Set to true to enable RTC time initialization only. Compile code with
// this enabled, upload it to the microncontroller to initialize
// the RTC and then recompile with this disabled to upload the regular
// functionality code.
constexpr inline bool INIT_RTC_TIME = false;

// Set to true to both initialize RTC time and run the regular functionality
// immediately after. Useful for debugging. Implies INIT_RTC_TIME.
constexpr inline bool INIT_RTC_TIME_AND_RUN = true;

} // namespace cvslpr

#endif
