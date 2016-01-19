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
 * FILE Neighbour.h
 * AUTHOR Blackcatn13
 * DATE Jun 29, 2015
 * VERSION 1
 * This file contains the Neighbour class.
 */
#ifndef BUNDLEAGENT_NODE_NEIGHBOUR_NEIGHBOUR_H_
#define BUNDLEAGENT_NODE_NEIGHBOUR_NEIGHBOUR_H_

#include <string>
#include <cstdint>
#include <chrono>

/**
 * CLASS Neighbour
 * This class contains the information of one neighbour.
 */
class Neighbour {
 public:
  /**
   * @brief Generates a Neighbour with the given values.
   *
   * Generates a Neighbour with a node id, an IP address and a port.
   *
   * @param nodeId the identifier of the neighbour.
   * @param nodeAddress the IP of the neighbour.
   * @param nodePort the port of the neighbour.
   */
  Neighbour(const std::string &nodeId, const std::string &nodeAddress,
            const uint16_t &nodePort);
  /**
   * Destructor of the class.
   */
  virtual ~Neighbour();
  /**
   * @brief Returns the elapsed time since the last activity.
   *
   * This functions calculates the elapsed time since the last activity.
   *
   * @return The elapsed seconds.
   */
  int getElapsedActivityTime();
  /**
   * @brief This functions updates the last activity.
   *
   * This function is called to update the last activity value.
   * @param nodeAddress IP address of the neighbour.
   * @param nodePort port of the neighbour.
   */
  void update(const std::string &nodeAddress, const uint16_t &nodePort);
  /**
   * Equality operator overload.
   * @param neighbour to compare.
   * @return True if the neighbours are equals.
   */
  bool operator==(const Neighbour &neighbour) const;
  /**
   * Gets the node address of the neighbour.
   * @return The node address.
   */
  std::string getNodeAddress();
  /**
   * Gets the node id of the neighbour.
   * @return The node id.
   */
  std::string getNodeId();
  /**
   * Gets the node port of the neighbour.
   * @return The node port.
   */
  uint16_t getNodePort();

 private:
  /**
   * Identifier of the neighbour node.
   */
  std::string m_nodeId;
  /**
   * IP Address of the neighbour node.
   */
  std::string m_nodeAddress;
  /**
   * Port of the neighbour node.
   */
  uint16_t m_nodePort;
  /**
   * Time of the last activity of the neighbour.
   */
  std::chrono::steady_clock::time_point m_lastActivity;
};

#endif  // BUNDLEAGENT_NODE_NEIGHBOUR_NEIGHBOUR_H_
