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
#include "Node/Config.h"
#include "Utils/globals.h"

/**
 * Check NeighbourCleaner thread.
 * Create the neighbour discovery object, add some neighbours and let the
 * cleaner delete them.
 * Stop the cleaner and check that it has really stopped.
 */
TEST(NeighbourDiscoveryTest, NeighbourCleanerTest) {
  g_stop = false;
  Config cf = Config("../BundleAgent/Config/adtn.ini");
  // clear the Nighbour table to ensure test values.
  sleep(1);
  NeighbourTable::getInstance()->cleanNeighbours(1);
  NeighbourDiscovery nd(cf);
  NT->update("node100", "192.168.1.1", 4000);
  auto neighbours = NT->getNeighbours();
  ASSERT_EQ(1, neighbours.size());
  NT->update("node101", "192.168.1.1", 4000);
  neighbours.clear();
  neighbours = NT->getNeighbours();
  ASSERT_EQ(2, neighbours.size());
  sleep(3);
  neighbours.clear();
  neighbours = NT->getNeighbours();
  ASSERT_EQ(2, neighbours.size());
  NeighbourTable::getInstance()->update("node101", "192.168.1.1", 4000);
  sleep(2);
  neighbours.clear();
  neighbours = NT->getNeighbours();
  ASSERT_EQ(1, neighbours.size());
  ASSERT_EQ("node101", NT->getNeighbour("node101")->getNodeId());
  sleep(5);
  neighbours.clear();
  neighbours = NT->getNeighbours();
  ASSERT_EQ(0, neighbours.size());
  g_stop = true;
  // The neighbour cleaner thread has been stopped, so the new neighbours
  // must not be cleaned.
  NeighbourTable::getInstance()->update("node101", "192.168.1.1", 4000);
  sleep(5);
  neighbours.clear();
  neighbours = NT->getNeighbours();
  ASSERT_EQ(1, neighbours.size());
}

/**
 * Check if the sender and receiver threads are working correctly.
 * To do this, we start the neighbour discovery, and put in test mode.
 * With this we are going to have a neighbour, ourselves.
 */
TEST(NeighbourDiscoveryTest, NeighbourSendAndReceiveTest) {
  g_stop = false;
  Config cf = Config("../BundleAgent/Config/adtn.ini");
  // clear the Nighbour table to ensure test values.
  sleep(1);
  NeighbourTable::getInstance()->cleanNeighbours(1);
  NeighbourDiscovery nd(cf);
  nd.setTestMode(true);
  sleep(3);
  auto neighbours = NT->getNeighbours();
  ASSERT_EQ(1, neighbours.size());
  ASSERT_EQ("node1", NT->getNeighbour(*neighbours.begin())->getNodeId());
  ASSERT_EQ("127.0.0.1",
            NT->getNeighbour(*neighbours.begin())->getNodeAddress());
  ASSERT_EQ(40000, NT->getNeighbour(*neighbours.begin())->getNodePort());
  g_stop = true;
}
