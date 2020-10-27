/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <memory>
#include <set>
#include <vector>

#include "ActivityType.h"
#include "ActivityTraceInterface.h"

namespace libkineto {

class ActivityProfilerController;
class CpuTraceBuffer;
class Config;

class ActivityProfilerInterface {

 public:
  virtual ~ActivityProfilerInterface() {};

  virtual void init() {}
  virtual bool isInitialized() {
    return false;
  }
  virtual bool isActive(){
    return false;
  }

  virtual void scheduleTrace(const std::string& configStr) {}
  virtual void scheduleTrace(const Config& configStr) {}
  virtual void prepareTrace(const std::set<ActivityType>& activityTypes) {}
  virtual void startTrace() {}
  virtual std::unique_ptr<ActivityTraceInterface> stopTrace() {
    return nullptr;
  }
  virtual void pushCorrelationId(uint64_t id){}
  virtual void popCorrelationId(){}
  virtual void transferCpuTrace(
      std::unique_ptr<CpuTraceBuffer> traceBuffer){}
  virtual bool traceInclusionFilter(const std::string& match) {
    return true;
  }
};

} // namespace libkineto
