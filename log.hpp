#ifndef CAVE_SLEEPER_LOG_HPP
#define CAVE_SLEEPER_LOG_HPP

#include "rtc.hpp"
#include "sensors.hpp"

#include <SD.h>

#include <stddef.h>
#include <stdint.h>

namespace cvslpr {

struct LogEntry
{
  uint32_t timestamp;
  double temperature;
  double humidity;

  LogEntry() = default;

  LogEntry(const uint32_t timestamp,
           const double temperature,
           const double humidity)
    : timestamp(timestamp)
    , temperature(temperature)
    , humidity(humidity)
  {
  }
} __attribute__((packed));

struct FormattedLogEntry
{
  static const char HEADER[sizeof("datetime,temperature,humidity\n")];
  static const int TEMP_WIDTH;
  static const int HUM_WIDTH;
  static const int PRECISION;

  char data[sizeof("0000-00-00T00:00:00Z,000.00,000.00\n")];
  size_t size{ 0 };

  FormattedLogEntry(const LogEntry& entry);
} __attribute__((packed));

[[nodiscard]] bool
init_log();

[[nodiscard]] bool
deinit_log();

void
log(const SensorsReadout& readout, const DateTime& now);

[[nodiscard]] File
open_log();
[[nodiscard]] bool
load_log_entry(File& logfile, LogEntry& entry);

} // namespace cvslpr

#endif
