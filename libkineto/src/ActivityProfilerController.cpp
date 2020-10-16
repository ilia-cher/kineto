/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ActivityProfilerController.h"

#include <chrono>
#include <thread>

#include "ConfigLoader.h"
#include "CuptiActivityInterface.h"
#include "ThreadName.h"
#include "output_json.h"

#include "Logger.h"

using namespace std::chrono;

namespace KINETO_NAMESPACE {

constexpr milliseconds kDefaultInactiveProfilerIntervalMsecs(1000);
constexpr milliseconds kDefaultActiveProfilerIntervalMsecs(200);

ActivityProfilerController::ActivityProfilerController(
    ConfigLoader& configLoader, bool cpuOnly)
    : configLoader_(configLoader) {
  profiler_ = std::make_unique<ActivityProfiler>(
      CuptiActivityInterface::singleton(), cpuOnly);
  // start a profilerLoop() thread to handle trace requests
  profilerThread_ =
      new std::thread(&ActivityProfilerController::profilerLoop, this);
}

ActivityProfilerController::~ActivityProfilerController() {
  LOG(INFO) << "Destroying ActivityProfilerController";
  if (profilerThread_ != nullptr) {
    // signaling termination of the profiler loop
    stopRunloop_ = true;
    profilerThread_->join();
    delete profilerThread_;
    profilerThread_ = nullptr;
  }
  VLOG(0) << "Stopped activity profiler";
}

static ActivityLoggerFactory& loggerFactory() {
  static ActivityLoggerFactory factory{nullptr};
  return factory;
}

void ActivityProfilerController::setLoggerFactory(
    const ActivityLoggerFactory& factory) {
  loggerFactory() = factory;
}

static std::unique_ptr<ActivityLogger> makeLogger(const Config& config) {
  if (loggerFactory()) {
    return loggerFactory()(config);
  }
  return std::make_unique<ChromeTraceLogger>(config.activitiesLogFile());
}

static milliseconds profilerInterval(bool profilerActive) {
  return profilerActive ? kDefaultActiveProfilerIntervalMsecs
                        : kDefaultInactiveProfilerIntervalMsecs;
}

void ActivityProfilerController::profilerLoop() {
  setThreadName("CUPTI Activity Profiler");
  VLOG(0) << "Entering activity profiling loop";

  std::unique_ptr<Config> config = std::make_unique<Config>();

  auto now = system_clock::now();
  auto next_wakeup_time = now + profilerInterval(false);

  while (!stopRunloop_) {
    now = system_clock::now();

    while (now < next_wakeup_time) {
      /* sleep override */
      std::this_thread::sleep_for(next_wakeup_time - now);
      now = system_clock::now();
    }
    if (!profiler_->isActive() &&
        configLoader_.hasNewActivityProfilerOnDemandConfig(*config)) {
      LOG(INFO) << "Received on-demand activity trace request";
      config = configLoader_.getActivityProfilerOnDemandConfigCopy();
      profiler_->configure(*config, makeLogger(*config), now);
    }

    configLoader_.setActivityProfilerBusy(profiler_->isActive());

    while (next_wakeup_time < now) {
      next_wakeup_time += profilerInterval(profiler_->isActive());
    }

    if (profiler_->isActive()) {
      next_wakeup_time = profiler_->performRunLoopStep(now, next_wakeup_time);
      VLOG(1) << "Profiler loop: "
          << duration_cast<milliseconds>(system_clock::now() - now).count()
          << "ms";
    }
  }

  VLOG(0) << "Exited activity profiling loop";
}

void ActivityProfilerController::prepareTrace(Config& config) {
  // Requests from ActivityProfilerApi has higher priority than
  // requests from other sources (signal, daemon).
  // Cancel any ongoing request and refuse new ones.
  auto now = system_clock::now();
  configLoader_.setActivityProfilerBusy(true);
  if (profiler_->isActive()) {
    LOG(WARNING) << "Cancelling current trace request in order to start "
                 << "higher priority synchronous request";
    profiler_->cancelTrace(now);
  }

  profiler_->configure(config, makeLogger(config), now);
}

std::vector<ActivityEvent> ActivityProfilerController::getEvents() {
  return profiler_->getEvents();
}

} // namespace KINETO_NAMESPACE
