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
 * FILE TimestampManager.h
 * AUTHOR Blackcatn13
 * DATE Jun 16, 2015
 * VERSION 1
 * This file contains the class TimestampManager
 */
#ifndef BUNDLEAGENT_UTILS_TIMESTAMPMANAGER_H_
#define BUNDLEAGENT_UTILS_TIMESTAMPMANAGER_H_

#include <cstdint>
#include <utility>
#include <mutex>

const uint64_t g_timeFrom2000 = 946684800;

/**
 * CLASS TimsetampManager
 * This class is a singleton that gives a timestamp, and a sequence number.
 * The sequence number is used to identify different bundles with the same timestamp.
 */
class TimestampManager {
 public:
  /**
   * @brief Singleton instance
   *
   * Function to get a reference to the singleton class.
   *
   * @return A pointer to the singleton.
   */
  static TimestampManager* getInstance();
  /**
   * Destructor of the class.
   */
  virtual ~TimestampManager();
  /**
   * @brief Function that returns a timestamp an a sequence number.
   *
   * This function returns a pair, the first element of the pair is the timestamp value.
   * The second element is the sequence number.
   *
   * @return A pair containing the timestamp and the sequence number.
   */
  std::pair<uint64_t, uint64_t> getTimestamp();

 private:
  /**
   * Constructor of the TimestampManager.
   */
  TimestampManager();
  /**
   * Instance of the singleton.
   */
  static TimestampManager *m_instance;
  /**
   * Last timestamp generated.
   */
  uint64_t m_lastTimestamp;
  /**
   * Last sequence number given.
   */
  uint64_t m_lastSeqNum;

  std::mutex m_mutex;


};

#endif  // BUNDLEAGENT_UTILS_TIMESTAMPMANAGER_H_
