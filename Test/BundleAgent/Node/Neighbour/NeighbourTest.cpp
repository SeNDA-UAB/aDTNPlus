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
 * This file contains the test of the NeighbourTest class.
 */

#include "Node/Neighbour/Neighbour.h"
#include "gtest/gtest.h"

TEST(NeighbourTest, Constructor) {
  Neighbour n = Neighbour("node100", "192.168.1.2", 40000);
  ASSERT_EQ("node100", n.m_nodeId);
  ASSERT_EQ("192.168.1.2", n.m_nodeAddress);
  ASSERT_EQ(40000, n.m_nodePort);
}

TEST(NeighbourTest, Activity) {
  Neighbour n = Neighbour("node100", "192.168.1.2", 40000);
  ASSERT_EQ(0, n.getElapsedActivityTime());
  sleep(2);
  ASSERT_EQ(2, n.getElapsedActivityTime());
  n.Update();
  ASSERT_EQ(0, n.getElapsedActivityTime());
}
