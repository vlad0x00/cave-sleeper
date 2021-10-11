#include <DS3231.h>
#include <Wire.h>
#include <avr/sleep.h>
#include <SPI.h>
#include <SD.h>
#include <string.h>

// ====================== CONFIGURATION SECTION ======================

// Uncomment to enable RTC time initialization only. Compile code with
// this uncommented, upload it to the microncontroller to initialize
// the RTC and then recompile with this line commented out to upload
// the regular functionality code.
//#define INIT_RTC_TIME

// Uncomment to both initialize RTC time and run the regular functionality
// immediately after. Useful for debugging. Implies the definition of
// INIT_RTC_TIME
//#define INIT_RTC_TIME_AND_RUN

// Offset from UTC for the computer this code is compiled on.
// This is needed in order to determine the current UTC time
// to upload it to the RTC.
constexpr int COMPILATION_TIMEZONE = -7;

// Uncomment to enable readable, but unoptimized, log format
//#define TEXT_LOG_FORMAT

// Uncomment to resume the previous log found on the SD card, if any.
// If there isn't one, a new file is created. If commented out,
// the previous log deleted, if present, and a new one is started.
//#define RESUME_PREVIOUS_LOG

// Uncomment to print debug messages to serial port.
#define PRINT_DEBUG

// Preamble is used to indicate the format of the log
const char TEXT_LOG_PREAMBLE[] = "TextLog_v1\n";
const char TEXT_LOG_HEADER[] = "utc_date,utc_time,temperature,humidity\n";
const char BINARY_LOG_PREAMBLE[] = "BinaryLog_v1\n";
constexpr long SERIAL_BAUD_RATE = 9600;
const char LOG_FILENAME[] = "log";
constexpr int RTC_INTERRUPT_PIN = 2;
constexpr int SD_CS = 4; // SD chip select

constexpr int SLEEP_DURATION = 15 * 60; // In seconds

// ====================== END OF CONFIGURATION SECTION ======================

constexpr uint32_t ENDIANNESS_SIGNATURE = (uint32_t(78) << 24) | (uint32_t(185) << 16) | (uint32_t(219) << 8) | (uint32_t(110));
constexpr uint32_t INT_SIZE = sizeof(int);
constexpr uint32_t FLOAT_SIZE = sizeof(float);

constexpr byte ALRM1_MATCH_EVERY_SEC     = 0x0F;  // Once a second
constexpr byte ALRM1_MATCH_SEC           = 0x0E;  // When seconds match
constexpr byte ALRM1_MATCH_MIN_SEC       = 0x0C;  // When minutes and seconds match
constexpr byte ALRM1_MATCH_HR_MIN_SEC    = 0x08;  // When hours, minutes, and seconds match
constexpr byte ALRM1_MATCH_DT_HR_MIN_SEC = 0x00;  // When date, hours, minutes, and seconds match

constexpr byte ALRM2_ONCE_PER_MIN     = 0x07;  // Once per minute (00 seconds of every minute)
constexpr byte ALRM2_MATCH_MIN        = 0x06;  // When minutes match
constexpr byte ALRM2_MATCH_HR_MIN     = 0x04;  // When hours and minutes match

constexpr byte ALRM1_SET = ALRM1_MATCH_DT_HR_MIN_SEC;
constexpr byte ALRM2_SET = ALRM2_MATCH_MIN;
constexpr byte ALARM_BITS = ALRM1_SET;

DS3231 clock;
RTClib rtclib;
File logfile;

template<typename T>
inline void msg_print(T t) {
#ifdef PRINT_DEBUG
  Serial.print(t);
  Serial.flush();
#endif
}

template<typename T>
inline void msg_println(T t) {
#ifdef PRINT_DEBUG
  Serial.println(t);
  Serial.flush();
#endif
}

inline void go_sleep() {
  msg_println(F("Sleeping..."));
  sleep_mode();
}

