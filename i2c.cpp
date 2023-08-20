#include "i2c.hpp"

#include <Wire.h>

namespace cvslpr {

bool
init_i2c()
{
  Wire.begin();
  return true;
}

} // namespace cvslpr
