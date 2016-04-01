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
#include <vector>
#include <iostream>
#include "gtest/gtest.h"
#include "Bundle/BundleTypes.h"
#include "Bundle/CodeDataCarrierMEB.h"
#include "ExternTools/json/json.hpp"

TEST(CodeDataCarrierMEBTest, FilledConstructor) {
  uint8_t metadata_type =
      static_cast<uint8_t>(MetadataTypes::CODE_DATA_CARRIER_MEB);
  std::string code = "Some code";
  std::string data = "{\"neighbour\" : [\"node1\", \"node2\"],"
      "\"from\" : \"node1\"}";
  std::string metadata = std::to_string(code.length()) +
      code + data;
  uint16_t codeLength = static_cast<uint16_t>(
      std::stoi(std::to_string(code.length())));

  CodeDataCarrierMEB nm = CodeDataCarrierMEB(code, data);
  ASSERT_EQ(metadata_type, nm.getMetadataType());
  ASSERT_EQ(metadata, nm.getMetadata());
  ASSERT_EQ(codeLength, nm.getCodeLength());
  ASSERT_EQ(code, nm.getCode());
  ASSERT_EQ(nlohmann::json::parse(data), nm.getData());
}

TEST(CodeDataCarrierMEBTest, RawConstructor) {
  uint8_t metadata_type =
      static_cast<uint8_t>(MetadataTypes::CODE_DATA_CARRIER_MEB);
  std::string code = "Some code";
  std::string data = "{\"from\":\"node1\","
        "\"neighbour\":[\"node1\",\"node2\"]}";
  std::vector<std::string> neighbours{"node1", "node2"};
  std::string from("node1");
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
  ASSERT_EQ(nlohmann::json::parse(data), nm2.getData());
  ASSERT_EQ(nm2.getData(), nm.getData());
  nlohmann::json json = nm.getData();
  nlohmann::json json2 = nm2.getData();
  std::vector<std::string> v = json["neighbour"];
  std::vector<std::string> v1 = json2["neighbour"];
  ASSERT_EQ(neighbours, v);
  ASSERT_EQ(v, v1);
  ASSERT_EQ(from, json["from"]);
  ASSERT_EQ(json["from"], json2["from"]);
}

TEST(CodeDataCarrierMEBTest, Setters) {
  std::string code = "Some code";
  std::string data = "{\"from\" : \"node1\", "
      "\"neighbour\" : [\"node1\", \"node2\"]}";
  std::string from("node1");

  CodeDataCarrierMEB nm = CodeDataCarrierMEB(code, data);
  nm.setData("{\"from\" : \"node3\", "
      "\"neighbour\" : [\"node3\", \"node4\"]}");
  std::string raw = nm.toRaw();
  CodeDataCarrierMEB nm2 = CodeDataCarrierMEB(raw);
  ASSERT_EQ("Some code", nm2.getCode());
  ASSERT_EQ(nm2.getCodeLength(), 9);
  ASSERT_EQ(nlohmann::json::parse("{\"from\":\"node3\","
      "\"neighbour\":[\"node3\",\"node4\"]}"), nm2.getData());
}

TEST(CodeDataCarrierMEBTest, JSONException) {
  std::string code = "Some code";
  std::string data = "{\"from\" : \"node1\", "
        "\"neighbour : [\"node1\", \"node2\"]}";
  ASSERT_THROW(CodeDataCarrierMEB(code, data), JSONException);
  std::string data2 = "{\"from\" : \"node1\", "
          "\"neighbour\" : [\"node1\", \"node2\"]}";
  CodeDataCarrierMEB nm = CodeDataCarrierMEB(code, data2);
  ASSERT_THROW(nm.setData(data), JSONException);
}


