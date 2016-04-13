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
#include "Utils/Table.h"

/**
 * CLASS ListeningEndpointsTable
 * This class contains all the listening Endpoints.
 */
class ListeningEndpointsTable : public Table<Endpoint> {
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
};

#endif  // BUNDLEAGENT_NODE_ENDPOINTLISTENER_LISTENINGENDPOINTSTABLE_H_