inline char* format_time(DateTime dt) {
  static char time_str[] = "0000-00-00,00:00:00";
  sprintf(time_str, "%04d-%02d-%02d,%02d:%02d:%02d", dt.year(), dt.month(), dt.day(), dt.hour(), dt.minute(), dt.second());
  return time_str;
}

inline void init_rtc_time() {
  delay(5000);
  msg_println(F("Beginning RTC time initialization..."));
  
  const uint32_t unixtime = DateTime(__DATE__, __TIME__).unixtime() - COMPILATION_TIMEZONE * 60 * 60;
  DateTime dt_utc(unixtime);
  
  clock.setClockMode(false);
  clock.setYear(dt_utc.year() - 2000);
  clock.setMonth(dt_utc.month());
  clock.setDate(dt_utc.day());
  clock.setHour(dt_utc.hour());
  clock.setMinute(dt_utc.minute());
  clock.setSecond(dt_utc.second());

  msg_print(F("RTC time initialized to: "));
  msg_println(format_time(rtclib.now()));
}

inline bool init_sd() {
  if (SD.begin(SD_CS)) {
    msg_println(F("SD card initialized."));
    return true;
  }
  msg_println(F("Failed to initialize SD card."));
  return false;
}

inline bool init_log() {
  bool found_prev = false;
  if (SD.exists(LOG_FILENAME)) {
    found_prev = true;
#ifdef RESUME_PREVIOUS_LOG
    msg_println(F("Previous log found, appending."));
#else
    msg_println(F("Previous log found, removing."));
    SD.remove("log");
#endif
  } else {
    msg_println(F("No previous log found, creating a new one."));
  }

#ifdef RESUME_PREVIOUS_LOG
    if (found_prev) {
      logfile = SD.open(LOG_FILENAME, FILE_READ);
      if (!logfile) {
        msg_println(F("Failed to open previous log file."));
        return false;
      }
      
      // Load the preamble of the previous log
#ifdef TEXT_LOG_FORMAT
      const size_t PREAMBLE_BUFFER_SIZE = sizeof(TEXT_LOG_PREAMBLE);
      const char* EXPECTED_PREAMBLE = TEXT_LOG_PREAMBLE;
#else
      const size_t PREAMBLE_BUFFER_SIZE = sizeof(BINARY_LOG_PREAMBLE);
      const char* EXPECTED_PREAMBLE = BINARY_LOG_PREAMBLE;
#endif
      char preamble[PREAMBLE_BUFFER_SIZE];
      size_t i = 0;
      while (logfile.available()) {
        if (i == PREAMBLE_BUFFER_SIZE) {
          msg_println(F("Preamble too long in previous log."));
          return false;
        }
        char c = logfile.read();
        preamble[i++] = c;
        if (c == '\0') {
          break;
        }
      }
      if (strcmp(EXPECTED_PREAMBLE, preamble)) {
        msg_println(F("Loaded preamble is different from expected."));
        msg_print(F("Expected: "));
        msg_println(EXPECTED_PREAMBLE);
        msg_print(F("Found: "));
        msg_println(preamble);
        return false;
      }
    }
#endif

  logfile = SD.open(LOG_FILENAME, FILE_WRITE);
  if (logfile) {
    msg_println(F("Log file opened for writing."));

#ifdef RESUME_PREVIOUS_LOG
    if (!found_prev) {
#endif

#ifdef TEXT_LOG_FORMAT
      logfile.print(TEXT_LOG_PREAMBLE);
      logfile.print(TEXT_LOG_HEADER);
#else
      logfile.write(BINARY_LOG_PREAMBLE, sizeof(BINARY_LOG_PREAMBLE));
      logfile.write((byte*)(&ENDIANNESS_SIGNATURE), sizeof(ENDIANNESS_SIGNATURE));
      logfile.write((byte*)(&INT_SIZE), sizeof(INT_SIZE));
      logfile.write((byte*)(&FLOAT_SIZE), sizeof(FLOAT_SIZE));
#endif
      logfile.flush();
      
#ifdef RESUME_PREVIOUS_LOG
    }
#endif

    return true;
  }
  msg_println(F("Failed to open log file for writing."));
  return false;
}

