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
 * This file contains the implementation of the Neighbour class.
 */

#include "Node/Neighbour/NeighbourTable.h"
#include <string>
#include <cstdint>
#include <map>
#include <mutex>
#include "Node/Neighbour/Neighbour.h"

NeighbourTable *NeighbourTable::m_instance = 0;

NeighbourTable::NeighbourTable() {
}

NeighbourTable::~NeighbourTable() {
}

NeighbourTable* NeighbourTable::getInstance() {
  if (m_instance == 0) {
    m_instance = new NeighbourTable();
  }
  return m_instance;
}

void NeighbourTable::update(const std::string &nodeId,
                            const std::string &nodeAddress,
                            const uint16_t &nodePort) {
  mutex.lock();
  std::map<std::string, std::shared_ptr<Neighbour>>::iterator it = m_neighbours
      .find(nodeId);
  if (it != m_neighbours.end()) {
    if (m_neighbours[nodeId]->m_nodeAddress != nodeAddress)
      m_neighbours[nodeId]->m_nodeAddress = nodeAddress;
    if (m_neighbours[nodeId]->m_nodePort != nodePort)
      m_neighbours[nodeId]->m_nodePort = nodePort;
  } else {
    m_neighbours[nodeId] = std::make_shared<Neighbour>(nodeId, nodeAddress,
                                                       nodePort);
  }
  mutex.unlock();
}

void NeighbourTable::cleanNeighbours(int expirationTime) {
  mutex.lock();
  for (std::map<std::string, std::shared_ptr<Neighbour>>::iterator it =
      m_neighbours.begin(); it != m_neighbours.end(); ++it) {
    if ((*it).second->getElapsedActivityTime() >= expirationTime) {
      m_neighbours.erase(it);
    }
  }
  mutex.unlock();
}

void NeighbourTable::getNeighbours(
    std::map<std::string, std::shared_ptr<Neighbour>> *map) {
  mutex.lock();
  (*map).insert(m_neighbours.begin(), m_neighbours.end());
  mutex.unlock();
}

