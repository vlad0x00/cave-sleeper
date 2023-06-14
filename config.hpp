#ifndef CAVE_SLEEPER_CONFIG_HPP
#define CAVE_SLEEPER_CONFIG_HPP

namespace cvslpr {

// Set to true to enable RTC time initialization only. Compile code with
// this enabled, upload it to the microncontroller to initialize
// the RTC and then recompile with this disabled to upload the regular
// functionality code.
constexpr inline bool INIT_RTC_TIME = false;

// Set to true to both initialize RTC time and run the regular functionality
// immediately after. Useful for debugging. Implies INIT_RTC_TIME.
constexpr inline bool INIT_RTC_TIME_AND_RUN = false;

// Offset from UTC for the computer this code is compiled on.
// This is needed in order to determine the current UTC time
// to upload it to the RTC.
constexpr inline int COMPILATION_TIMEZONE = -7;

// Set to true to enable readable, but unoptimized, log format
constexpr inline bool TEXT_LOG_FORMAT = false;

// Set to true to resume the previous log found on the SD card, if any.
// If there isn't one, a new file is created. If disabled,
// the previous log deleted, if present, and a new one is started.
constexpr inline bool RESUME_PREVIOUS_LOG = false;

// Change to true to print debug messages to serial port.
constexpr inline bool PRINT_DEBUG = true;

// Preamble is used to indicate the format of the log
const char TEXT_LOG_PREAMBLE[] = "TextLog_v1\n";
const char TEXT_LOG_HEADER[] = "utc_date,utc_time,temperature\n";
const char BINARY_LOG_PREAMBLE[] = "BinaryLog_v1\n";
const char PREAMBLE_END_TOKEN = '\n';
constexpr inline long SERIAL_BAUD_RATE = 9600;
constexpr inline long BLUETOOTH_SERIAL_BAUD_RATE = 9600;
const char LOG_FILENAME[] = "log";
constexpr inline int RTC_INTERRUPT_PIN = 2;
constexpr inline int BLUETOOTH_INTERRUPT_PIN = 3;
constexpr inline int BLUETOOTH_RX_PIN = 4;
constexpr inline int BLUETOOTH_TX_PIN = 5;
constexpr inline int SD_CS = 6; // SD chip select

constexpr inline int SLEEP_DURATION = 15 * 60; // In seconds

// How long to wait after booting before running any code. Useful in order to
// avoid code execution on boot in case you instead want to upload different
// code.
constexpr inline unsigned STARTUP_DELAY = 5000;

} // namespace cvslpr

#endif