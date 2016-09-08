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
 * FILE ListeningEndpointsTable.cpp
 * AUTHOR Blackcatn13
 * DATE Jan 19, 2016
 * VERSION 1
 * This file contains the implementation of the ListeningEndpointsTable class.
 */

#include "Node/EndpointListener/ListeningEndpointsTable.h"
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include "Utils/Logger.h"

ListeningEndpointsTable::ListeningEndpointsTable() {
}

ListeningEndpointsTable::~ListeningEndpointsTable() {
}

void ListeningEndpointsTable::clean(int expirationTime) {
  LOG(62) << "Cleaning endpoints that have been out for more than "
          << expirationTime;
}

void ListeningEndpointsTable::update(std::string endpointId,
                                     std::shared_ptr<Endpoint> endpoint) {
  auto it = m_values.find(endpointId);
  if (it != m_values.end()) {
    m_values[endpointId].push_back(endpoint);
  } else {
    std::vector<std::shared_ptr<Endpoint>> endpoints { endpoint };
    m_values[endpointId] = endpoints;
  }
}

std::vector<std::string> ListeningEndpointsTable::getValues() {
  std::vector<std::string> keys;
  for (auto it = m_values.begin(); it != m_values.end(); ++it) {
    keys.push_back(it->first);
  }
  return keys;
}

std::vector<std::shared_ptr<Endpoint>> ListeningEndpointsTable::getValue(
    const std::string &name) {
  auto it = m_values.find(name);
  if (it != m_values.end())
    return it->second;
  else
    throw TableException("Value not found.");
}
