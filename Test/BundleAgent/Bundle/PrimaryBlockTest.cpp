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
  std::string rawData = pb.toRaw();
  PrimaryBlock pb1 = PrimaryBlock(rawData);
  ASSERT_EQ(pb.getSource(), pb1.getSource());
  ASSERT_EQ(pb.getDestination(), pb1.getDestination());
  ASSERT_EQ(pb.getCreationTimestamp(), pb1.getCreationTimestamp());
  ASSERT_EQ(pb.getCreationTimestampSeqNumber(),
            pb1.getCreationTimestampSeqNumber());
}

/**
 * Check a full PrimaryBlock to raw and to PrimaryBlock again.
 */
TEST(PrimaryBlockTest, FullRawFunctions) {
  std::pair<uint64_t, uint64_t> time = TimestampManager::getInstance()
      ->getTimestamp();
  PrimaryBlock pb = PrimaryBlock("Source", "Destination", time.first,
                                 time.second);
  pb.setReportTo("ReportTo");
  pb.setCustodian("Custodian");
  pb.setLifetime(time.first);
  pb.setPrimaryProcFlag(PrimaryBlockControlFlags::CUSTODY_TRANSFER);
  pb.setPrimaryProcFlag(PrimaryBlockControlFlags::PRIORITY_NORMAL);
  pb.setPrimaryProcFlag(PrimaryBlockControlFlags::REQUEST_FORWARDING);
  ASSERT_TRUE(
      pb.checkPrimaryProcFlag(PrimaryBlockControlFlags::PRIORITY_NORMAL));
  ASSERT_TRUE(
      pb.checkPrimaryProcFlag(PrimaryBlockControlFlags::REQUEST_FORWARDING));
  pb.unsetPrimaryProcFlag(PrimaryBlockControlFlags::CUSTODY_TRANSFER);
  ASSERT_FALSE(
      pb.checkPrimaryProcFlag(PrimaryBlockControlFlags::CUSTODY_TRANSFER));
  std::string rawData = pb.toRaw();
  PrimaryBlock pb1 = PrimaryBlock(rawData);
  ASSERT_EQ(pb.getSource(), pb1.getSource());
  ASSERT_EQ(pb.getDestination(), pb1.getDestination());
  ASSERT_EQ(pb.getReportTo(), pb1.getReportTo());
  ASSERT_EQ(pb.getCustodian(), pb1.getCustodian());
  ASSERT_EQ(pb.getLifetime(), pb1.getLifetime());
  ASSERT_EQ(pb.getCreationTimestamp(), pb1.getCreationTimestamp());
  ASSERT_EQ(pb.getCreationTimestampSeqNumber(),
            pb1.getCreationTimestampSeqNumber());
  ASSERT_EQ(
      pb.checkPrimaryProcFlag(PrimaryBlockControlFlags::CUSTODY_TRANSFER),
      pb1.checkPrimaryProcFlag(PrimaryBlockControlFlags::CUSTODY_TRANSFER));
  ASSERT_EQ(
      pb.checkPrimaryProcFlag(PrimaryBlockControlFlags::PRIORITY_NORMAL),
      pb1.checkPrimaryProcFlag(PrimaryBlockControlFlags::PRIORITY_NORMAL));
  ASSERT_EQ(
      pb.checkPrimaryProcFlag(PrimaryBlockControlFlags::REQUEST_FORWARDING),
      pb1.checkPrimaryProcFlag(PrimaryBlockControlFlags::REQUEST_FORWARDING));
  ASSERT_EQ(
      pb.checkPrimaryProcFlag(PrimaryBlockControlFlags::DESTINATION_SINGLETON),
      pb1.checkPrimaryProcFlag(
          PrimaryBlockControlFlags::DESTINATION_SINGLETON));
  ASSERT_EQ(
      pb.checkPrimaryProcFlag(
          PrimaryBlockControlFlags::IS_ADMINISTRATIVE_RECORD),
      pb1.checkPrimaryProcFlag(
          PrimaryBlockControlFlags::IS_ADMINISTRATIVE_RECORD));
}

/**
 * Check the flag functions
 */
TEST(PrimaryBlockTest, FlagFunctions) {
  std::pair<uint64_t, uint64_t> time = TimestampManager::getInstance()
      ->getTimestamp();
  PrimaryBlock pb = PrimaryBlock("Source", "Destination", time.first,
                                 time.second);
  pb.setPrimaryProcFlag(PrimaryBlockControlFlags::PRIORITY_BULK);
  ASSERT_TRUE(pb.checkPrimaryProcFlag(PrimaryBlockControlFlags::PRIORITY_BULK));
  ASSERT_FALSE(
      pb.checkPrimaryProcFlag(PrimaryBlockControlFlags::PRIORITY_NORMAL));
  pb.unsetPrimaryProcFlag(PrimaryBlockControlFlags::PRIORITY_BULK);
  // Default priority is BULK
  ASSERT_TRUE(pb.checkPrimaryProcFlag(PrimaryBlockControlFlags::PRIORITY_BULK));
  pb.setPrimaryProcFlag(PrimaryBlockControlFlags::PRIORITY_EXPEDITED);
  pb.setPrimaryProcFlag(PrimaryBlockControlFlags::PRIORITY_NORMAL);
  ASSERT_FALSE(
      pb.checkPrimaryProcFlag(PrimaryBlockControlFlags::PRIORITY_EXPEDITED));
  ASSERT_TRUE(
      pb.checkPrimaryProcFlag(PrimaryBlockControlFlags::PRIORITY_NORMAL));
  pb.unsetPrimaryProcFlag(PrimaryBlockControlFlags::PRIORITY_NORMAL);
  ASSERT_FALSE(
      pb.checkPrimaryProcFlag(PrimaryBlockControlFlags::PRIORITY_NORMAL));
  pb.setPrimaryProcFlag(PrimaryBlockControlFlags::PRIORITY_EXPEDITED);
  pb.unsetPrimaryProcFlag(PrimaryBlockControlFlags::PRIORITY_EXPEDITED);
  ASSERT_FALSE(
      pb.checkPrimaryProcFlag(PrimaryBlockControlFlags::PRIORITY_EXPEDITED));
}

/**
 * Check a bad block construction
 */
TEST(PrimaryBlockTest, PrimaryBlockConstructionException) {
  std::pair<uint64_t, uint64_t> time = TimestampManager::getInstance()
      ->getTimestamp();
  PrimaryBlock pb = PrimaryBlock("Source", "Destination", time.first,
                                 time.second);
  pb.setReportTo("ReportTo");
  pb.setCustodian("Custodian");
  pb.setLifetime(time.first);
  pb.setPrimaryProcFlag(PrimaryBlockControlFlags::CUSTODY_TRANSFER);
  pb.setPrimaryProcFlag(PrimaryBlockControlFlags::PRIORITY_NORMAL);
  pb.setPrimaryProcFlag(PrimaryBlockControlFlags::REQUEST_FORWARDING);
  std::stringstream ss;
  ss << pb.toRaw();
  std::string raw = ss.str();
  ASSERT_THROW(PrimaryBlock(raw.substr(0, 5)),
               BlockConstructionException);
}
