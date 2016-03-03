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

NeighbourTable::NeighbourTable() {
}

NeighbourTable::~NeighbourTable() {
}

void NeighbourTable::clean(int expirationTime) {
  LOG(62) << "Cleaning neighbours that have been out for more than "
          << expirationTime;
  mutex.lock();
  for (std::map<std::string, std::shared_ptr<Neighbour>>::iterator it = m_values
      .begin(); it != m_values.end();) {
    if ((*it).second->getElapsedActivityTime() >= expirationTime) {
      LOG(17) << "Neighbour " << (*it).second->getId() << " has disappeared";
      it = m_values.erase(it);
    } else {
      ++it;
    }
  }
  mutex.unlock();
}
