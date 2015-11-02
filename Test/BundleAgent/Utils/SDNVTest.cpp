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
 * FILE SDNVTest.cpp
 * AUTHOR Blackcatn13
 * DATE Jun 22, 2015
 * VERSION 1
 * This file contains the tests for the SDNV functions
 */

#include <string>
#include <sstream>
#include "Utils/SDNV.h"
#include "gtest/gtest.h"

/**
 * Check the calculus of codified length.
 */
TEST(SDNVTest, LengthTest) {
  ASSERT_EQ(1, SDNV::getLength(127));
  ASSERT_EQ(2, SDNV::getLength(15789));
  ASSERT_EQ(3, SDNV::getLength(45123));
  ASSERT_EQ(4, SDNV::getLength(9123456));
  ASSERT_EQ(5, SDNV::getLength(4294967295));
}

/**
 * Check the length of an encoded value.
 * The length of an encoded value must be the same that
 * the origin value.
 */
TEST(SDNVTest, EncodeTest) {
  for (uint64_t i = 1; i <= (2 ^ 32); i += (2 ^ 7)) {
    std::string coded = SDNV::encode(i);
    ASSERT_EQ(coded.length(), SDNV::getLength(i));
  }
}

/**
 * Check the decode function.
 * Encode a value first and decode it after,
 * the decoded value must be the same as the encoded.
 */
TEST(SDNVTest, EncodeDecodeTest) {
  for (uint64_t i = 1; i <= (2 ^ 32); i += (2 ^ 7)) {
    std::string coded = SDNV::encode(i);
    ASSERT_EQ(coded.length(), SDNV::getLength(i));
    uint64_t value = SDNV::decode(coded);
    ASSERT_EQ(i, value);
  }
}

/**
 * Check the length of an encoded value inside a byte array.
 * The length of the encoded value must be the same as the
 * length of the original value.
 */
TEST(SDNVTest, LengthStringTest) {
  std::string coded = SDNV::encode(127);
  ASSERT_EQ(1, SDNV::getLength(coded));
  coded = SDNV::encode(15789);
  ASSERT_EQ(2, SDNV::getLength(coded));
  coded = SDNV::encode(45123);
  ASSERT_EQ(3, SDNV::getLength(coded));
  coded = SDNV::encode(9123456);
  ASSERT_EQ(4, SDNV::getLength(coded));
  coded = SDNV::encode(4294967295);
  ASSERT_EQ(5, SDNV::getLength(coded));
}

/**
 * Check encode and decode of large numbers.
 * The length of the encoded value, and the decoded value
 * must be equals to the original values.
 */
TEST(SDNVTest, EncodeDecodeLargeTest) {
  std::string coded;
  uint64_t value;
  coded = SDNV::encode(127);
  ASSERT_EQ(1, SDNV::getLength(coded));
  value = SDNV::decode(coded);
  ASSERT_EQ(127, value);
  coded = SDNV::encode(15789);
  ASSERT_EQ(2, SDNV::getLength(coded));
  value = SDNV::decode(coded);
  ASSERT_EQ(15789, value);
  coded = SDNV::encode(45123);
  ASSERT_EQ(3, SDNV::getLength(coded));
  value = SDNV::decode(coded);
  ASSERT_EQ(45123, value);
  coded = SDNV::encode(9123456);
  ASSERT_EQ(4, SDNV::getLength(coded));
  value = SDNV::decode(coded);
  ASSERT_EQ(9123456, value);
  coded = SDNV::encode(4294967295);
  ASSERT_EQ(5, SDNV::getLength(coded));
  value = SDNV::decode(coded);
  ASSERT_EQ(4294967295, value);
}

/**
 * Check the extraction of encoded values.
 * Encoded different values and append all of them
 * into one byte array.
 * If now we extract the same number of values, jumping
 * the length of the codification to get the next value,
 * the decoded value must be the same as the original.
 */
TEST(SDNVTest, EncodeDecodeStringTest) {
  uint64_t values[5] = { 127, 15788, 45321, 9456789, 4123645897 };
  std::stringstream ss;
  for (int i = 0; i < 5; ++i) {
    ss << SDNV::encode(values[i]);
  }
  std::string coded = ss.str();
  for (int i = 0; i < 5; ++i) {
    uint64_t size = SDNV::getLength(coded);
    ASSERT_EQ(SDNV::getLength(values[i]), size);
    coded = coded.substr(size);
  }
}
