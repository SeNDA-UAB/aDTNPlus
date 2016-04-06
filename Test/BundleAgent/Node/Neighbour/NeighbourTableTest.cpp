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
#include "Utils/Table.h"

/**
 * Check the add and remove options.
 * Add a neighbour, check that the neighbour is in, sleep for 2s,
 * clean all the neighbours that have more than 1s of time.
 * Check that we did not loose the neighbour.
 * Get the neighbours again, and check that now the neighbour is out.
 */
TEST(NeighbourTableTest, AddAndRemove) {
  NeighbourTable* nt = new NeighbourTable();
  nt->update(std::make_shared<Neighbour>("node100", "192.168.1.1", 40000));
  // Get the neighbours
  auto neighbours = nt->getValues();
  // Check neighbour
  ASSERT_EQ("node100", nt->getValue("node100")->getId());
  sleep(2);
  // Clean the neighbour
  auto node = nt->getValue("node100");
  nt->clean(1);
  // Check that we still hold the pointer
  ASSERT_EQ("node100", node->getId());
  neighbours.clear();
  neighbours = nt->getValues();
  ASSERT_THROW(nt->getValue("node100"), TableException);
  delete nt;
}

/**
 * Check add and remove with more neighbours.
 * Make the same as the previous test, but with more neighbours.
 */
TEST(NeighbourTableTest, AddAndRemoveMore) {
  NeighbourTable* nt = new NeighbourTable();
  nt->update(std::make_shared<Neighbour>("node100", "192.168.1.1", 40100));
  sleep(1);
  nt->update(std::make_shared<Neighbour>("node101", "192.168.1.1", 40101));
  sleep(1);
  nt->update(std::make_shared<Neighbour>("node102", "192.168.1.1", 40102));
  auto neighbours = nt->getValues();
  ASSERT_EQ(static_cast<size_t>(3), neighbours.size());
  ASSERT_EQ(40100, nt->getValue("node100")->getNodePort());
  ASSERT_EQ(40101, nt->getValue("node101")->getNodePort());
  ASSERT_EQ(40102, nt->getValue("node102")->getNodePort());
  nt->clean(2);
  neighbours.clear();
  neighbours = nt->getValues();
  ASSERT_EQ(static_cast<size_t>(2), neighbours.size());
  ASSERT_EQ(static_cast<uint16_t>(40101),
            nt->getValue("node101")->getNodePort());
  ASSERT_EQ(static_cast<uint16_t>(40102),
            nt->getValue("node102")->getNodePort());
  nt->clean(1);
  neighbours.clear();
  nt->update(std::make_shared<Neighbour>("node102", "192.168.1.1", 40105));
  neighbours = nt->getValues();
  ASSERT_EQ(static_cast<size_t>(1), neighbours.size());
  ASSERT_EQ(static_cast<uint16_t>(40105),
            nt->getValue("node102")->getNodePort());
  neighbours.clear();
  nt->update(std::make_shared<Neighbour>("node102", "192.168.1.102", 40105));
  neighbours = nt->getValues();
  ASSERT_EQ(static_cast<size_t>(1), neighbours.size());
  ASSERT_EQ("192.168.1.102", nt->getValue("node102")->getNodeAddress());
  delete nt;
}
