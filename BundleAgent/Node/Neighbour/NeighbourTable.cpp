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
#include "Utils/Logger.h"

NeighbourTable *NeighbourTable::m_instance = 0;

NeighbourTable::NeighbourTable() {
}

NeighbourTable::~NeighbourTable() {
}

NeighbourTable* NeighbourTable::getInstance() {
  LOG(70) << "Getting instance";
  if (m_instance == 0) {
    m_instance = new NeighbourTable();
  }
  return m_instance;
}

void NeighbourTable::update(const std::string &nodeId,
                            const std::string &nodeAddress,
                            const uint16_t &nodePort) {
  LOG(61) << "Updating table with neighbour [nodeId: " << nodeId
          << "][nodeAddress: " << nodeAddress << "][nodePort: " << nodePort
          << "]";
  mutex.lock();
  std::map<std::string, std::shared_ptr<Neighbour>>::iterator it = m_neighbours
      .find(nodeId);
  if (it != m_neighbours.end()) {
    m_neighbours[nodeId]->update(nodeAddress, nodePort);
  } else {
    LOG(17) << "New neighbour " << nodeId << " added";
    m_neighbours[nodeId] = std::make_shared<Neighbour>(nodeId, nodeAddress,
                                                       nodePort);
  }
  mutex.unlock();
}

void NeighbourTable::cleanNeighbours(int expirationTime) {
  LOG(62) << "Cleaning neighbours that have been out for more than "
          << expirationTime;
  mutex.lock();
  for (std::map<std::string, std::shared_ptr<Neighbour>>::iterator it =
      m_neighbours.begin(); it != m_neighbours.end(); ++it) {
    if ((*it).second->getElapsedActivityTime() >= expirationTime) {
      LOG(17) << "Neighbour " << (*it).second->getNodeId()
              << " has disappeared";
      m_neighbours.erase(it);
    }
  }
  mutex.unlock();
}

void NeighbourTable::getNeighbours(
    std::map<std::string, std::shared_ptr<Neighbour>> *map) {
  LOG(61) << "Returning current neighbours";
  mutex.lock();
  (*map).insert(m_neighbours.begin(), m_neighbours.end());
  mutex.unlock();
}

