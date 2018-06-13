/*
 * Copyright (c) 2018 SeNDA
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
/**
 * FILE PerfLogger.h
 * AUTHOR Blackcatn13
 * DATE Apr 6, 2018
 * VERSION 1
 *
 */
#ifndef BUNDLEAGENT_UTILS_PERFLOGGER_H_
#define BUNDLEAGENT_UTILS_PERFLOGGER_H_

#ifndef PERF_ACTIVE
#define PERF_ACTIVE true
#endif

#include <cstring>
#include <fstream>
#include <string>
#include <mutex>
#include <thread>
#include "Utils/Perfstream.h"
#include "Utils/ConfigLoader.h"

class PerfLogger {
 public:
  virtual ~PerfLogger();

  void log(int level, std::string message);

  static PerfLogger* getInstance();

  Perfstream operator()(int level);

  void setConfigAndStart(std::string filename);

 private:

  PerfLogger();

  static PerfLogger* m_instance;

  std::ofstream os;

  std::mutex m_mutex;

  std::string m_filename;
};

enum PerfMessages {
  PLATFORM_START,
  PLATFORM_END,
  MESSAGE_CREATED,
  MESSAGE_RELAYED_FROM,
  MESSAGE_RELAYED,
  MESSAGE_DELIVERED,
  MESSAGE_DROPPED,
  MESSAGE_REMOVED,
  MESSAGE_RECEIVED,
  NEIGH_APPEAR,
  NEIGH_DISAPPEAR
};

#define PERF(level)                                       \
  if (!PERF_ACTIVE) ;                                     \
  else                                                    \
    PerfLogger::getInstance()->operator()(level)          \

#endif  // BUNDLEAGENT_UTILS_PERFLOGGER_H_
