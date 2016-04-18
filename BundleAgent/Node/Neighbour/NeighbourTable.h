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

  void update(std::shared_ptr<Neighbour> neighbour);

  std::vector<std::string> getValues();

  std::shared_ptr<Neighbour> getValue(const std::string &name);

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

  void insert(std::vector<std::string> endpoints, std::string neigbour);
  void remove(std::vector<std::string> endpoints, std::string neigbour);

  std::mutex m_mutex;
  std::unordered_map<std::string, std::unordered_set<std::string>> m_endpoints;
  std::unordered_map<std::string, std::shared_ptr<Neighbour>> m_neigbours;
};

#endif  // BUNDLEAGENT_NODE_NEIGHBOUR_NEIGHBOURTABLE_H_
