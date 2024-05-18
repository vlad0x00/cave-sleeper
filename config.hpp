#ifndef CAVE_SLEEPER_CONFIG_HPP
#define CAVE_SLEEPER_CONFIG_HPP

namespace cvslpr {

// The RTC should send a falling edge interrupt when the alarm expires
// to the following pin.
constexpr inline int RTC_INTERRUPT_PIN = 2;

// The SD card chip select pin.
constexpr inline int SD_CS_PIN = 4;

// The pin to which a physical switch should be attached to, which
// should send a rising edge when turned on. This will indicate
// to the microcontroller that the bluetooth module should be
// turned on and ready to connect to the user's mobile device.
// The physical switch should also enable power on bluetooth module.
constexpr inline int BLUETOOTH_SWITCH_INTERRUPT_PIN = 3;

// Pin on which bluetooth RX is connected on.
constexpr inline int BLUETOOTH_RX_PIN = 5;

// Pin on which bluetooth TX is connected on.
constexpr inline int BLUETOOTH_TX_PIN = 6;

// The following pin might be named "EN" on the bluetooth module.
constexpr inline int BLUETOOTH_KEY_PIN = 7;

// The pin which indicates whether or not the bluetooth module is
// connected.
constexpr inline int BLUETOOTH_STATE_PIN = 9;

// The microcontroller will set this pin to HIGH when it wants to
// turn on the LED.
constexpr inline int LED_PIN = 8;

constexpr inline long BLUETOOTH_SERIAL_BAUD_RATE_CMD = 38400;
constexpr inline long BLUETOOTH_SERIAL_BAUD_RATE_DATA = 9600;

// Change to true to print debug messages to serial port.
constexpr inline bool PRINT_DEBUG = false;

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
constexpr inline int COMPILATION_TIMEZONE = +2;

constexpr inline int SLEEP_DURATION = 15 * 60; // In seconds

// Set to true to enable RTC time initialization only. Compile code with
// this enabled, upload it to the microncontroller to initialize
// the RTC and then recompile with this disabled to upload the regular
// functionality code.
constexpr inline bool INIT_RTC_TIME = false;

// Set to true to both initialize RTC time and run the regular functionality
// immediately after. Useful for debugging. Implies INIT_RTC_TIME.
constexpr inline bool INIT_RTC_TIME_AND_RUN = false;

} // namespace cvslpr

#endif
