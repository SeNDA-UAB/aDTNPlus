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
 * FILE globals.h
 * AUTHOR Blackcatn13
 * DATE Jul 6, 2015
 * VERSION 1
 * This file contains the global variables.
 */
#ifndef BUNDLEAGENT_UTILS_GLOBALS_H_
#define BUNDLEAGENT_UTILS_GLOBALS_H_

#include <cstdint>
#include <atomic>
#include <mutex>
#include <condition_variable>

extern std::atomic<bool> g_stop;
extern std::atomic<uint16_t> g_stopped;
extern std::atomic<uint16_t> g_startedThread;
extern std::atomic<uint32_t> g_queueSize;
extern std::mutex g_processorMutex;
extern std::condition_variable g_processorConditionVariable;
extern std::atomic<uint32_t> g_queueProcessEvents;

#endif  // BUNDLEAGENT_UTILS_GLOBALS_H_
