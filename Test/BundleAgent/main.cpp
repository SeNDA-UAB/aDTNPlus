/*
* Copyright (c) 2015 SeNDA
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
 * FILE main.cpp
 * AUTHOR Blackcatn13
 * DATE Jul 3, 2015
 * VERSION 1
 * This file contains the call to start all the tests.
 */

#include <iostream>
#include <atomic>
#include <cstdint>
#include "gtest/gtest.h"
#include "Utils/Logger.h"
#include "Utils/globals.h"

std::atomic<bool> g_stop;
std::atomic<uint16_t> g_stopped;
std::atomic<uint16_t> g_startedThread;
std::atomic<uint32_t> g_processed;
std::atomic<uint32_t> g_queueSize;
std::mutex g_processorMutex;
std::condition_variable g_processorConditionVariable;

GTEST_API_ int main(int argc, char **argv) {
  g_stop = false;
  std::cout << "Runing tests" << std::endl;
  testing::InitGoogleTest(&argc, argv);
  std::cout << "Starting Logger" << std::endl;
  Logger::getInstance()->setLoggerConfigAndStart("/tmp/adtn-test.log");
  Logger::getInstance()->setLogLevel(100);
  int ret = RUN_ALL_TESTS();
  delete Logger::getInstance();
  return ret;
}
