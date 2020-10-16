/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <ostream>

namespace libkineto {

enum class ActivityType {
  GPU_MEMCPY,
  GPU_MEMSET,
  CONCURRENT_KERNEL,
  EXTERNAL_CORRELATION,
  CUDA_RUNTIME
};

struct ActivityEvent {
  std::string name;
  uint64_t startUs;
  uint64_t endUs;
  uint32_t deviceId;
  uint32_t threadId;
  uint32_t correlationId;
  ActivityType type;
};

} // namespace libkineto
