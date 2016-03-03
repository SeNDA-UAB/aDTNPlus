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
 * FILE ListeningAppsTable.h
 * AUTHOR Blackcatn13
 * DATE Jan 19, 2016
 * VERSION 1
 * This file contains the ListeningAppsTable class.
 */
#ifndef BUNDLEAGENT_NODE_APPLISTENER_LISTENINGAPPSTABLE_H_
#define BUNDLEAGENT_NODE_APPLISTENER_LISTENINGAPPSTABLE_H_

#include <map>
#include <string>
#include <cstdint>
#include <mutex>
#include <memory>
#include <vector>
#include <stdexcept>
#include "Node/AppListener/App.h"
#include "Utils/Table.h"

/**
 * CLASS ListeningAppsTable
 * This class contains all the listening apps.
 */
class ListeningAppsTable : public Table<App> {
 public:
  /**
   * Constructor of the ListeningAppsTable.
   */
  ListeningAppsTable();
  /**
   * Destructor of the class.
   */
  virtual ~ListeningAppsTable();
  /**
   * @brief Clean all the apps that have expired.
   *
   * This function deletes all the apps that have been expired.
   * This means all the apps that have a last activity value greater that
   * the expirationTime.
   *
   * @param expirationTime Minimum time to expire a neighbour.
   */
  void clean(int expirationTime);
};

#endif  // BUNDLEAGENT_NODE_APPLISTENER_LISTENINGAPPSTABLE_H_
