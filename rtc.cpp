#include "rtc.hpp"
#include "print.hpp"

#include <Arduino.h>

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

constexpr inline int RTC_INTERRUPT_PIN = 2;

constexpr inline int SLEEP_DURATION = 15 * 60; // In seconds

constexpr inline byte ALRM1_MATCH_EVERY_SEC = 0x0F; // Once a second
constexpr inline byte ALRM1_MATCH_SEC = 0x0E;       // When seconds match
constexpr inline byte ALRM1_MATCH_MIN_SEC =
  0x0C; // When minutes and seconds match
constexpr inline byte ALRM1_MATCH_HR_MIN_SEC =
  0x08; // When hours, minutes, and seconds match
constexpr inline byte ALRM1_MATCH_DT_HR_MIN_SEC =
  0x00; // When date, hours, minutes, and seconds match

constexpr inline byte ALRM2_ONCE_PER_MIN =
  0x07; // Once per minute (00 seconds of every minute)
constexpr inline byte ALRM2_MATCH_MIN = 0x06;    // When minutes match
constexpr inline byte ALRM2_MATCH_HR_MIN = 0x04; // When hours and minutes match

constexpr inline byte ALRM1_SET = ALRM1_MATCH_DT_HR_MIN_SEC;
constexpr inline byte ALRM2_SET = ALRM2_MATCH_MIN;
constexpr inline byte ALARM_BITS = ALRM1_SET;

static DS3231 rtcclock;
static RTClib rtclib;

char*
format_time(const DateTime& dt)
{
  static char time_str[] = "0000-00-00,00:00:00";
  sprintf(time_str,
          "%04d-%02d-%02d,%02d:%02d:%02d",
          dt.year(),
          dt.month(),
          dt.day(),
          dt.hour(),
          dt.minute(),
          dt.second());
  return time_str;
}

void
init_rtc_time()
{
  msg_println(F("Beginning RTC time initialization..."));

  const uint32_t unixtime =
    DateTime(__DATE__, __TIME__).unixtime() - COMPILATION_TIMEZONE * 60 * 60;
  DateTime dt_utc(unixtime);

  rtcclock.setClockMode(false);
  rtcclock.setYear(dt_utc.year() - 2000);
  rtcclock.setMonth(dt_utc.month());
  rtcclock.setDate(dt_utc.day());
  rtcclock.setHour(dt_utc.hour());
  rtcclock.setMinute(dt_utc.minute());
  rtcclock.setSecond(dt_utc.second());

  msg_print(F("RTC time initialized to: "));
  msg_println(format_time(rtclib.now()));
}

static void
on_rtc_wakeup()
{
  msg_println(F("Woken up by RTC."));
}

DateTime
get_current_time()
{
  const auto now = rtclib.now();
  msg_print(F("Current time: "));
  msg_println(format_time(now));
  return now;
}

void
set_alarm_time(const DateTime& now)
{
  const DateTime dt_alarm(now.unixtime() + SLEEP_DURATION);
  rtcclock.checkIfAlarm(1); // Reset the alarm bit
  rtcclock.setA1Time(dt_alarm.day(),
                     dt_alarm.hour(),
                     dt_alarm.minute(),
                     dt_alarm.second(),
                     ALARM_BITS,
                     false,
                     false,
                     false);
  msg_print(F("Alarm set for: "));
  msg_println(format_time(dt_alarm));
}

bool
init_rtc()
{
  pinMode(RTC_INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(
    digitalPinToInterrupt(RTC_INTERRUPT_PIN), on_rtc_wakeup, FALLING);

  const DateTime dt_alarm(
    rtclib.now().unixtime() +
    10000); // Set an alarm that won't expire before being changed
  rtcclock.setA1Time(dt_alarm.day(),
                     dt_alarm.hour(),
                     dt_alarm.minute(),
                     dt_alarm.second(),
                     ALARM_BITS,
                     false,
                     false,
                     false);

  rtcclock.checkIfAlarm(1); // Reset the alarm bit
  rtcclock.turnOnAlarm(1);
  if (rtcclock.checkAlarmEnabled(1)) {
    msg_println(F("RTC alarm enabled."));
    return true;
  }
  msg_println(F("Failed to enable RTC alarm."));
  return false;
}

} // namespace cvslpr
