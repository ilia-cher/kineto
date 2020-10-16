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

} // namespace libkineto
