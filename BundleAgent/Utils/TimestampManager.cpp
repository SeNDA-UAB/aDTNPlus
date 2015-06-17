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
 * FILE TimestampManager.cpp
 * AUTHOR Blackcatn13
 * DATE Jun 16, 2015
 * VERSION 1
 * This file contains the implementation of the TimestampManager class.
 */

#include "Utils/TimestampManager.h"
#include <utility>
#include <ctime>

TimestampManager *TimestampManager::m_instance = 0;

const uint64_t g_timeFrom2000 = 946684800;

TimestampManager::TimestampManager()
    : m_lastTimestamp(0),
      m_lastSeqNum(0) {
}

TimestampManager::~TimestampManager() {
  delete m_instance;
}

TimestampManager* TimestampManager::getInstance() {
  if (m_instance == 0) {
    m_instance = new TimestampManager();
  }
  return m_instance;
}

std::pair<uint64_t, uint64_t> TimestampManager::getTimestamp() {
  uint64_t timestamp = time(NULL) - g_timeFrom2000;
  if (timestamp != m_lastTimestamp) {
    m_lastSeqNum = 0;
    m_lastTimestamp = timestamp;
  } else {
    m_lastSeqNum++;
  }
  return std::pair<uint64_t, uint64_t>(m_lastTimestamp, m_lastSeqNum);
}

