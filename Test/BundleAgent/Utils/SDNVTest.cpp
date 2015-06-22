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
  ASSERT_EQ(1, getLength(127));
  ASSERT_EQ(2, getLength(15789));
  ASSERT_EQ(3, getLength(45123));
  ASSERT_EQ(4, getLength(9123456));
  ASSERT_EQ(5, getLength(4294967295));
}

/**
 * Check the length of an encoded value.
 * The length of an encoded value must be the same that
 * the origin value.
 */
TEST(SDNVTest, EncodeTest) {
  for (uint64_t i = 1; i <= (2 ^ 32); i += (2 ^ 7)) {
    std::string coded = encode(i);
    ASSERT_EQ(coded.length(), getLength(i));
  }
}

/**
 * Check the decode function.
 * Encode a value first and decode it after,
 * the decoded value must be the same as the encoded.
 */
TEST(SDNVTest, EncodeDecodeTest) {
  for (uint64_t i = 1; i <= (2 ^ 32); i += (2 ^ 7)) {
    std::string coded = encode(i);
    ASSERT_EQ(coded.length(), getLength(i));
    uint64_t value = decode(coded);
    ASSERT_EQ(i, value);
  }
}

/**
 * Check the length of an encoded value inside a byte array.
 * The length of the encoded value must be the same as the
 * length of the original value.
 */
TEST(SDNVTest, LengthStringTest) {
  std::string coded = encode(127);
  ASSERT_EQ(1, getLength(coded));
  coded = encode(15789);
  ASSERT_EQ(2, getLength(coded));
  coded = encode(45123);
  ASSERT_EQ(3, getLength(coded));
  coded = encode(9123456);
  ASSERT_EQ(4, getLength(coded));
  coded = encode(4294967295);
  ASSERT_EQ(5, getLength(coded));
}

/**
 * Check encode and decode of large numbers.
 * The length of the encoded value, and the decoded value
 * must be equals to the original values.
 */
TEST(SDNVTest, EncodeDecodeLargeTest) {
  std::string coded;
  uint64_t value;
  coded = encode(127);
  ASSERT_EQ(1, getLength(coded));
  value = decode(coded);
  ASSERT_EQ(127, value);
  coded = encode(15789);
  ASSERT_EQ(2, getLength(coded));
  value = decode(coded);
  ASSERT_EQ(15789, value);
  coded = encode(45123);
  ASSERT_EQ(3, getLength(coded));
  value = decode(coded);
  ASSERT_EQ(45123, value);
  coded = encode(9123456);
  ASSERT_EQ(4, getLength(coded));
  value = decode(coded);
  ASSERT_EQ(9123456, value);
  coded = encode(4294967295);
  ASSERT_EQ(5, getLength(coded));
  value = decode(coded);
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
    ss << encode(values[i]);
  }
  std::string coded = ss.str();
  for (int i = 0; i < 5; ++i) {
    uint64_t size = getLength(coded);
    ASSERT_EQ(getLength(values[i]), size);
    coded = coded.substr(size);
  }
}
