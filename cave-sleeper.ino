#include "bluetooth.hpp"
#include "rtc.hpp"
#include "sensors.hpp"
#include "log.hpp"
#include "config.hpp"
#include "utils.hpp"

#include <SPI.h>
#include <Wire.h>
#include <avr/sleep.h>
#include <string.h>

namespace cvslpr {

constexpr inline uint32_t ENDIANNESS_SIGNATURE =
  (uint32_t(78) << 24) | (uint32_t(185) << 16) | (uint32_t(219) << 8) |
  (uint32_t(110));
constexpr inline uint32_t INT_SIZE = sizeof(int);
constexpr inline uint32_t DOUBLE_SIZE = sizeof(double);

static inline void
go_sleep()
{
  msg_println(F("Sleeping..."));
  sleep_mode();
}

static inline bool
init_sd()
{
  if (SD.begin(SD_CS)) {
    msg_println(F("SD card initialized."));
    return true;
  }
  msg_println(F("Failed to initialize SD card."));
  return false;
}

static inline bool
init_log()
{
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

// These are unnecessary, as at no point will the microcontroller be notified to
// shut down

// static inline bool deinit_log() {
//   logfile.close();
//   msg_println(F("Log deinitialized."));
//   return true;
// }

// static inline bool deinit_rtc() {
//   detachInterrupt(digitalPinToInterrupt(RTC_INTERRUPT_PIN));
//   msg_println(F("RTC deinitialized"));
//   return true;
// }

static inline void
measure(const DateTime& now)
{
  // Replace the following lines with actual sensor readouts
  const double temp = tempsensor.readTempC();
  // const double hum = 0.5;

  msg_println(F("Measurements taken."));
  msg_print(F("Temperature = "));
  msg_print(temp);
  msg_println("C");
  // msg_print(F("Humidity = "));
  // msg_println(hum);

  if constexpr (TEXT_LOG_FORMAT) {
    logfile.print(format_time(now));
    logfile.print(',');
    logfile.print(temp);
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

static inline void
normal_setup()
{
  if (!(init_sd() && init_log() && init_rtc() && init_sensors() && init_bluetooth())) {
    msg_println(F("Initialization failed."));
    go_sleep();
  }
  msg_println(F("Initialization successful."));
}

static inline void
normal_loop()
{
  const auto now = get_current_time();
  measure(now);
  set_alarm_time(now);
  go_sleep();
  if (bluetooth_wakeup) {
    bluetooth_transfer_data();
    bluetooth_wakeup = false;
  }
}

void setup() {
  delay(STARTUP_DELAY);

  Serial.begin(SERIAL_BAUD_RATE);
  Wire.begin();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();

  if constexpr (INIT_RTC_TIME || INIT_RTC_TIME_AND_RUN) {
    init_rtc_time();
  }

  if constexpr (!INIT_RTC_TIME || INIT_RTC_TIME_AND_RUN) {
    normal_setup();
  } else {
    go_sleep();
  }
}

void loop() {
  if constexpr (!INIT_RTC_TIME || INIT_RTC_TIME_AND_RUN) {
    normal_loop();
  }
}

} // namespace cvslpr

void
setup()
{
  cvslpr::setup();
}

void
loop()
{
  cvslpr::loop();
}
