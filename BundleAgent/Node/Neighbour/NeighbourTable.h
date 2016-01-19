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
 * FILE NeighbourTable.h
 * AUTHOR Blackcatn13
 * DATE Jun 29, 2015
 * VERSION 1
 * This file contains the NeighbourTable class.
 */
#ifndef BUNDLEAGENT_NODE_NEIGHBOUR_NEIGHBOURTABLE_H_
#define BUNDLEAGENT_NODE_NEIGHBOUR_NEIGHBOURTABLE_H_

#include <map>
#include <string>
#include <cstdint>
#include <mutex>
#include <memory>
#include <vector>
#include <stdexcept>

class Neighbour;

class NeighbourTableException : public std::runtime_error {
 public:
  explicit NeighbourTableException(const std::string &what)
      : runtime_error(what) {
  }
};

/**
 * CLASS NeighbourTable
 * This class contains all the neighbours.
 */
class NeighbourTable {
 public:
  /**
   * Constructor of the NeighbourTable.
   */
  NeighbourTable();
  /**
   * Destructor of the class.
   */
  virtual ~NeighbourTable();
  /**
   * @brief Updates the neighbour.
   *
   * This function updates a neighbour.
   * If the neighbour is not present in the table this function adds the new
   * neighbour.
   *
   * @param nodeId identifier of the node.
   * @param nodeAddress IP address of the node.
   * @param nodePort port of the node.
   */
  void update(const std::string &nodeId, const std::string &nodeAddress,
              const uint16_t &nodePort);
  /**
   * @brief Clean all the neighbours that have expired.
   *
   * This function deletes all the neighbours that have been expired.
   * This means all the neighbours that have a last activity value greater than
   * the expirationTime.
   *
   * @param expirationTime Minimum time to expire a neighbour.
   */
  void clean(int expirationTime);
  /**
   * Function to get a copy of the current neighbours list id's.
   *
   * @return a vector with the current neighbours id's.
   */
  std::vector<std::string> getNeighbours();

  /**
   * Function to get the information of the given neighbour.
   *
   * @param nodeId the node id of the neighbour.
   * @return a Neighbour pointer if exists, else throws a NeighbourTableException.
   */
  std::shared_ptr<Neighbour> getNeighbour(const std::string &nodeId);

 private:
  /**
   * Map with the neighbours.
   */
  std::map<std::string, std::shared_ptr<Neighbour>> m_neighbours;
  /**
   * Mutex for the map.
   */
  std::mutex mutex;
};

#endif  // BUNDLEAGENT_NODE_NEIGHBOUR_NEIGHBOURTABLE_H_
