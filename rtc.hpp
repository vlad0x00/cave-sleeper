#ifndef CAVE_SLEEPER_RTC_HPP
#define CAVE_SLEEPER_RTC_HPP

#include <DS3231.h>

namespace cvslpr {

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
