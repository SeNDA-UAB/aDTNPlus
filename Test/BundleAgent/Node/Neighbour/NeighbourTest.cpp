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
 * FILE NeighbourTest.cpp
 * AUTHOR Blackcatn13
 * DATE Jun 29, 2015
 * VERSION 1
 * This file contains the tests of the NeighbourTest class.
 */

#include "Node/Neighbour/Neighbour.h"
#include "gtest/gtest.h"

/**
 * Check the neighbour constructor.
 * Generate a neighbour and check the fields.
 */
TEST(NeighbourTest, Constructor) {
  Neighbour n = Neighbour("node100", "192.168.1.2", 40000);
  ASSERT_EQ("node100", n.m_nodeId);
  ASSERT_EQ("192.168.1.2", n.m_nodeAddress);
  ASSERT_EQ(40000, n.m_nodePort);
}

/**
 * Check the activity time.
 * Create the neighbour and sleep, the activity time must be the same.
 * After that update the neighbour and check that the activity time is 0.
 */
TEST(NeighbourTest, Activity) {
  Neighbour n = Neighbour("node100", "192.168.1.2", 40000);
  ASSERT_EQ(0, n.getElapsedActivityTime());
  sleep(2);
  ASSERT_EQ(2, n.getElapsedActivityTime());
  n.Update();
  ASSERT_EQ(0, n.getElapsedActivityTime());
}

/**
 * Check equality.
 * Check if two nodes are equals.
 */
TEST(NeighbourTest, Equality) {
  Neighbour n = Neighbour("node100", "192.168.1.2", 40000);
  Neighbour n1 = Neighbour("node100", "192.168.1.2", 40000);
  ASSERT_TRUE(n1 == n);
  n.m_nodeId = "node101";
  ASSERT_FALSE(n1 == n);
}
