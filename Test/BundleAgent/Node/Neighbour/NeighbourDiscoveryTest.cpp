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
 * FILE NeighbourDiscoveryTest.cpp
 * AUTHOR Blackcatn13
 * DATE Jun 30, 2015
 * VERSION 1
 * This file contains the NeighbourDiscovery tests.
 */

#include <map>
#include <string>
#include <memory>
#include "Node/Neighbour/NeighbourDiscovery.h"
#include "gtest/gtest.h"
#include "Node/Neighbour/NeighbourTable.h"
#include "Node/Neighbour/Neighbour.h"
#include "Node/ConfigLoader.h"

/**
 * Check NeighbourCleaner thread.
 * Create the neighbour discovery object, add some neighbours and let the
 * cleaner delete them.
 * Stop the cleaner and check that it has really stopped.
 */
TEST(NeighbourDiscoveryTest, NeighbourCleanerTest) {
  ConfigLoader cf = ConfigLoader();
  cf.load("../BundleAgent/Config/adtn.ini");
  // clear the Nighbour table to ensure test values.
  sleep(1);
  NeighbourTable::getInstance()->cleanNeighbours(1);
  NeighbourDiscovery nd(cf);
  NeighbourTable::getInstance()->update("node100", "192.168.1.1", 4000);
  std::map<std::string, std::shared_ptr<Neighbour>> neighbours = std::map<
      std::string, std::shared_ptr<Neighbour>>();
  NeighbourTable::getInstance()->getNeighbours(&neighbours);
  ASSERT_EQ(1, neighbours.size());
  NeighbourTable::getInstance()->update("node101", "192.168.1.1", 4000);
  neighbours.clear();
  NeighbourTable::getInstance()->getNeighbours(&neighbours);
  ASSERT_EQ(2, neighbours.size());
  sleep(3);
  neighbours.clear();
  NeighbourTable::getInstance()->getNeighbours(&neighbours);
  ASSERT_EQ(2, neighbours.size());
  NeighbourTable::getInstance()->update("node101", "192.168.1.1", 4000);
  sleep(2);
  neighbours.clear();
  NeighbourTable::getInstance()->getNeighbours(&neighbours);
  ASSERT_EQ(1, neighbours.size());
  ASSERT_EQ("node101", neighbours["node101"]->m_nodeId);
  sleep(5);
  neighbours.clear();
  NeighbourTable::getInstance()->getNeighbours(&neighbours);
  ASSERT_EQ(0, neighbours.size());
  nd.stop();
  // The neighbour cleaner thread has been stopped, so the new neighbours
  // must not be cleaned.
  NeighbourTable::getInstance()->update("node101", "192.168.1.1", 4000);
  sleep(5);
  neighbours.clear();
  NeighbourTable::getInstance()->getNeighbours(&neighbours);
  ASSERT_EQ(1, neighbours.size());
}
