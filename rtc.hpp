#ifndef CAVE_SLEEPER_RTC_HPP
#define CAVE_SLEEPER_RTC_HPP

#include <RTClib.h>

namespace cvslpr {

extern volatile bool rtc_interrupt_registered;

struct FormattedTime
{
  char time[sizeof("0000-00-00T00:00:00Z")];
};

FormattedTime
format_time(const DateTime& dt);

[[nodiscard]] bool
init_rtc();

void
init_rtc_time();

[[nodiscard]] DateTime
get_current_time();

[[nodiscard]] bool
set_alarm_time(const DateTime& now);

} // namespace cvslpr

#endif
