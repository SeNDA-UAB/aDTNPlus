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
#include <unordered_set>

/**
 * CLASS Endpoint.
 * This class contains the information of one endpoint.
 */
class Endpoint {
 public:
  /**
   * @brief Generates a Endpoint with the given values.
   *
   * Generates an Endpoint with a endpoint id, an IP address and a port.
   *
   * @param endpointId the identifier of the endpoint.
   * @param endpointAddress the IP of the endpoint.
   * @param endpointPort the port of the endpoint.
   */
  Endpoint(const std::string &endpointId, const std::string &endpointAddress,
      const uint16_t &endpointPort, const int &socket);
  /**
   * Destructor of the class.
   */
  virtual ~Endpoint();
  /**
   * Gets the endpoint id.
   * @return The endpoint id.
   */
  std::string getId();
  /**
   * Gets the endpoint IP address.
   * @return The endpoint IP address.
   */
  std::string getAddress();
  /**
   * Gets the endpoint port.
   * @return The endpoint port.
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
   * @param endpointAddress IP address of the endpoint.
   * @param endpointPort port of the endpoint.
   */
  void update(std::shared_ptr<Endpoint> endpoint);
  /**
   * Equality operator overload.
   * @param endpoint to compare
   * @return True if the endpoints are equals.
   */
  bool operator==(const Endpoint &endpoint) const;

  bool checkDeliveredId(const std::string &id);

  void addDeliveredId(const std::string &id);

 private:
  /**
   * Identifier of the endpoint.
   */
  std::string m_id;
  /**
   * Ip address of the endpoint.
   */
  std::string m_address;
  /**
   * Port of the endpoint.
   */
  uint16_t m_port;
  /**
   * Socket of communication.
   */
  int m_socket;
  /**
   * Set of delivered bundle id, to calculate aggregation.
   */
  std::unordered_set<std::string> m_deliveredIds;
  /**
   * Time of the last activity of the endpoint.
   */
  std::chrono::steady_clock::time_point m_lastActivity;
};

#endif  // BUNDLEAGENT_NODE_ENDPOINTLISTENER_ENDPOINT_H_
