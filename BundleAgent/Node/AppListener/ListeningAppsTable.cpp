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
#include "Node/AppListener/App.h"
#include "Utils/Logger.h"

ListeningAppsTable::ListeningAppsTable() {
}

ListeningAppsTable::~ListeningAppsTable() {
}

void ListeningAppsTable::update(const std::string &appId,
                                const std::string &appAddress,
                                const uint16_t &appPort, const int &socket) {
  LOG(61) << "Updating table with app [appId: " << appId << "][appAddress: "
          << appAddress << "][appPort: " << appPort << "]";
  mutex.lock();
  std::map<std::string, std::shared_ptr<App>>::iterator it = m_listeningApps
      .find(appId);
  if (it != m_listeningApps.end()) {
    m_listeningApps[appId]->update(appAddress, appPort);
  } else {
    LOG(17) << "New app " << appId << " added";
    m_listeningApps[appId] = std::make_shared<App>(appId, appAddress, appPort,
                                                   socket);
  }
  mutex.unlock();
}

void ListeningAppsTable::clean(int expirationTime) {
  LOG(62) << "Cleaning apps that have bben out for more than "
          << expirationTime;
  mutex.lock();
  for (std::map<std::string, std::shared_ptr<App>>::iterator it =
      m_listeningApps.begin(); it != m_listeningApps.end(); ++it) {
    if ((*it).second->getElapsedActivityTime() >= expirationTime) {
      LOG(17) << "App " << (*it).second->getAppId() << "has disappeared";
      m_listeningApps.erase(it);
    }
  }
  mutex.unlock();
}

std::vector<std::string> ListeningAppsTable::getAppIds() {
  LOG(61) << "Returning current apps.";
  std::vector<std::string> keys;
  mutex.lock();
  keys.reserve(m_listeningApps.size());
  std::transform(
      m_listeningApps.begin(), m_listeningApps.end(), std::back_inserter(keys),
      [](const std::map<std::string,
          std::shared_ptr<App>>::value_type &pair) {return pair.first;});
  mutex.unlock();
  return keys;
}

std::shared_ptr<App> ListeningAppsTable::getApp(const std::string &appId) {
  LOG(61) << "Returning app " << appId;
  auto it = m_listeningApps.find(appId);
  if (it != m_listeningApps.end())
    return it->second;
  else
    throw ListeningAppsTableException("App not found");
}
