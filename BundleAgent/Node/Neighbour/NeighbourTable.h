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
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
#include "Node/Neighbour/Neighbour.h"

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
   * @brief Updates the value in the table.
   *
   * This function updates the given value.
   * If the value is not present in the table, this function adds the new value.
   *
   * @param neighbour The neighbour to check.
   */
  void update(std::shared_ptr<Neighbour> neighbour);
  /**
   * Returns a list with all the endpoints in the table.
   *
   * @return a vector with the current endpoints.
   */
  std::vector<std::string> getConnectedEID();
  /**
   * Returns a list with all the singleton endpoints in the table.
   *
   * @return a vector with the current singleton endpoints.
   */
  std::vector<std::string> getSingletonConnectedEID();
  /**
   * Returns the neighbour given a name.
   *
   * @param name The id of the neighbour.
   * @return The neighbour.
   */
  std::shared_ptr<Neighbour> getValue(const std::string &name);
  /**
   * Returns a list containing the minimum group that holds all the given
   * endpoints.
   *
   * @param endpoints The endpoints to generate the group.
   * @return All the nodes that compose that group.
   */
  std::vector<std::string> getMinNeighbours(std::vector<std::string> endpoints);
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

 private:
  /**
   * Function that inserts the given endpoints with the given neighbour id.
   *
   * If the endpoint exists, it will append the neighbour to it's lists, otherwise
   * it will create the new endpoint.
   *
   * @param endpoints The list of endpoints to insert.
   * @param neigbour The neighbour id.
   */
  void insert(std::vector<std::string> endpoints, std::string neigbour);
  /**
   * Function that removes from the given endpoints the given neighbour id.
   *
   * If the neighbour is the last one in the given endpoint, the endpoint is removed
   * from the list, otherwise only the given neighbour is removed.
   *
   * @param endpoints The list of endpoints to remove.
   * @param neigbour The neighbour id.
   */
  void remove(std::vector<std::string> endpoints, std::string neigbour);
  /**
   * Mutex for the maps.
   */
  std::mutex m_mutex;
  /**
   * Map that holds the endpoints, it uses the endpoint as key, and points
   * to a vector with the neighbours id's that have announced the endpoint.
   */
  std::unordered_map<std::string, std::unordered_set<std::string>> m_endpoints;
  /**
   * Map that holds the neighbours.
   */
  std::unordered_map<std::string, std::shared_ptr<Neighbour>> m_neigbours;
};

#endif  // BUNDLEAGENT_NODE_NEIGHBOUR_NEIGHBOURTABLE_H_
