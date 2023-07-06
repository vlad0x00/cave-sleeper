#ifndef CAVE_SLEEPER_LOG_HPP
#define CAVE_SLEEPER_LOG_HPP

#include "rtc.hpp"
#include "sensors.hpp"

namespace cvslpr {

// Preamble is used to indicate the format of the log
const char TEXT_LOG_PREAMBLE[] = "TextLog_v1\n";
const char TEXT_LOG_HEADER[] = "utc_date,utc_time,temperature\n";
const char BINARY_LOG_PREAMBLE[] = "BinaryLog_v1\n";
const char PREAMBLE_END_TOKEN = '\n';

const char LOG_FILENAME[] = "log";

[[nodiscard]] bool
init_log();

[[nodiscard]] bool
deinit_log();

void
log(const SensorsReadout& readout, const DateTime& now);

} // namespace cvslpr

#endif
