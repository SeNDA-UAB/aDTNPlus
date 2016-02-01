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
 * FILE ListeningAppsTable.cpp
 * AUTHOR Blackcatn13
 * DATE Jan 19, 2016
 * VERSION 1
 * This file contains the implementation of the ListeningAppsTable class.
 */

#include "Node/AppListener/ListeningAppsTable.h"
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include "Utils/Logger.h"

ListeningAppsTable::ListeningAppsTable() {
}

ListeningAppsTable::~ListeningAppsTable() {
}

void ListeningAppsTable::clean(int expirationTime) {
  LOG(62) << "Cleaning apps that have bben out for more than "
          << expirationTime;
  mutex.lock();
  for (std::map<std::string, std::shared_ptr<App>>::iterator it =
      m_values.begin(); it != m_values.end(); ++it) {
    if ((*it).second->getElapsedActivityTime() >= expirationTime) {
      LOG(17) << "App " << (*it).second->getId() << "has disappeared";
      m_values.erase(it);
    }
  }
  mutex.unlock();
}
