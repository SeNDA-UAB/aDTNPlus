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
 * FILE BeaconTest.cpp
 * AUTHOR Blackcatn13
 * DATE Jun 29, 2015
 * VERSION 1
 * This file contains the tests of the Beacon class.
 */

#include <string>
#include <vector>
#include "Node/Neighbour/Beacon.h"
#include "gtest/gtest.h"

/**
 * Check the constructor.
 * Generate a Beacon and check that all the fields are the same.
 */
TEST(BeaconTest, GenerateBeacon) {
  Beacon b = Beacon("Node001", "192.168.1.2", 40000,
                    std::vector<std::string>());
  ASSERT_EQ("Node001", b.getNodeId());
  ASSERT_EQ("192.168.1.2", b.getNodeAddress());
  ASSERT_EQ(40000, b.getNodePort());
}

/**
 * Check the raw functions.
 * Create a beacon, convert it to raw data. Generate a new bundle from the
 * raw, the fields must be the same.
 */
TEST(BeaconTest, RawFunctions) {
  std::vector<std::string> endpoints = { "e1", "e2", "e3" };
  Beacon b = Beacon("Node001", "192.168.1.2", 40000, endpoints);
  std::string raw = b.getRaw();
  Beacon b1 = Beacon(raw);
  ASSERT_EQ(b.getNodeId(), b1.getNodeId());
  ASSERT_EQ(b.getNodeAddress(), b1.getNodeAddress());
  ASSERT_EQ(b.getNodePort(), b1.getNodePort());
  ASSERT_EQ(b.getEndpoints(), b1.getEndpoints());
}

/**
 * Check the raw functions with changes.
 * Create a beacon and test it as the previous test did.
 * After that change some fields and do the same.
 */
TEST(BeaconTest, RawFunctionsWithChange) {
  std::vector<std::string> endpoints = { "e1", "e2", "e3" };
  Beacon b = Beacon("Node001", "192.168.1.2", 40000, endpoints);
  std::string raw = b.getRaw();
  Beacon b1 = Beacon(raw);
  ASSERT_EQ(b.getNodeId(), b1.getNodeId());
  ASSERT_EQ(b.getNodeAddress(), b1.getNodeAddress());
  ASSERT_EQ(b.getNodePort(), b1.getNodePort());
  ASSERT_EQ(b.getEndpoints(), b1.getEndpoints());
  endpoints = {"e3", "e4", "this"};
  b1 = Beacon("Node002", "192.168.1.2", 5000, endpoints);
  raw = b1.getRaw();
  b = Beacon(raw);
  ASSERT_EQ(b1.getNodeId(), b.getNodeId());
  ASSERT_EQ(b1.getNodeAddress(), b.getNodeAddress());
  ASSERT_EQ(b1.getNodePort(), b.getNodePort());
  ASSERT_EQ(b1.getEndpoints(), b.getEndpoints());
}
