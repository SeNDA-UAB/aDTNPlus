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
 * FILE NeighbourDiscovery.h
 * AUTHOR Blackcatn13
 * DATE Jun 29, 2015
 * VERSION 1
 * This file contains the NeighbourDiscovery class.
 */
#ifndef BUNDLEAGENT_NODE_NEIGHBOUR_NEIGHBOURDISCOVERY_H_
#define BUNDLEAGENT_NODE_NEIGHBOUR_NEIGHBOURDISCOVERY_H_

#include <string>
#include <cstdint>
#include <thread>
#include <atomic>
#include <memory>
#include "Node/Config.h"
#include "Node/Neighbour/NeighbourTable.h"

/**
 * CLASS NeighbourDiscovery
 * This class implements the neighbour discovery system.
 * It spawns 3 threads, one to send beacons, another to receive beacons and
 * the last one to clean the disappeared neighbours.
 *
 * This class use the following parameters (all of them are in the adtn.ini file
 * under the NeighbourDiscovery section) :
 *
 * discoveryAddress : address to send the beacons.
 *
 * discoveryPort : port to send the beacons.
 *
 * discoveryPeriod : time between beacon send.
 *
 * neighbourExpirationTime : time to consider a neighbour has expired.
 *
 * neighbourCleanerTime : seconds to wait between calls to neighbour cleaner.
 */
class NeighbourDiscovery {
 public:
  /**
   * @brief Constructor
   *
   * This will create a NeighbourDiscovery object using the values in the
   * ConfigLoader.
   * This NeighbourDiscovery will spawn the needed threads to run the discovery
   * process.
   *
   * @param config ConfigLoader with the paramenters.
   */
  explicit NeighbourDiscovery(Config config,
                              std::shared_ptr<NeighbourTable> neighbourTable);
  /**
   * Destructor of the class.
   */
  virtual ~NeighbourDiscovery();

 protected:
  /**
   * Function to receive beacons.
   */
  virtual void receiveBeacons();
  /**
   * ConfigLoader initialised.
   */
  Config m_config;
  /**
   * The neighbour table reference.
   */
  std::shared_ptr<NeighbourTable> m_neighbourTable;

 private:
  /**
   * Function to send beacons.
   */
  void sendBeacons();

  /**
   * Function to clean neighbours.
   */
  void cleanNeighbours();
};

#endif  // BUNDLEAGENT_NODE_NEIGHBOUR_NEIGHBOURDISCOVERY_H_
