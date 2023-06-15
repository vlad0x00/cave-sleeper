#ifndef CAVE_SLEEPER_RTC_HPP
#define CAVE_SLEEPER_RTC_HPP

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

DS3231 rtcclock;
RTClib rtclib;

char*
format_time(DateTime dt);

void
init_rtc_time();

void
on_rtc_wakeup();

DateTime
get_current_time();

void
set_alarm_time(const DateTime& now);

[[nodiscard]] bool
init_rtc();

[[nodiscard]] bool deinit_rtc();

} // namespace cvslpr

#endif
