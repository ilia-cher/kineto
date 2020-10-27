/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

// Mediator for initialization and profiler control

#pragma once

#include <signal.h>
#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <set>
#include <thread>
#include <vector>

#include "ActivityProfilerInterface.h"
#include "ActivityTraceInterface.h"
#include "ActivityType.h"
#include "ClientInterface.h"
#include "ClientTraceActivity.h"
#include "TraceSpan.h"

namespace libkineto {

class Config;

// FIXME: Replace
struct CpuTraceBuffer {
  TraceSpan span;
  int gpuOpCount;
  std::vector<ClientTraceActivity> ops;
};

class LibkinetoApi {
 public:

  // *** Initialization ***
  // Called by client that supports tracing API.
  // libkineto can still function without this.
  void registerClient(ClientInterface* client);


  // *** Synchronous API ***
  // Client must call these in order:
  // prepareTrace -> startTrace -> stopTrace.

  // Many tracing structures are lazily initialized during trace collection,
  // with potentially high overhead.
  // Call prepareTrace to enable tracing, then run the the region to trace
  // at least once (and ideally run the same code that is to be traced) to
  // allow tracing structures to be initialized.
  // TODO: Add optional config string param
  void prepareTrace(const std::set<ActivityType>& activities);

  // Start recording, potentially reusing any buffers allocated since
  // prepareTrace was called.
  void startTrace() {
    activityProfiler_->startTrace();
  }

  // Stop and process trace, producing an in-memory list of trace records.
  // The processing is done synchronously (using the calling thread.)
  const std::unique_ptr<ActivityTraceInterface> stopTrace() {
    return activityProfiler_->stopTrace();
  }

  // *** Asynchronous API ***
  // Instead of starting and stopping the trace manually, provide a start time
  // and duration and / or iteration stop criterion.
  // Tracing terminates when either condition is met.
  void scheduleTrace(const std::string& config) {
    // This may include more profile types in the future
    activityProfiler_->scheduleTrace(config);
  }

  // Same as above but for already parsed config object
  void scheduleTrace(const Config& config) {
    // This may include more profile types in the future
    activityProfiler_->scheduleTrace(config);
  }

  // FIXME: Add profilerStatus
  bool traceActive() {
    return activityProfiler_->isActive();
  }

  // *** Observer API ***
  // FIXME: Pass activityProfiler interface into clientInterface instead?
  void transferCpuTrace(std::unique_ptr<CpuTraceBuffer> cpuTrace) {
    activityProfiler_->transferCpuTrace(std::move(cpuTrace));
  }

  void pushCorrelationId(uint64_t id) {
    activityProfiler_->pushCorrelationId(id);
  }

  void popCorrelationId() {
    activityProfiler_->popCorrelationId();
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


  // *** libkineto API ***
  // Called by libkineto on init
  void registerProfiler(std::unique_ptr<ActivityProfilerInterface> profiler) {
    activityProfiler_ = std::move(profiler);
    initClientIfRegistered();
  }

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
