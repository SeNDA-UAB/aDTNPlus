/*
 * Copyright (c) 2016 SeNDA
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
 * FILE ListeningEndpointsTable.h
 * AUTHOR Blackcatn13
 * DATE Jan 19, 2016
 * VERSION 1
 * This file contains the ListeningEndpointsTable class.
 */
#ifndef BUNDLEAGENT_NODE_ENDPOINTLISTENER_LISTENINGENDPOINTSTABLE_H_
#define BUNDLEAGENT_NODE_ENDPOINTLISTENER_LISTENINGENDPOINTSTABLE_H_

#include <map>
#include <string>
#include <cstdint>
#include <mutex>
#include <memory>
#include <vector>
#include <stdexcept>
#include "Node/EndpointListener/Endpoint.h"

class TableException : public std::runtime_error {
 public:
  explicit TableException(const std::string &what)
      : runtime_error(what) {
  }
};

/**
 * CLASS ListeningEndpointsTable
 * This class contains all the listening Endpoints.
 */
class ListeningEndpointsTable {
 public:
  /**
   * Constructor of the ListeningEndpointsTable.
   */
  ListeningEndpointsTable();
  /**
   * Destructor of the class.
   */
  virtual ~ListeningEndpointsTable();
  /**
   * @brief Clean all the Endpoints that have expired.
   *
   * This function deletes all the Endpoints that have been expired.
   * This means all the Endpoints that have a last activity value greater that
   * the expirationTime.
   *
   * @param expirationTime Minimum time to expire a neighbour.
   */
  void clean(int expirationTime);
  /**
   * @brief Updates the value in the table.
   *
   * This function updates the given value.
   * If the value is not present in the table, this function adds the new value.
   *
   * @param value The value to check.
   */
  void update(std::string endpointId, std::shared_ptr<Endpoint> endpoint);

  /**
   * Function to get a list of all the id's in the Table.
   *
   * @return a vector with the current values id's.
   */
  std::vector<std::string> getValues();
  /**
   * Function to get the information of the given name.
   *
   * @param name the id of the value.
   * @return a T pointer if exists, else throws a TableException.
   */
  std::vector<std::shared_ptr<Endpoint>> getValue(const std::string &name);

 protected:
  /**
   * Map with the neighbours.
   */
  std::map<std::string, std::vector<std::shared_ptr<Endpoint>>> m_values;
  /**
   * Mutex for the map.
   */
  std::mutex mutex;
};

#endif  // BUNDLEAGENT_NODE_ENDPOINTLISTENER_LISTENINGENDPOINTSTABLE_H_
