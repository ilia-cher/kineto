/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <signal.h>
#include <atomic>
#include <chrono>
#include <functional>
#include <mutex>
#include <string>
#include <set>
#include <thread>
#include <vector>

#include "ActivityType.h"
#include "ActivityProfilerInterface.h"
#include "ClientInterface.h"
#include "CpuOpInfo.h"

namespace libkineto {

struct CpuTraceBuffer {
  std::string netName;
  int64_t startTime;
  int64_t endTime;
  int gpuOpCount;
  std::vector<CpuOpInfo> ops;
};

class LibkinetoApi {
 public:
  void prepareTrace(const std::set<ActivityType>& activities);

  void startTrace() {
    activityProfiler_->startTrace();
  }

  void stopTrace() {
    activityProfiler_->stopTrace();
  }

  void transferCpuTrace(std::unique_ptr<CpuTraceBuffer> cpuTrace) {
    activityProfiler_->transferCpuTrace(std::move(cpuTrace));
  }

  void pushCorrelationId(uint64_t id) {
    activityProfiler_->pushCorrelationId(id);
  }

  void popCorrelationId() {
    activityProfiler_->popCorrelationId();
  }

  // Called by ActivityProfiler on init
  void registerProfiler(std::unique_ptr<ActivityProfilerInterface> profiler) {
    activityProfiler_ = std::move(profiler);
    initClientIfRegistered();
  }

  // Return whether the start/stopProfiler have been registered
  inline bool hasProfilerRegistered() {
    return activityProfiler_ != nullptr;
  }

  // Called by client that supports tracing API.
  // libkineto can still function without this.
  void registerClient(ClientInterface* client);

  ClientInterface* client() {
    return client_;
  }

  void initProfilerIfRegistered() {
    if (activityProfiler_ && !activityProfiler_->isInitialized()) {
      activityProfiler_->init();
    }
  }

  bool isProfilerInitialized() {
    return activityProfiler_ && activityProfiler_->isInitialized();
  }

  // Profile net with this name
  // FIXME: Change to enableForTrace?
  bool traceInclusionFilter(const std::string& name) {
    return activityProfiler_->traceInclusionFilter(name);
  }

  // Profile nets with at least this many ops
  int netSizeThreshold() {
    return netSizeThreshold_;
  }

  void setNetSizeThreshold(int gpu_ops) {
    netSizeThreshold_ = gpu_ops;
  }

 private:

  // Client is initialized once both it and libkineto has registered
  void initClientIfRegistered();

  std::unique_ptr<ActivityProfilerInterface> activityProfiler_{};
  ClientInterface* client_{};
  pthread_t clientRegisterThread_{0};

  bool isLoaded_{false};
  std::atomic_int netSizeThreshold_{};
};

// Singleton
LibkinetoApi& api();

} // namespace libkineto
