#include "log.hpp"
#include "config.hpp"
#include "print.hpp"

#include <SD.h>
#include <SPI.h>

#include <stddef.h>
#include <string.h>
#include <tuple>

namespace cvslpr {

// Preamble is used to indicate the format of the log
const char LOG_PREAMBLE[] = "Log_v2";
const char LOG_FILENAME[] = "log";

constexpr inline uint32_t ENDIANNESS_BYTES =
  (uint32_t(78) << 24) | (uint32_t(185) << 16) | (uint32_t(219) << 8) |
  (uint32_t(110));

static_assert(sizeof(int) == sizeof(uint32_t), "int is not 32 bits.");

FormattedLogEntry::FormattedLogEntry(const LogEntry& entry)
{
  sprintf(data,
          "%s,%.2f,%.2f",
          format_time(entry.timestamp).time,
          entry.temperature,
          entry.humidity);
  size = strlen(data);
}

struct LogMeta
{
  char preamble[sizeof(LOG_PREAMBLE)];
  uint32_t endianness_bytes{ ENDIANNESS_BYTES };
  uint32_t int_size{ sizeof(int) };
  uint32_t double_size{ sizeof(int) };

  LogMeta() { memcpy(preamble, LOG_PREAMBLE, sizeof(LOG_PREAMBLE)); }

  bool operator==(const LogMeta& other) const
  {
    return !memcmp(this, &other, sizeof(LogMeta));
  }

  operator bool() const { return int_size != 0; }
} __attribute__((packed)) DEFAULT_META;

static File logfile;

[[nodiscard]] static bool
init_sd()
{
  if (SD.begin(SD_CS_PIN)) {
    msg_println(F("SD card module initialized."));
    return true;
  }
  msg_println(F("Failed to initialize SD card module."));
  return false;
}

[[nodiscard]] static LogMeta
get_meta(File& logfile)
{
  if (logfile.available() < sizeof(LogMeta)) {
    msg_println(F("Log file too short."));
    return {};
  }

  LogMeta meta;
  logfile.readBytes((char*)(&meta), sizeof(LogMeta));
  return meta;
}

File
open_log()
{
  File f = SD.open(LOG_FILENAME, FILE_WRITE);
  f.seek(0UL);

  const auto meta = get_meta(f);
  if (!meta) {
    msg_println(F("Failed to read log meta."));
    return {};
  }

  if (meta != DEFAULT_META) {
    msg_println(F("Loaded meta is different from expected."));
    return {};
  }

  // Seek to the end of the file
  f.seek(f.size());

  return f;
}

bool
init_log()
{
  if (!init_sd()) {
    return false;
  }

  bool found_prev = false;
  if (SD.exists(LOG_FILENAME)) {
    found_prev = true;
    if constexpr (RESUME_PREVIOUS_LOG) {
      msg_println(F("Previous log found, appending."));
    } else {
      msg_println(F("Previous log found, removing."));
      SD.remove("log");
    }
  } else {
    msg_println(F("No previous log found, creating a new one."));
  }

  if constexpr (RESUME_PREVIOUS_LOG) {
    if (found_prev) {
      logfile = open_log();
      if (logfile) {
        return true;
      } else {
        msg_println(F("Failed to open log file for appending."));
        return false;
      }
    }
  }

  logfile = SD.open(LOG_FILENAME, FILE_WRITE);
  if (logfile) {
    msg_println(F("Log file opened for writing."));

    logfile.write((byte*)(&DEFAULT_META), sizeof(DEFAULT_META));
    logfile.flush();

    return true;
  }
  msg_println(F("Failed to open log file for writing."));
  return false;
}

bool
deinit_log()
{
  logfile.close();
  msg_println(F("Log deinitialized."));
  return true;
}

void
log(const SensorsReadout& readout, const DateTime& now)
{
  const int unixtime = now.unixtime();

  const LogEntry entry(unixtime, readout.temp, readout.hum);

  logfile.write((byte*)(&entry), sizeof(LogEntry));
  logfile.flush();

  msg_println(F("Measurements written to the SD card."));
}

bool
load_log_entry(File& logfile, LogEntry& entry)
{
  if (logfile.available() < sizeof(LogEntry)) {
    return false;
  }

  logfile.readBytes((char*)(&entry), sizeof(LogEntry));
  return true;
}

} // namespace cvslpr
