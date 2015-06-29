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
#include "Node/Neighbour/Beacon.h"
#include "gtest/gtest.h"

TEST(BeaconTest, GenerateBeacon) {
  Beacon b = Beacon("Node001", "192.168.1.2", 40000);
  ASSERT_EQ("Node001", b.m_nodeId);
  ASSERT_EQ("192.168.1.2", b.m_nodeAddress);
  ASSERT_EQ(40000, b.m_nodePort);
}

TEST(BeaconTest, RawFunctions) {
  Beacon b = Beacon("Node001", "192.168.1.2", 40000);
  std::string raw = b.getRaw();
  Beacon b1 = Beacon(raw);
  ASSERT_EQ(b.m_nodeId, b1.m_nodeId);
  ASSERT_EQ(b.m_nodeAddress, b1.m_nodeAddress);
  ASSERT_EQ(b.m_nodePort, b1.m_nodePort);
}

TEST(BeaconTest, RawFunctionsWithChange) {
  Beacon b = Beacon("Node001", "192.168.1.2", 40000);
  std::string raw = b.getRaw();
  Beacon b1 = Beacon(raw);
  ASSERT_EQ(b.m_nodeId, b1.m_nodeId);
  ASSERT_EQ(b.m_nodeAddress, b1.m_nodeAddress);
  ASSERT_EQ(b.m_nodePort, b1.m_nodePort);
  b1.m_nodeId = "Node002";
  b1.m_nodePort = 50000;
  raw = b1.getRaw();
  b = Beacon(raw);
  ASSERT_EQ(b1.m_nodeId, b.m_nodeId);
  ASSERT_EQ(b1.m_nodeAddress, b.m_nodeAddress);
  ASSERT_EQ(b1.m_nodePort, b.m_nodePort);
}
