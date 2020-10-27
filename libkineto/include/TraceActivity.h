/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <string>
#include <sys/types.h>
#include <unistd.h>

namespace libkineto {

class ActivityLogger;

struct TraceActivity {
  virtual ~TraceActivity() {}
  virtual int64_t deviceId() const = 0;
  virtual int64_t resourceId() const = 0;
  virtual int64_t timestamp() const = 0;
  virtual int64_t duration() const = 0;
  virtual int64_t correlationId() const = 0;
  virtual const std::string name() const = 0;
  virtual const TraceActivity* linkedActivity() const = 0;
  virtual void log(ActivityLogger& logger) const = 0;
};

namespace {
  // Caching pid is not safe across forks and clones but we currently
  // don't support an active profiler in a forked process.
  static inline pid_t cachedPid() {
    static pid_t pid = getpid();
    return pid;
  }

  static inline int64_t nsToUs(int64_t ns) {
    // It's important that this conversion is the same everywhere.
    // No rounding!
    return ns / 1000;
  }
}

} // namespace libkineto
