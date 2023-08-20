#include "sleep.hpp"
#include "config.hpp"

namespace cvslpr {

bool
init_sleep()
{
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  return true;
}

} // namespace cvslpr
