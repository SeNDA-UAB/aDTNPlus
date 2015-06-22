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
 * FILE PayloadBlockTest.cpp
 * AUTHOR Blackcatn13
 * DATE Jun 18, 2015
 * VERSION 1
 * This file contains the test of the PayloadBlock class.
 */

#include <string>
#include "Bundle/PayloadBlock.h"
#include "gtest/gtest.h"

/**
 * Check the default constructor.
 * The payload must be empty, and the block type must be
 * a PAYLOAD_BLOCK type.
 */
TEST(PayloadBlockTest, DefaultConstructor) {
  PayloadBlock pb = PayloadBlock();
  ASSERT_EQ("", pb.getPayload());
  ASSERT_EQ(static_cast<uint8_t>(BlockTypes::PAYLOAD_BLOCK),
              pb.getBlockType());
}

/**
 * Check the payload set.
 * After setting a payload and then retrieving it, they must have
 * the same value.
 */
TEST(PayloadBlockTest, SetGetPayload) {
  PayloadBlock pb = PayloadBlock();
  ASSERT_EQ("", pb.getPayload());
  pb.setPayload("This is a test payload");
  ASSERT_EQ("This is a test payload", pb.getPayload());
  pb.setPayload("This is a new test payload");
  ASSERT_EQ("This is a new test payload", pb.getPayload());
}

TEST(PayloadBlockTest, RawFunctions) {
  PayloadBlock pb = PayloadBlock();
  pb.setPayload("This is a test payload");
  std::string rawBlock = pb.getRaw();
  PayloadBlock pb1 = PayloadBlock(rawBlock);
  ASSERT_EQ(static_cast<uint8_t>(BlockTypes::PAYLOAD_BLOCK),
                pb1.getBlockType());
  ASSERT_EQ("This is a test payload", pb1.getPayload());
}
