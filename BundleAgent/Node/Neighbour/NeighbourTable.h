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

class Neighbour;

/**
 * CLASS NeighbourTable
 * This class is singleton that contains all the neighbours.
 */
class NeighbourTable {
 public:
  /**
   * @brief Singleton instance.
   *
   * Function to get a reference to the singleton class.
   *
   * @return A pointer to the singleton.
   */
  static NeighbourTable* getInstance();
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
  void cleanNeighbours(int expirationTime);
  /**
   * Function to get a copy of the current neighbours list.
   *
   * @return a map with the current neighbours.
   */
  void getNeighbours(std::map<std::string, std::shared_ptr<Neighbour>> *map);

 private:
  /**
   * Constructor of the NeighbourTable.
   */
  NeighbourTable();
  /**
   * Instance of the singleton.
   */
  static NeighbourTable *m_instance;
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
