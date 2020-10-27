# Copyright (c) Facebook, Inc. and its affiliates.
# All rights reserved.
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree.

def get_libkineto_srcs():
    return [
        "src/AbstractConfig.cpp",
        "src/ActivityProfilerController.cpp",
        "src/ActivityProfilerProxy.cpp",
        "src/ActivityProfiler.cpp",
        "src/Config.cpp",
        "src/ConfigLoader.cpp",
        "src/CuptiActivityInterface.cpp",
        "src/CuptiEventInterface.cpp",
        "src/CuptiMetricInterface.cpp",
        "src/cupti_strings.cpp",
        "src/Demangle.cpp",
        "src/EventProfilerController.cpp",
        "src/EventProfiler.cpp",
        "src/init.cpp",
        "src/Logger.cpp",
        "src/output_csv.cpp",
        "src/output_json.cpp",
        "src/ProcessInfo.cpp",
        "src/ThreadName.cpp",
    ]

def get_libkineto_public_headers():
    return [
        "include/ActivityProfilerInterface.h",
        "include/ActivityType.h",
        "include/ClientInterface.h",
        "include/TraceActivity.h",
        "include/TraceSpan.h",
        "include/libkineto.h",
        "include/time_since_epoch.h",
    ]

