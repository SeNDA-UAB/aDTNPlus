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
 * FILE NeighbourTable.cpp
 * AUTHOR Blackcatn13
 * DATE Jun 29, 2015
 * VERSION 1
 * This file contains the implementation of the NeighbourTable class.
 */

#include "Node/Neighbour/NeighbourTable.h"
#include <string>
#include <cstdint>
#include <map>
#include <mutex>
#include <vector>
#include <algorithm>
#include "Utils/Logger.h"
#include "Utils/globals.h"

#define LOG_NEIGHBOUR_TABLE 98

NeighbourTable::NeighbourTable() {
}

NeighbourTable::~NeighbourTable() {
}

void NeighbourTable::update(std::shared_ptr<Neighbour> neighbour) {
  m_mutex.lock();
  auto it = m_neigbours.find(neighbour->getId());
  if (it != m_neigbours.end()) {
    auto newEndpoints = neighbour->getEndpoints();
    auto oldEndpoints = it->second->getEndpoints();
    if (newEndpoints.size() != oldEndpoints.size()
        || !std::equal(newEndpoints.begin(), newEndpoints.end(),
                       oldEndpoints.begin())) {
      std::vector<std::string> oldDiff(oldEndpoints.size());
      std::set_difference(oldEndpoints.begin(), oldEndpoints.end(),
                          newEndpoints.begin(), newEndpoints.end(),
                          oldDiff.begin());
      remove(oldDiff, neighbour->getId());
      std::vector<std::string> newDiff(newEndpoints.size());
      std::set_difference(newEndpoints.begin(), newEndpoints.end(),
                          oldEndpoints.begin(), oldEndpoints.end(),
                          newDiff.begin());
      insert(newDiff, neighbour->getId());
    }
    it->second->update(neighbour);
  } else {
    m_neigbours[neighbour->getId()] = neighbour;
    insert(neighbour->getEndpoints(), neighbour->getId());
    // Notify Processor that a new neighbour has appeared, so it can process
    g_queueProcessEvents++;
    std::unique_lock<std::mutex> lck(g_processorMutex);
    g_processorConditionVariable.notify_one();
  }
  m_mutex.unlock();
}

std::vector<std::string> NeighbourTable::getConnectedEID() {
  std::vector<std::string> keys;
  m_mutex.lock();
  keys.reserve(m_endpoints.size());
  std::transform(m_endpoints.begin(), m_endpoints.end(),
                 std::back_inserter(keys),
                 [](const typename std::unordered_map<std::string,
                     std::unordered_set<std::string>>::value_type &pair) {
                   return pair.first;
                 });
  m_mutex.unlock();
  return keys;
}

std::vector<std::string> NeighbourTable::getSingletonConnectedEID() {
  std::vector<std::string> keys;
  m_mutex.lock();
  keys.reserve(m_neigbours.size());
  std::transform(m_neigbours.begin(), m_neigbours.end(),
                 std::back_inserter(keys),
                 [](const typename std::unordered_map<std::string,
                     std::shared_ptr<Neighbour>>::value_type &pair) {
                   return pair.first;
                 });
  m_mutex.unlock();
  return keys;
}

std::shared_ptr<Neighbour> NeighbourTable::getValue(const std::string &name) {
  auto it = m_neigbours.find(name);
  if (it != m_neigbours.end())
    return it->second;
  else
    throw NeighbourTableException("Value not found.");
}

std::vector<std::string> NeighbourTable::getMinNeighbours(
    std::vector<std::string> endpoints) {
  std::vector<std::string> neighbours;
  m_mutex.lock();
  for (auto endpoint : endpoints) {
    neighbours.insert(neighbours.begin(), m_endpoints[endpoint].begin(),
                      m_endpoints[endpoint].end());
  }
  m_mutex.unlock();
  std::sort(neighbours.begin(), neighbours.end());
  auto last = std::unique(neighbours.begin(), neighbours.end());
  neighbours.erase(last, neighbours.end());
  return neighbours;
}

void NeighbourTable::clean(int expirationTime) {
  LOG(LOG_NEIGHBOUR_TABLE) << "Cleaning neighbours that have been out for more than "
          << expirationTime;
  m_mutex.lock();
  for (auto it = m_neigbours.begin(); it != m_neigbours.end();) {
    if ((*it).second->getElapsedActivityTime() >= expirationTime) {
      LOG(LOG_NEIGHBOUR_TABLE) << "Neighbour " << (*it).second->getId() << " has disappeared";
      remove(it->second->getEndpoints(), it->second->getId());
      it = m_neigbours.erase(it);
    } else {
      ++it;
    }
  }
  m_mutex.unlock();
}

void NeighbourTable::insert(std::vector<std::string> endpoints,
                            std::string neighbour) {
  for (auto endpoint : endpoints) {
    m_endpoints[endpoint].insert(neighbour);
  }
  m_endpoints[neighbour].insert(neighbour);
}

void NeighbourTable::remove(std::vector<std::string> endpoints,
                            std::string neighbour) {
  for (auto endpoint : endpoints) {
    if (m_endpoints[endpoint].size() != 1) {
      m_endpoints[endpoint].erase(neighbour);
    } else {
      m_endpoints.erase(endpoint);
    }
  }
  m_endpoints.erase(neighbour);
}
