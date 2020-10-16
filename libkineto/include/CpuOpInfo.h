/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <string>
#include <thread>

namespace libkineto {

struct CpuOpInfo {
  int64_t startTime;
  int64_t endTime;
  uint64_t correlationId;
  std::string opType;
  int deviceId;
  pthread_t threadId;
  std::string inputDims;
  std::string inputTypes;
  std::string arguments;
  std::string outputDims;
  std::string outputTypes;
  std::string inputNames;
  std::string outputNames;
};

} // namespace libkineto
