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
 * FILE PerfLogger.cpp
 * AUTHOR Blackcatn13
 * DATE Apr 6, 2018
 * VERSION 1
 *
 */

#include "Utils/PerfLogger.h"
#include <ctime>
#include <string>
#include <sstream>
#include <chrono>
#include <iostream>

PerfLogger *PerfLogger::m_instance = 0;

PerfLogger::PerfLogger() {
}

PerfLogger::~PerfLogger() {
  PERF(PLATFORM_END);
  m_instance = 0;
  os.close();
}

void PerfLogger::log(int level, std::string message) {
  m_mutex.lock();
  std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
  auto duration = now.time_since_epoch();
  auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration)
      .count();
  os << level << ' ' << millis << ' ' << message << std::endl;
  m_mutex.unlock();
}

PerfLogger* PerfLogger::getInstance() {
  if (m_instance == 0)
    m_instance = new PerfLogger();
  return m_instance;
}

Perfstream PerfLogger::operator()(int level) {
  return Perfstream(this, level);
}

void PerfLogger::setConfigAndStart(std::string filename) {
  os.open(filename, std::ios_base::trunc);
}