void on_wakeup() {
  msg_println(F("Woken up."));
}

inline DateTime get_current_time() {
  const auto now = rtclib.now();
  msg_print(F("Current time: "));
  msg_println(format_time(now));
  return now;
}

inline void set_alarm_time(const DateTime& now) {
  const DateTime dt_alarm(now.unixtime() + SLEEP_DURATION);
  clock.checkIfAlarm(1); // Reset the alarm bit
  clock.setA1Time(dt_alarm.day(), dt_alarm.hour(), dt_alarm.minute(), dt_alarm.second(), ALARM_BITS, false, false, false);
  msg_print(F("Alarm set for: "));
  msg_println(format_time(dt_alarm));
}

inline bool init_rtc() {
  pinMode(RTC_INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RTC_INTERRUPT_PIN), on_wakeup, FALLING);

  const DateTime dt_alarm(rtclib.now().unixtime() + 10000); // Set an alarm that won't expire before being changed
  clock.setA1Time(dt_alarm.day(), dt_alarm.hour(), dt_alarm.minute(), dt_alarm.second(), ALARM_BITS, false, false, false);

  clock.checkIfAlarm(1); // Reset the alarm bit
  clock.turnOnAlarm(1);
  if (clock.checkAlarmEnabled(1)) {
    msg_println(F("RTC alarm enabled."));
    return true;
  }
  msg_println(F("Failed to enable RTC alarm."));
  return false;
}

// These are unnecessary, as at no point will the microcontroller be notified to shut down

//inline bool deinit_log() {
//  logfile.close();
//  msg_println(F("Log deinitialized."));
//  return true;
//}

//inline bool deinit_rtc() {
//  detachInterrupt(digitalPinToInterrupt(RTC_INTERRUPT_PIN));
//  msg_println(F("RTC deinitialized"));
//  return true;
//}

inline void measure(const DateTime& now) {
  // Replace the following lines with actual sensor readouts
  const float temp = 30.0;
  const float hum = 0.5;

  msg_println(F("Measurements taken."));
  msg_print(F("Temperature = "));
  msg_println(temp);
  msg_print(F("Humidity = "));
  msg_println(hum);

#ifdef TEXT_LOG_FORMAT
  logfile.print(format_time(now));
  logfile.print(',');
  logfile.print(temp);
  logfile.print(',');
  logfile.print(hum);
  logfile.print('\n');
#else
  const auto unixtime = now.unixtime();
  logfile.write((byte*)(&unixtime), sizeof(unixtime));
  logfile.write((byte*)(&temp), sizeof(temp));
  logfile.write((byte*)(&hum), sizeof(hum));
#endif

  logfile.flush();

  msg_println(F("Measurements written to the SD card."));
}

inline void normal_setup() {
  if (!(init_sd() && init_log() && init_rtc())) {
    msg_println(F("Initialization failed."));
    go_sleep();
  }
  msg_println(F("Initialization successful."));
}

inline void normal_loop() {
  const auto now = get_current_time();
  measure(now);
  set_alarm_time(now);
  go_sleep();
}

void setup() {    
  Serial.begin(SERIAL_BAUD_RATE);
  Wire.begin();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

#if defined(INIT_RTC_TIME) || defined(INIT_RTC_TIME_AND_RUN)
  init_rtc_time();
#endif

#if !defined(INIT_RTC_TIME) || defined(INIT_RTC_TIME_AND_RUN)
  normal_setup();
#else
  go_sleep();
#endif
}

void loop() {
#if !defined(INIT_RTC_TIME) || defined(INIT_RTC_TIME_AND_RUN)
  normal_loop();
#endif
}
