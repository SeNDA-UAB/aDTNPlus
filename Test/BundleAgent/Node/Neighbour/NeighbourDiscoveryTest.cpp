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
#include <fstream>
#include <memory>
#include <vector>
#include "Node/Neighbour/NeighbourDiscovery.h"
#include "gtest/gtest.h"
#include "Node/Neighbour/NeighbourTable.h"
#include "Node/Neighbour/Neighbour.h"
#include "Node/Config.h"
#include "Utils/globals.h"
#include "Node/EndpointListener/ListeningEndpointsTable.h"
#include "Node/EndpointListener/Endpoint.h"

/**
 * Check NeighbourCleaner thread.
 * Create the neighbour discovery object, add some neighbours and let the
 * cleaner delete them.
 * Stop the cleaner and check that it has really stopped.
 */
TEST(NeighbourDiscoveryTest, NeighbourCleanerTest) {
  g_stop = false;
  std::ofstream ss;
  ss.open("adtn.ini");
  ss << "[Node]" << std::endl << "nodeId : node1" << std::endl
     << "nodeAddress : 127.0.0.1" << std::endl << "nodePort : 40000"
     << std::endl << "[NeighbourDiscovery]" << std::endl
     << "discoveryAddress : 239.100.100.100" << std::endl
     << "discoveryPort : 40001" << std::endl << "discoveryPeriod : 2"
     << std::endl << "neighbourExpirationTime : 4" << std::endl
     << "neighbourCleanerTime : 2" << std::endl << "testMode : false"
     << std::endl << "[Logger]" << std::endl << "filename : /tmp/adtn.log"
     << std::endl << "level : 100" << std::endl << "[Constants]" << std::endl
     << "timeout : 3" << std::endl << "[BundleProcess]" << std::endl
     << "dataPath : /tmp/.adtn/" << std::endl;
  ss.close();
  Config cf = Config("adtn.ini");
  std::shared_ptr<NeighbourTable> nt = std::shared_ptr<NeighbourTable>(
      new NeighbourTable());
  std::shared_ptr<ListeningEndpointsTable> let = std::shared_ptr<
      ListeningEndpointsTable>(new ListeningEndpointsTable());
  NeighbourDiscovery nd(cf, nt, let);
  nt->update(
      std::make_shared<Neighbour>("node100", "192.168.1.1", 4000,
                                  std::vector<std::string>( { "e1" })));
  auto neighbours = nt->getConnectedEID();
  ASSERT_EQ(static_cast<size_t>(2), neighbours.size());
  nt->update(
      std::make_shared<Neighbour>("node101", "192.168.1.1", 4000,
                                  std::vector<std::string>( { "e2" })));
  neighbours.clear();
  neighbours = nt->getConnectedEID();
  ASSERT_EQ(static_cast<size_t>(4), neighbours.size());
  sleep(3);
  neighbours.clear();
  neighbours = nt->getConnectedEID();
  ASSERT_EQ(static_cast<size_t>(4), neighbours.size());
  nt->update(
      std::make_shared<Neighbour>("node101", "192.168.1.1", 4000,
                                  std::vector<std::string>( { "e2" })));
  sleep(2);
  neighbours.clear();
  neighbours = nt->getConnectedEID();
  ASSERT_EQ(static_cast<size_t>(2), neighbours.size());
  ASSERT_EQ("node101", nt->getValue("node101")->getId());
  sleep(5);
  neighbours.clear();
  neighbours = nt->getConnectedEID();
  ASSERT_EQ(static_cast<size_t>(0), neighbours.size());
  g_stop = true;
  // The neighbour cleaner thread has been stopped, so the new neighbours
  // must not be cleaned.
  nt->update(
      std::make_shared<Neighbour>("node101", "192.168.1.1", 4000,
                                  std::vector<std::string>( { "e2" })));
  sleep(5);
  neighbours.clear();
  neighbours = nt->getConnectedEID();
  ASSERT_EQ(static_cast<size_t>(2), neighbours.size());
}

/**
 * Check if the sender and receiver threads are working correctly.
 * To do this, we start the neighbour discovery, and put in test mode.
 * With this we are going to have a neighbour, ourselves.
 */
TEST(NeighbourDiscoveryTest, NeighbourSendAndReceiveTest) {
  g_stop = false;
  std::ofstream ss;
  ss.open("adtn1.ini");
  ss << "[Node]" << std::endl << "nodeId : node1" << std::endl
     << "nodeAddress : 127.0.0.1" << std::endl << "nodePort : 40000"
     << std::endl << "[NeighbourDiscovery]" << std::endl
     << "discoveryAddress : 239.100.100.100" << std::endl
     << "discoveryPort : 40001" << std::endl << "discoveryPeriod : 2"
     << std::endl << "neighbourExpirationTime : 4" << std::endl
     << "neighbourCleanerTime : 2" << std::endl << "testMode : true"
     << std::endl << "[Logger]" << std::endl << "filename : /tmp/adtn.log"
     << std::endl << "level : 100" << std::endl << "[Constants]" << std::endl
     << "timeout : 3" << std::endl << "[BundleProcess]" << std::endl
     << "dataPath : /tmp/.adtn/" << std::endl;
  ss.close();

  Config cf = Config("adtn1.ini");
  // clear the Nighbour table to ensure test values.
  sleep(1);
  std::shared_ptr<NeighbourTable> nt = std::shared_ptr<NeighbourTable>(
      new NeighbourTable());
  std::shared_ptr<ListeningEndpointsTable> let = std::shared_ptr<
      ListeningEndpointsTable>(new ListeningEndpointsTable());
  NeighbourDiscovery nd(cf, nt, let);
  sleep(3);
  ASSERT_NO_THROW(nt->getValue("node1"));
  auto neighbour = nt->getValue("node1");
  ASSERT_EQ("node1", neighbour->getId());
  ASSERT_EQ("127.0.0.1", neighbour->getNodeAddress());
  ASSERT_EQ(static_cast<uint16_t>(40000), neighbour->getNodePort());
  // Add an endpoint, an check if its in the neighbour table.
  let->update("This", std::make_shared<Endpoint>("This", "127.0.0.1", 50, 2));
  sleep(3);
  ASSERT_NO_THROW(nt->getValue("node1"));
  neighbour = nt->getValue("node1");
  ASSERT_EQ("node1", neighbour->getId());
  ASSERT_EQ("127.0.0.1", neighbour->getNodeAddress());
  ASSERT_EQ(static_cast<uint16_t>(40000), neighbour->getNodePort());
  ASSERT_EQ(std::vector<std::string>({"This"}), neighbour->getEndpoints());
  g_stop = true;
  sleep(5);
}
