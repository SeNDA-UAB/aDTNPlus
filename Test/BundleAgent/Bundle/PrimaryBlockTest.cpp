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
 * FILE PrimaryBlockTest.cpp
 * AUTHOR Blackcatn13
 * DATE Jun 18, 2015
 * VERSION 1
 * This file contains the test of the PrimaryBlock class.
 */

#include <string>
#include <utility>
#include "Bundle/PrimaryBlock.h"
#include "Utils/TimestampManager.h"
#include "gtest/gtest.h"

/**
 * Check constructor with parameters.
 * After generating the PrimaryBlock the values must be the same,
 * and the reportTo and Custodian must be empty.
 */
TEST(PrimaryBlockTest, ParametersConstructor) {
  std::pair<uint64_t, uint64_t> time = TimestampManager::getInstance()
      ->getTimestamp();
  PrimaryBlock pb = PrimaryBlock("source", "destination", time.first,
                                 time.second);
  ASSERT_EQ("source", pb.getSource());
  ASSERT_EQ("destination", pb.getDestination());
  ASSERT_EQ("", pb.getReportTo());
  ASSERT_EQ("", pb.getCustodian());
  ASSERT_EQ(time.first, pb.getCreationTimestamp());
  ASSERT_EQ(time.second, pb.getCreationTimestampSeqNumber());
}

/**
 * Check raw functions.
 * After generating a PrimaryBlock, converting it to raw, and generating
 * a new PrimaryBlock with this raw data, the values must be the same.
 */
TEST(PrimaryBlockTest, RawFunctions) {
  std::pair<uint64_t, uint64_t> time = TimestampManager::getInstance()
      ->getTimestamp();
  PrimaryBlock pb = PrimaryBlock("source", "destination", time.first,
                                 time.second);
  std::string rawData = pb.getRaw();
  PrimaryBlock pb1 = PrimaryBlock(rawData);
  ASSERT_EQ(pb.getSource(), pb1.getSource());
  ASSERT_EQ(pb.getDestination(), pb1.getDestination());
  ASSERT_EQ(pb.getCreationTimestamp(), pb1.getCreationTimestamp());
  ASSERT_EQ(pb.getCreationTimestampSeqNumber(),
            pb1.getCreationTimestampSeqNumber());
}
