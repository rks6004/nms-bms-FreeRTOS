#pragma once

#include "FreeRTOS.h"
#include "task.h"

#ifdef ESP_PLATFORM
#undef taskENTER_CRITICAL
#undef taskEXIT_CRITICAL

extern portMUX_TYPE global_spinlock;

#define taskENTER_CRITICAL() (portENTER_CRITICAL(&global_spinlock))
#define taskEXIT_CRITICAL()  (portEXIT_CRITICAL(&global_spinlock))

#define IS_ISR() (xPortInIsrContext())
#endif // ESP_PLATFORM

#define INT64_TO_BYTE8(value, bytes)                                                               \
  do                                                                                               \
  {                                                                                                \
    for (int i = 0; i < 8; i++)                                                                    \
    {                                                                                              \
      (bytes)[i] = ((value) >> (i * 8)) & 0xFF;                                                    \
    }                                                                                              \
  } while (0)

#define BYTE8_TO_INT64(value, bytes)                                                               \
  do                                                                                               \
  {                                                                                                \
    (value) = (((uint64_t)(bytes)[0] << 0) | ((uint64_t)(bytes)[1] << 8) |                         \
               ((uint64_t)(bytes)[2] << 16) | ((uint64_t)(bytes)[3] << 24) |                       \
               ((uint64_t)(bytes)[4] << 32) | ((uint64_t)(bytes)[5] << 40) |                       \
               ((uint64_t)(bytes)[6] << 48) | ((uint64_t)(bytes)[7] << 56));                       \
  } while (0)

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

enum task_priority
{
  PRIORITY_IDLE = tskIDLE_PRIORITY,
  PRIORITY_LOW = tskIDLE_PRIORITY + 1,
  PRIORITY_MEDIUM = tskIDLE_PRIORITY + 2,
  PRIORITY_HIGH = tskIDLE_PRIORITY + 3,
  PRIORITY_REAL_TIME = tskIDLE_PRIORITY + 4
};

/**
 * @brief Get the percentage of a value within a range.
 * 
 * This function calculates the percentage of a given value within a specified minimum and maximum range.
 * If the range is zero (min == max), it returns 0.0f to avoid division by zero.
 * 
 * @param value The value to calculate the percentage for.
 * @param min The minimum value of the range.
 * @param max The maximum value of the range.
 * @return float The percentage of the value within the range, or 0.0f if the range is zero.
 */
float calculate_percent(float value, float min, float max);
