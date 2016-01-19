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
 * FILE App.h
 * AUTHOR Blackcatn13
 * DATE Jan 19, 2016
 * VERSION 1
 * This file contains the App class.
 */
#ifndef BUNDLEAGENT_NODE_APPLISTENER_APP_H_
#define BUNDLEAGENT_NODE_APPLISTENER_APP_H_

#include <string>
#include <cstdint>
#include <chrono>

/**
 * CLASS App.
 * This class contains the information of one app.
 */
class App {
 public:
  /**
   * @brief Generates a App with the given values.
   *
   * Generates an App with a app id, an IP address and a port.
   *
   * @param appId the identifier of the app.
   * @param appAddress the IP of the app.
   * @param appPort the port of the app.
   */
  App(const std::string &appId, const std::string &appAddress,
      const uint16_t &appPort);
  /**
   * Destructor of the class.
   */
  virtual ~App();
  /**
   * Gets the app id.
   * @return The app id.
   */
  std::string getAppId();
  /**
   * Gets the app IP address.
   * @return The app IP address.
   */
  std::string getAppAddress();
  /**
   * Gets the app port.
   * @return The app port.
   */
  uint16_t getAppPort();
  /**
   * Returns the elapsed time since the last activity.
   *
   * @return The elapsed seconds.
   */
  int getElapsedActivityTime();
  /**
   * This functionupdates the last activity
   * @param appAddress IP address of the app.
   * @param appPort port of the app.
   */
  void update(const std::string &appAddress, const uint16_t &appPort);
  /**
   * Equality operator overload.
   * @param app to compare
   * @return True if the apps are equals.
   */
  bool operator==(const App &app) const;

 private:
  /**
   * Identifier of the app.
   */
  std::string m_appId;
  /**
   * Ip address of the app.
   */
  std::string m_appAddress;
  /**
   * Port of the app.
   */
  uint16_t m_appPort;
  /**
   * Time of the last activity of the app.
   */
  std::chrono::steady_clock::time_point m_lastActivity;
};

#endif  // BUNDLEAGENT_NODE_APPLISTENER_APP_H_
