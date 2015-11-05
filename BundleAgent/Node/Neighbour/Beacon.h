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
 * FILE Beacon.h
 * AUTHOR Blackcatn13
 * DATE Jun 29, 2015
 * VERSION 1
 * This file contains the Beacon class.
 */
#ifndef BUNDLEAGENT_NODE_NEIGHBOUR_BEACON_H_
#define BUNDLEAGENT_NODE_NEIGHBOUR_BEACON_H_

#include <string>
#include <cstdint>

/**
 * CLASS Beacon
 * This class represents a beacon, this beacon is used into the neighbour
 * discovery process.
 */
class Beacon {
 public:
  /**
   * @brief Raw Constructor.
   *
   * Generates a Beacon from a raw data.
   *
   * @param rawData containing the beacon.
   */
  explicit Beacon(std::string rawData);
  /**
   * @brief Constructs a Beacon with the provided information.
   *
   * To create a new Beacon it's needed the node ID,
   * the IP Address of the node, and the port.
   *
   * @param nodeId identifier of the node.
   * @param nodeAddress IP address of the node.
   * @param nodePort port of the node.
   */
  Beacon(const std::string &nodeId, const std::string &nodeAddress,
         const uint16_t &nodePort);
  /**
   * Destructor of the class.
   */
  virtual ~Beacon();
  /**
   * @brief Function to get the beacon in raw format.
   *
   * This function provides the raw version of the current beacon.
   *
   * @return the beacon in raw format.
   */
  std::string getRaw();

  /**
   * Max size of a beacon.
   * This is taking the max size of the nodeId of 1023 bytes, and the size
   * of the ip address and the port.
   */
  static const int MAX_BEACON_SIZE = 1050;

  std::string getNodeAddress();

  std::string getNodeId();

  uint16_t getNodePort();


private:
  /**
   * Byte array containing the beacon as a raw.
   */
  std::string m_raw;
  /**
   * Identifier of the node.
   */
  std::string m_nodeId;
  /**
   * IP address of the node.
   */
  std::string m_nodeAddress;
  /**
   * Port of the node.
   */
  uint16_t m_nodePort;
};

#endif  // BUNDLEAGENT_NODE_NEIGHBOUR_BEACON_H_
