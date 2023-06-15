#include "log.hpp"
#include "print.hpp"

#include <SD.h>
#include <SPI.h>
#include <string.h>

#include <cstddef>

namespace cvslpr {

constexpr inline uint32_t ENDIANNESS_SIGNATURE =
  (uint32_t(78) << 24) | (uint32_t(185) << 16) | (uint32_t(219) << 8) |
  (uint32_t(110));
constexpr inline uint32_t INT_SIZE = sizeof(int);
constexpr inline uint32_t DOUBLE_SIZE = sizeof(double);

static File logfile;

static bool
init_sd()
{
  if (SD.begin(SD_CS)) {
    msg_println(F("SD card initialized."));
    return true;
  }
  msg_println(F("Failed to initialize SD card."));
  return false;
}

bool
init_log()
{
  if (!init_sd()) { return false; }

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
      logfile = SD.open(LOG_FILENAME, FILE_READ);
      if (!logfile) {
        msg_println(F("Failed to open previous log file."));
        return false;
      }

      // Load the preamble of the previous log
      constexpr size_t PREAMBLE_BUFFER_SIZE = TEXT_LOG_FORMAT
                                                ? sizeof(TEXT_LOG_PREAMBLE)
                                                : sizeof(BINARY_LOG_PREAMBLE);
      const char* EXPECTED_PREAMBLE =
        TEXT_LOG_FORMAT ? TEXT_LOG_PREAMBLE : BINARY_LOG_PREAMBLE;

      char preamble[PREAMBLE_BUFFER_SIZE];
      size_t i = 0;
      while (logfile.available()) {
        if (i == PREAMBLE_BUFFER_SIZE - 1) {
          msg_println(F("Preamble too long in previous log."));
          logfile.close();
          return false;
        }
        const char c = logfile.read();
        preamble[i++] = c;
        if (c == PREAMBLE_END_TOKEN) {
          break;
        }
      }
      preamble[i++] = '\0';
      logfile.close();
      if (strcmp(EXPECTED_PREAMBLE, preamble)) {
        msg_println(F("Loaded preamble is different from expected."));
        msg_print(F("Expected: "));
        msg_println(EXPECTED_PREAMBLE);
        msg_print(F("Found: "));
        msg_println(preamble);
        return false;
      }
    }
  }

  logfile = SD.open(LOG_FILENAME, FILE_WRITE);
  if (logfile) {
    msg_println(F("Log file opened for writing."));

    if (!RESUME_PREVIOUS_LOG || !found_prev) {
      if constexpr (TEXT_LOG_FORMAT) {
        logfile.print(TEXT_LOG_PREAMBLE);
        logfile.print(TEXT_LOG_HEADER);
      } else {
        logfile.write(BINARY_LOG_PREAMBLE, sizeof(BINARY_LOG_PREAMBLE) - 1);
        logfile.write((byte*)(&ENDIANNESS_SIGNATURE),
                      sizeof(ENDIANNESS_SIGNATURE));
        logfile.write((byte*)(&INT_SIZE), sizeof(INT_SIZE));
        logfile.write((byte*)(&DOUBLE_SIZE), sizeof(DOUBLE_SIZE));
      }
      logfile.flush();
    }

    return true;
  }
  msg_println(F("Failed to open log file for writing."));
  return false;
}

bool deinit_log() {
  logfile.close();
  msg_println(F("Log deinitialized."));
  return true;
}

void log(const SensorsReadout& readout, const DateTime& now) {
  if constexpr (TEXT_LOG_FORMAT) {
    logfile.print(format_time(now));
    logfile.print(',');
    logfile.print(readout.temp);
    // logfile.print(',');
    // logfile.print(hum);
    logfile.print('\n');
  } else {
    const auto unixtime = now.unixtime();
    logfile.write((byte*)(&unixtime), sizeof(unixtime));
    logfile.write((byte*)(&temp), sizeof(temp));
    // logfile.write((byte*)(&hum), sizeof(hum));
  }

  logfile.flush();

  msg_println(F("Measurements written to the SD card."));
}

} // namespace cvslpr
