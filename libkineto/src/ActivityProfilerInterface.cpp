/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ActivityProfilerInterface.h"
#include "ActivityProfilerController.h"
#include "Config.h"
#include "ConfigLoader.h"
#include "CuptiActivityInterface.h"

namespace libkineto {

void ActivityProfilerInterface::init() {
  // ConfigLoader singleton must be created before ActivityProfiler
  // to ensure orderly destruction on exit.
  ConfigLoader& config_loader = ConfigLoader::instance();
  auto config = config_loader.getConfigCopy();
  if (!config->activityProfilerEnabled()) {
    return;
  }
  static ActivityProfilerController controller(config_loader, cpuOnly_);
  controller_ = &controller;
}

void ActivityProfilerInterface::prepareTrace(
    const std::set<ActivityType>& activityTypes) {
  Config config;
  config.setClientDefaults();
  config.setSelectedActivityTypes(activityTypes);
  config.validate();
  controller_->prepareTrace(config);
}

void ActivityProfilerInterface::startTrace() {
  controller_->startTrace();
}

void ActivityProfilerInterface::stopTrace() {
  controller_->stopTrace();
}

void ActivityProfilerInterface::pushCorrelationId(uint64_t id) {
  CuptiActivityInterface::pushCorrelationID(id);
}

void ActivityProfilerInterface::popCorrelationId() {
  CuptiActivityInterface::popCorrelationID();
}

void ActivityProfilerInterface::transferCpuTrace(
    std::unique_ptr<CpuTraceBuffer> traceBuffer) {
  controller_->transferCpuTrace(std::move(traceBuffer));
}

bool ActivityProfilerInterface::traceInclusionFilter(const std::string& match) {
  return controller_->traceInclusionFilter(match);
}

} // namespace libkineto
