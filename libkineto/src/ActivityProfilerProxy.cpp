/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ActivityProfilerProxy.h"

#include "ActivityProfilerController.h"
#include "Config.h"
#include "ConfigLoader.h"
#include "CuptiActivityInterface.h"

namespace libkineto {

void ActivityProfilerProxy::init() {
  // ConfigLoader singleton must be created before ActivityProfiler
  // to ensure orderly destruction on exit.
  ConfigLoader& config_loader = ConfigLoader::instance();
  auto config = config_loader.getConfigCopy();
  if (!config->activityProfilerEnabled()) {
    return;
  }
  if (!controller_) {
    controller_ = new ActivityProfilerController(config_loader, cpuOnly_);
  }
}

ActivityProfilerProxy::~ActivityProfilerProxy() {
  delete controller_;
};

void ActivityProfilerProxy::scheduleTrace(const std::string& configStr) {
  Config config;
  config.parse(configStr);
  controller_->scheduleTrace(config);
}

void ActivityProfilerProxy::prepareTrace(
    const std::set<ActivityType>& activityTypes) {
  Config config;
  config.setClientDefaults();
  config.setSelectedActivityTypes(activityTypes);
  config.validate();
  controller_->prepareTrace(config);
}

} // namespace libkineto
