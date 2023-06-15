#ifndef CAVE_SLEEPER_LOG_HPP
#define CAVE_SLEEPER_LOG_HPP

#include "sensors.hpp"
#include "rtc.hpp"

namespace cvslpr {

constexpr inline int SD_CS = 6; // SD chip select

// Set to true to enable readable, but unoptimized, log format
constexpr inline bool TEXT_LOG_FORMAT = false;

// Set to true to resume the previous log found on the SD card, if any.
// If there isn't one, a new file is created. If disabled,
// the previous log deleted, if present, and a new one is started.
constexpr inline bool RESUME_PREVIOUS_LOG = false;

// Preamble is used to indicate the format of the log
const char TEXT_LOG_PREAMBLE[] = "TextLog_v1\n";
const char TEXT_LOG_HEADER[] = "utc_date,utc_time,temperature\n";
const char BINARY_LOG_PREAMBLE[] = "BinaryLog_v1\n";
const char PREAMBLE_END_TOKEN = '\n';

const char LOG_FILENAME[] = "log";

[[nodiscard]] bool
init_log();

[[nodiscard]] bool deinit_log();

void log(const SensorsReadout& readout, const DateTime& now);

} // namespace cvslpr

#endif
