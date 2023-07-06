#include "sleep.hpp"
#include "print.hpp"

#include <avr/sleep.h>

namespace cvslpr {

bool
init_sleep()
{
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  return true;
}

void
go_sleep()
{
  msg_println(F("Sleeping..."));
  sleep_mode();
}

} // namespace cvslpr
