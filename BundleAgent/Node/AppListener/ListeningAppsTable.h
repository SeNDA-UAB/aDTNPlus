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

class App;

class ListeningAppsTableException : public std::runtime_error {
 public:
  explicit ListeningAppsTableException(const std::string &what)
      : runtime_error(what) {
  }
};

/**
 * CLASS ListeningAppsTable
 * This class contains all the listening apps.
 */
class ListeningAppsTable {
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
   * @brief Updates the app.
   *
   * This function updates an app.
   * If the app is not present in the table this function add the new app.
   *
   * @param appId identifier of the app.
   * @param appAddress IP address of the app.
   * @param port port of the app.
   */
  void update(const std::string &appId, const std::string &appAddress,
              const uint16_t &port, const int &socket);
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
  /**
   * Function to get a copy of the current apps list id's
   *
   * @return
   *  a vector with the current apps id's.
   */
  std::vector<std::string> getAppIds();
  /**
   * Function to get the information of the given app.
   *
   * @param appId the id of the app.
   * @return a App pointer if exists, else throws a ListeningAppsTableException
   */
  std::shared_ptr<App> getApp(const std::string &appId);

 private:
  /**
   * Map with the apps.
   */
  std::map<std::string, std::shared_ptr<App>> m_listeningApps;
  /**
   * Mutex for the map.
   */
  std::mutex mutex;
};

#endif  // BUNDLEAGENT_NODE_APPLISTENER_LISTENINGAPPSTABLE_H_
