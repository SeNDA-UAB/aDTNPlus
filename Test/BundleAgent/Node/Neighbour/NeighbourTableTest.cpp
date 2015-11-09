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
 * FILE NeighbourTable.cpp
 * AUTHOR Blackcatn13
 * DATE Jun 29, 2015
 * VERSION 1
 * This file contains the tests of the NeigbourTable class.
 */

#include <string>
#include <map>
#include <memory>
#include <vector>
#include "Node/Neighbour/NeighbourTable.h"
#include "gtest/gtest.h"
#include "Node/Neighbour/Neighbour.h"

/**
 * Check the add and remove options.
 * Add a neighbour, check that the neighbour is in, sleep for 2s,
 * clean all the neighbours that have more than 1s of time.
 * Check that we did not loose the neighbour.
 * Get the neighbours again, and check that now the neighbour is out.
 */
TEST(NeighbourTableTest, AddAndRemove) {
  NeighbourTable* nt = NeighbourTable::getInstance();
  nt->update("node100", "192.168.1.1", 40000);
  // Get the neighbours
  auto neighbours = nt->getNeighbours();
  // Check neighbour
  ASSERT_EQ("node100", nt->getNeighbour("node100")->getNodeId());
  sleep(2);
  // Clean the neighbour
  auto node = nt->getNeighbour("node100");
  NeighbourTable::getInstance()->cleanNeighbours(1);
  // Check that we still hold the pointer
  ASSERT_EQ("node100", node->getNodeId());
  neighbours.clear();
  neighbours = nt->getNeighbours();
  ASSERT_THROW(nt->getNeighbour("node100"), NeighbourTableException);
}

/**
 * Check add and remove with more neighbours.
 * Make the same as the previous test, but with more neighbours.
 */
TEST(NeighbourTableTest, AddAndRemoveMore) {
  NeighbourTable* nt = NeighbourTable::getInstance();
  nt->update("node100", "192.168.1.1", 40100);
  sleep(1);
  nt->update("node101", "192.168.1.1", 40101);
  sleep(1);
  nt->update("node102", "192.168.1.1", 40102);
  auto neighbours = nt->getNeighbours();
  ASSERT_EQ(3, neighbours.size());
  ASSERT_EQ(40100, nt->getNeighbour("node100")->getNodePort());
  ASSERT_EQ(40101, nt->getNeighbour("node101")->getNodePort());
  ASSERT_EQ(40102, nt->getNeighbour("node102")->getNodePort());
  NeighbourTable::getInstance()->cleanNeighbours(2);
  neighbours.clear();
  neighbours = nt->getNeighbours();
  ASSERT_EQ(2, neighbours.size());
  ASSERT_EQ(40101, nt->getNeighbour("node101")->getNodePort());
  ASSERT_EQ(40102, nt->getNeighbour("node102")->getNodePort());
  NeighbourTable::getInstance()->cleanNeighbours(1);
  neighbours.clear();
  nt->update("node102", "192.168.1.1", 40105);
  neighbours = nt->getNeighbours();
  ASSERT_EQ(1, neighbours.size());
  ASSERT_EQ(40105, nt->getNeighbour("node102")->getNodePort());
  neighbours.clear();
  nt->update("node102", "192.168.1.102", 40105);
  neighbours = nt->getNeighbours();
  ASSERT_EQ(1, neighbours.size());
  ASSERT_EQ("192.168.1.102", nt->getNeighbour("node102")->getNodeAddress());
}
