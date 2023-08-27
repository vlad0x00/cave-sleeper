#ifndef CAVE_SLEEPER_RTC_HPP
#define CAVE_SLEEPER_RTC_HPP

#include <DS3231.h>

namespace cvslpr {

extern volatile bool rtc_wakeup;

struct FormattedTime
{
  char time[sizeof("0000-00-00T00:00:00Z")];
};

FormattedTime
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
