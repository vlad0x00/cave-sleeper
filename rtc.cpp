#include "rtc.hpp"
#include "print.hpp"

#include <Arduino.h>
#include <DS3231.h>

namespace cvslpr {

static DS3231 rtcclock;
static RTClib rtclib;

char*
format_time(DateTime dt)
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

void
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
  attachInterrupt(digitalPinToInterrupt(RTC_INTERRUPT_PIN), on_rtc_wakeup, FALLING);

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

bool deinit_rtc() {
  detachInterrupt(digitalPinToInterrupt(RTC_INTERRUPT_PIN));
  msg_println(F("RTC deinitialized"));
  return true;
}

} // namespace cvslpr
