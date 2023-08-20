#ifndef CAVE_SLEEPER_RTC_HPP
#define CAVE_SLEEPER_RTC_HPP

#include <DS3231.h>

namespace cvslpr {

// Set to true to enable RTC time initialization only. Compile code with
// this enabled, upload it to the microncontroller to initialize
// the RTC and then recompile with this disabled to upload the regular
// functionality code.
constexpr inline bool INIT_RTC_TIME = false;

// Set to true to both initialize RTC time and run the regular functionality
// immediately after. Useful for debugging. Implies INIT_RTC_TIME.
constexpr inline bool INIT_RTC_TIME_AND_RUN = true;

extern volatile bool rtc_wakeup;

char*
format_time(const DateTime& dt);

void
init_rtc_time();

DateTime
get_current_time();

void
set_alarm_time(const DateTime& now);

[[nodiscard]] bool
init_rtc();

} // namespace cvslpr

#endif
