#include "util.h"

#ifdef ESP_PLATFORM
portMUX_TYPE global_spinlock = portMUX_INITIALIZER_UNLOCKED;
#endif // ESP_PLATFORM

float calculate_percent(float value, float min, float max)
{
  if (max - min == 0)
  {
    return 0.0f;
  }
  return ((value - min) / (max - min)) * 100.0f;
}