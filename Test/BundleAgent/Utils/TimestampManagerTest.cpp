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
 * FILE TimestampManagerTest.cpp
 * AUTHOR Blackcatn13
 * DATE Jun 18, 2015
 * VERSION 1
 * This file contains the test of the TimestampManager Class.
 */

#include <unistd.h>
#include <utility>
#include "Utils/TimestampManager.h"
#include "gtest/gtest.h"

TEST(TimestampManager, Singleton) {
  TimestampManager* tm1 = TimestampManager::getInstance();
  TimestampManager* tm2 = TimestampManager::getInstance();
  ASSERT_EQ(tm1, tm2);
}

TEST(TimestampManager, TimestampSequece) {
  sleep(1);
  TimestampManager* tm1 = TimestampManager::getInstance();
  std::pair<uint64_t, uint64_t> pair1 = tm1->getTimestamp();
  std::pair<uint64_t, uint64_t> pair2 = tm1->getTimestamp();
  std::pair<uint64_t, uint64_t> pair3 = tm1->getTimestamp();
  EXPECT_EQ(pair1.first, pair2.first);
  EXPECT_EQ(pair1.first, pair3.first);
  EXPECT_EQ(0, pair1.second);
  EXPECT_EQ(1, pair2.second);
  EXPECT_EQ(2, pair3.second);
  sleep(1);
  pair2 = tm1->getTimestamp();
  pair3 = tm1->getTimestamp();
  EXPECT_EQ(pair2.first, pair3.first);
  EXPECT_GT(pair2.first, pair1.first);
  EXPECT_LT(pair2.second, pair3.second);
}
