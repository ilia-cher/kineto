/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ActivityProfilerInterface.h"

#include <memory>
#include <set>
#include <vector>

#include "ActivityProfilerController.h"
#include "CuptiActivityInterface.h"
#include "ActivityType.h"
#include "TraceActivity.h"

namespace libkineto {

class ActivityProfilerController;
class CpuTraceBuffer;
class Config;

class ActivityProfilerProxy : public ActivityProfilerInterface {

 public:
  ActivityProfilerProxy(bool cpuOnly) : cpuOnly_(cpuOnly) {}
  ~ActivityProfilerProxy() override;

  void init() override;
  bool isInitialized() override {
    return controller_ != nullptr;
  }

  void scheduleTrace(const std::string& configStr) override;

  void scheduleTrace(const Config& config) override {
    controller_->scheduleTrace(config);
  }

  void prepareTrace(const std::set<ActivityType>& activityTypes) override;

  void startTrace() override {
    controller_->startTrace();
  }

  std::unique_ptr<ActivityTraceInterface> stopTrace() override {
    return controller_->stopTrace();
  }

  bool isActive() override {
    return controller_->isActive();
  }

  void pushCorrelationId(uint64_t id) override {
    CuptiActivityInterface::pushCorrelationID(id);
  }

  void popCorrelationId() override {
    CuptiActivityInterface::popCorrelationID();
  }

  void transferCpuTrace(
     std::unique_ptr<CpuTraceBuffer> traceBuffer) override {
    controller_->transferCpuTrace(std::move(traceBuffer));
  }

  bool traceInclusionFilter(const std::string& match) override {
    return controller_->traceInclusionFilter(match);
  }

 private:
  bool cpuOnly_{true};
  ActivityProfilerController* controller_{nullptr};
};

} // namespace libkineto
