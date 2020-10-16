/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <atomic>
#include <memory>
#include <thread>

#include "ActivityProfiler.h"
#include "ActivityProfilerInterface.h"

namespace KINETO_NAMESPACE {

class Config;
class ConfigLoader;

using ActivityLoggerFactory =
    std::function<std::unique_ptr<ActivityLogger>(const Config&)>;

class ActivityProfilerController {
 public:
  explicit ActivityProfilerController(ConfigLoader& configLoader, bool cpuOnly);
  ActivityProfilerController(const ActivityProfilerController&) = delete;
  ActivityProfilerController& operator=(const ActivityProfilerController&) =
      delete;

  ~ActivityProfilerController();

  static void setLoggerFactory(const ActivityLoggerFactory& factory);

  void prepareTrace(Config& config);

  void startTrace() {
    profiler_->startTrace(std::chrono::system_clock::now());
  }

  std::vector<ActivityEvent> stopTrace() {
    // FIXME: Refactor
    if (libkineto::api().client()) {
      libkineto::api().client()->stop();
    }
    return profiler_->stopTrace(std::chrono::system_clock::now());
  }

  bool traceInclusionFilter(const std::string& match) {
    return profiler_->applyNetFilter(match);
  }

  void transferCpuTrace(
      std::unique_ptr<libkineto::CpuTraceBuffer> cpuTrace) {
    return profiler_->transferCpuTrace(std::move(cpuTrace));
  }

  std::vector<ActivityEvent> getEvents();

 private:
  void profilerLoop();

  ConfigLoader& configLoader_;
  std::unique_ptr<ActivityProfiler> profiler_;
  std::thread* profilerThread_;
  std::atomic_bool stopRunloop_{false};
};

} // namespace KINETO_NAMESPACE
