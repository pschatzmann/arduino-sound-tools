#pragma once
#include "AudioConfig.h"
#include <atomic>

#ifdef USE_STD_CONCURRENCY
#  include <mutex>
#endif

namespace audio_tools {

/**
 * @brief Empty Mutex implementation which does nothing
 * @ingroup concurrency
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class MutexBase {
public:
  virtual void lock() {}
  virtual void unlock() {}
};

class SpinLock : public MutexBase {
  volatile std::atomic<bool> lock_ = {0};

  void lock() {
    for (;;) {
      // Optimistically assume the lock is free on the first try
      if (!lock_.exchange(true, std::memory_order_acquire)) {
        return;
      }
      // Wait for lock to be released without generating cache misses
      while (lock_.load(std::memory_order_relaxed)) {
        // Issue X86 PAUSE or ARM YIELD instruction to reduce contention between
        // hyper-threads
        //__builtin_ia32_pause();
        delay(1);
      }
    }
  }

  bool try_lock() {
    // First do a relaxed load to check if lock is free in order to prevent
    // unnecessary cache misses if someone does while(!try_lock())
    return !lock_.load(std::memory_order_relaxed) &&
           !lock_.exchange(true, std::memory_order_acquire);
  }

  void unlock() {
    lock_.store(false, std::memory_order_release);
  }
};


#if defined(USE_STD_CONCURRENCY) 

/**
 * @brief Mutex implemntation based on std::mutex
 * @ingroup concurrency
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class StdMutex : public MutexBase {
public:
  void lock() override { std_mutex.lock(); }
  void unlock() override { std_mutex.unlock(); }

protected:
  std::mutex std_mutex;
};

#endif


}