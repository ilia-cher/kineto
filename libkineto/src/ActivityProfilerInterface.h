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

namespace libkineto {

class CpuTraceBuffer;
class ActivityProfilerController;

class ActivityProfilerInterface {

 public:
   ActivityProfilerInterface(bool cpuOnly) : cpuOnly_(cpuOnly) {}

  void init();
  inline bool isInitialized() {
    return controller_ != nullptr;
  }

  void prepareTrace(const std::set<ActivityType>& activityTypes);
  void startTrace();
  std::vector<ActivityEvent> stopTrace();
  void pushCorrelationId(uint64_t id);
  void popCorrelationId();
  void transferCpuTrace(
      std::unique_ptr<CpuTraceBuffer> traceBuffer);
  bool traceInclusionFilter(const std::string& match);

  std::vector<ActivityEvent> getEvents();

 private:
  bool cpuOnly_{true};
  ActivityProfilerController* controller_{nullptr};
};

} // namespace libkineto
