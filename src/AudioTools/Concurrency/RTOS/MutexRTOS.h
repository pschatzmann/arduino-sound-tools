#pragma once
#include "AudioConfig.h"
#include "AudioTools/Concurrency/Mutex.h"

#ifdef ESP32
#  include "freertos/FreeRTOS.h"
#  include "freertos/semphr.h"
#else
#  include "FreeRTOS.h"
#  include "semphr.h"
#endif

namespace audio_tools {

/**
 * @brief Mutex implemntation using FreeRTOS
 * @ingroup concurrency
 * @author Phil Schatzmann
 * @copyright GPLv3 *
 */
class MutexRTOS : public MutexBase {
public:
  MutexRTOS() {
    TRACED();
    xSemaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(xSemaphore);
  }
  virtual ~MutexRTOS() {
    TRACED();
    vSemaphoreDelete(xSemaphore);
  }
  void lock() override {
    TRACED();
    xSemaphoreTake(xSemaphore, portMAX_DELAY);
  }
  void unlock() override {
    TRACED();
    xSemaphoreGive(xSemaphore);
  }

protected:
  SemaphoreHandle_t xSemaphore = NULL;
};

using Mutex = MutexRTOS;

}