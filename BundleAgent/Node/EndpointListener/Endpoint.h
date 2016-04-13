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
 * FILE Endpoint.h
 * AUTHOR Blackcatn13
 * DATE Jan 19, 2016
 * VERSION 1
 * This file contains the Endpoint class.
 */
#ifndef BUNDLEAGENT_NODE_ENDPOINTLISTENER_ENDPOINT_H_
#define BUNDLEAGENT_NODE_ENDPOINTLISTENER_ENDPOINT_H_

#include <string>
#include <cstdint>
#include <chrono>
#include <memory>

/**
 * CLASS Endpoint.
 * This class contains the information of one app.
 */
class Endpoint {
 public:
  /**
   * @brief Generates a Endpoint with the given values.
   *
   * Generates an Endpoint with a app id, an IP address and a port.
   *
   * @param appId the identifier of the app.
   * @param appAddress the IP of the app.
   * @param appPort the port of the app.
   */
  Endpoint(const std::string &appId, const std::string &appAddress,
      const uint16_t &appPort, const int &socket);
  /**
   * Destructor of the class.
   */
  virtual ~Endpoint();
  /**
   * Gets the app id.
   * @return The app id.
   */
  std::string getId();
  /**
   * Gets the app IP address.
   * @return The app IP address.
   */
  std::string getAddress();
  /**
   * Gets the app port.
   * @return The app port.
   */
  uint16_t getPort();
  /**
   * Gets the socket used to communicate.
   * @return The socket.
   */
  int getSocket();
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
  void update(std::shared_ptr<Endpoint> app);
  /**
   * Equality operator overload.
   * @param app to compare
   * @return True if the apps are equals.
   */
  bool operator==(const Endpoint &app) const;

 private:
  /**
   * Identifier of the app.
   */
  std::string m_id;
  /**
   * Ip address of the app.
   */
  std::string m_address;
  /**
   * Port of the app.
   */
  uint16_t m_port;
  /**
   * Socket of communication.
   */
  int m_socket;
  /**
   * Time of the last activity of the app.
   */
  std::chrono::steady_clock::time_point m_lastActivity;
};

#endif  // BUNDLEAGENT_NODE_ENDPOINTLISTENER_ENDPOINT_H_
