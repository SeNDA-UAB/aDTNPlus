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
#include "Utils/Table.h"
#include "Node/Neighbour/Neighbour.h"

/**
 * CLASS NeighbourTable
 * This class contains all the neighbours.
 */
class NeighbourTable : public Table<Neighbour> {
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
   * @brief Clean all the neighbours that have expired.
   *
   * This function deletes all the neighbours that have been expired.
   * This means all the neighbours that have a last activity value greater than
   * the expirationTime.
   *
   * @param expirationTime Minimum time to expire a neighbour.
   */
  void clean(int expirationTime);
};

#endif  // BUNDLEAGENT_NODE_NEIGHBOUR_NEIGHBOURTABLE_H_
