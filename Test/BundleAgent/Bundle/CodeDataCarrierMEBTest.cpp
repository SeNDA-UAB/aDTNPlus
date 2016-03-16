/*
 * Copyright (c) 2016 SeNDA
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
 * FILE NewMEBTest.cpp
 * AUTHOR clacambra
 * DATE 11/03/2016
 * VERSION 1
 * This file contains the test of the CodeDataCarrierMEB class.
 */

#include <string>
#include <iostream>
#include "gtest/gtest.h"
#include "Bundle/BundleTypes.h"
#include "Bundle/CodeDataCarrierMEB.h"

/*
 *
 */
TEST(CodeDataCarrierMEBTest, FilledConstructor) {
  uint8_t metadata_type =
      static_cast<uint8_t>(MetadataTypes::CODE_DATA_CARRIER_MEB);
  std::string code = "Some code";
  std::string data = "Some data";
  std::string metadata = std::to_string(code.length()) +
      code + data;
  uint16_t codeLength = static_cast<uint16_t>(
      std::stoi(std::to_string(code.length())));

  CodeDataCarrierMEB nm = CodeDataCarrierMEB(code, data);
  ASSERT_EQ(metadata_type, nm.getMetadataType());
  ASSERT_EQ(metadata, nm.getMetadata());
  ASSERT_EQ(codeLength, nm.getCodeLength());
  ASSERT_EQ(code, nm.getCode());
  ASSERT_EQ(data, nm.getData());
}

TEST(CodeDataCarrierMEBTest, RawConstructor) {
  uint8_t metadata_type =
      static_cast<uint8_t>(MetadataTypes::CODE_DATA_CARRIER_MEB);
  std::string code = "Some code";
  std::string data = "Some data";
  std::string metadata = std::to_string(code.length()) +
      code + data;
  uint16_t codeLength = static_cast<uint16_t>(
      std::stoi(std::to_string(code.length())));

  CodeDataCarrierMEB nm = CodeDataCarrierMEB(code, data);
  std::string rawData = nm.toRaw();
  CodeDataCarrierMEB nm2 = CodeDataCarrierMEB(rawData);
  ASSERT_EQ(metadata_type, nm2.getMetadataType());
  ASSERT_EQ(nm2.getMetadataType(), nm.getMetadataType());
  ASSERT_EQ(metadata, nm2.getMetadata());
  ASSERT_EQ(nm2.getMetadata(), nm.getMetadata());
  ASSERT_EQ(codeLength, nm2.getCodeLength());
  ASSERT_EQ(nm2.getCodeLength(), nm.getCodeLength());
  ASSERT_EQ(code, nm2.getCode());
  ASSERT_EQ(nm2.getCode(), nm.getCode());
  ASSERT_EQ(data, nm2.getData());
  ASSERT_EQ(nm2.getData(), nm.getData());
}


