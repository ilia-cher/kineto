/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <cstdint>
#include <string>

namespace KINETO_NAMESPACE {

class DaemonConfigLoader {
 public:
  virtual ~DaemonConfigLoader() {}

  // Return a configuration string from the daemon, if one has been posted.
  virtual std::string readOnDemandConfig(bool events, bool activities) = 0;

  // Returns the number of tracked contexts for this device. The daemon has a
  // global view. If an unexpedted error occurs, return -1.
  virtual int gpuContextCount(uint32_t device) = 0;
};

} // namespace KINETO_NAMESPACE
