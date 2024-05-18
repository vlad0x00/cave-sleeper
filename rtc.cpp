#include "rtc.hpp"
#include "config.hpp"
#include "log.hpp"
#include "print.hpp"

#include <Arduino.h>

namespace cvslpr {

static RTC_DS3231 rtc;

volatile bool rtc_interrupt_registered = false;

FormattedTime
format_time(const DateTime& dt)
{
  FormattedTime time;
  sprintf(time.time,
          "%04d-%02d-%02dT%02d:%02d:%02dZ",
          dt.year(),
          dt.month(),
          dt.day(),
          dt.hour(),
          dt.minute(),
          dt.second());
  return time;
}

static void
on_rtc_interrupt()
{
  msg_println(F("RTC interrupt registered."));
  rtc_interrupt_registered = true;
}

bool
init_rtc()
{
  if (!rtc.begin()) {
    msg_println(F("Couldn't find RTC."));
    return false;
  }

  // We're not using 32k pin
  rtc.disable32K();

  // Clear any existing alarms
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);
  rtc.disableAlarm(1);
  rtc.disableAlarm(2);

  // Stop oscillating signals at SQW Pin
  // otherwise setAlarm1 will fail
  rtc.writeSqwPinMode(DS3231_OFF);

  pinMode(RTC_INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(
    digitalPinToInterrupt(RTC_INTERRUPT_PIN), on_rtc_interrupt, FALLING);

  msg_println(F("RTC module initialized."));
  return true;
}

void
init_rtc_time()
{
  msg_println(F("Beginning RTC time initialization..."));

  const uint32_t utc_unixtime =
    DateTime(__DATE__, __TIME__).unixtime() - COMPILATION_TIMEZONE * 60 * 60;
  DateTime utc(utc_unixtime);

  msg_print(F("Initializing RTC time to: "));
  msg_println(format_time(utc).time);

  rtc.adjust(utc);

  msg_print(F("RTC time initialized to: "));
  msg_println(format_time(rtc.now()).time);
}

DateTime
get_current_time()
{
  const auto now = rtc.now();
  msg_print(F("Current time: "));
  msg_println(format_time(now).time);
  return now;
}

bool
set_alarm_time(const DateTime& now)
{
  const DateTime dt_alarm(now.unixtime() + SLEEP_DURATION);
  rtc.clearAlarm(1);
  if (!rtc.setAlarm1(dt_alarm, DS3231_A1_Date)) {
    msg_println(F("Failed to set alarm."));
    return false;
  }
  msg_print(F("Alarm set for: "));
  msg_println(format_time(dt_alarm).time);
  return true;
}

} // namespace cvslpr
