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
 * FILE adtnSocket.h
 * AUTHOR Blackcatn13
 * DATE Feb 3, 2016
 * VERSION 1
 * This file contains the definition of the adtnSocket.
 */
#ifndef LIB_ADTNSOCKET_H_
#define LIB_ADTNSOCKET_H_

#include <string>
#include <stdexcept>
#include <memory>
#include <vector>
#include <map>
#include "Bundle/BundleTypes.h"

class Bundle;
class CanonicalBlock;
class FrameworkExtension;

class adtnSocketException : public std::runtime_error {
 public:
  explicit adtnSocketException(const std::string &what)
      : runtime_error(what) {
  }
};

class adtnMissingBundleException : public std::runtime_error {
 public:
  explicit adtnMissingBundleException(const std::string &what)
      : runtime_error(what) {
  }
};

class adtnSocketTimeoutException : public std::runtime_error {
 public:
  explicit adtnSocketTimeoutException(const std::string &what)
      : runtime_error(what) {
  }
};

/**
 * CLASS adtnSocket
 * This class is a library that's used to help the task of sending an receiving
 * messages using the adtnPlus platform.
 */
class adtnSocket {
 public:
  /**
   * Generates an adtnSocket with the following information:
   *
   * @param ip The IP used to send messages to the node.
   * @param sendPort The port used to send messages to the node.
   * @param recvPort The port used to register into the node.
   * @param recvIp The IP used to register into the node.
   */
  adtnSocket(std::string ip, int sendPort, int recvPort, std::string recvIp);
  /**
   * Generates an adtnSocket, in this case the IP is the same for sending and
   * registering in the node.
   *
   * @param ip The IP used to send messages and to register into the node.
   * @param sendPort The port used to send messages to the node.
   * @param recvPort The port used to register into the node.
   */
  adtnSocket(std::string ip, int sendPort, int recvPort);
  /**
   * Generates an adtnSocket, in this case the IP is the same for sending and
   * registering in the node. This function is used to only send or to register
   * depending on the send value.
   *
   * @param ip The IP used to send messages and to register into the node.
   * @param port The port used to send messages or to register to the node.
   * @param send If send is true (Default value) the port is assumed to be used
   *             to send messages, if the value is false is assumed to be used
   *             to register into the node.
   */
  adtnSocket(std::string ip, int port, bool send = true);
  /**
   * Destructor of the class.
   */
  virtual ~adtnSocket();
  /**
   * @brief Register this application into the node using the following appId.
   *
   * If the adtnSocket has been created without a receive Port the function will
   * throw an exception.
   *
   * @param appId The appId to register.
   */
  void connect(std::string appId);
  /**
   * @brief Receives a message from the node.
   *
   * This function will block until a message is received from the node.
   * The adtnSocket must have been connected before using this function.
   *
   * @return An string containing the received message.
   */
  std::string recv();
  /**
   * @brief Receives a message from the node.
   *
   * This function will block until a message is received from the node.
   * The adtnSocket must have been connected before using this function.
   *
   * @param timeout seconds to timeout the receive call.
   * @return An string containing the received message.
   */
  std::string recv(int timeout);

  /**
   * @brief Sends a message with the following parameters:
   *
   * @param destination The destination of the message.
   * @param message The message to send.
   */
  void send(std::string destination, std::string message);
  /**
   * @brief Allows to change the default sender id.
   *
   * @param source The new sender id.
   */
  void changeSource(std::string source);
  /**
   * @brief Adds a Routing Selection MEB to the bundle.
   *
   * @param type The type value, it can be
   *              * 0x01 for anti-rebooting
   *              * 0x02 for flooding
   */
  void addRoutingSelection(uint8_t type);
  /**
   * @brief Adds an Active forwarding MEB to the bundle.
   *
   * @param code The code that will be executed when forwarding.
   */
  void addActiveForwarding(std::string code);
  /**
   * @brief Ask to clear all the cached blocks that will go to a bundle.
   */
  void clearBlocks();
  /**
   * @brief Adds a Route reporting MEB to the bundle.
   *
   * The route reporting block will log the arrival and the depart time of
   * the bundle in the different nodes it travels.
   */
  void addRouteReporting();
  /**
   * If the last received bundle contains a routeReporting MEB it will
   * return the information.
   * It will throw an exception otherwise.
   *
   * @return The information of the routing report.
   */
  std::string getRouteReporting();
  /**
   * @brief Adds a Framework extension.
   *
   * If the framework extension already exist it will overwrite it, otherwise
   * it will be created.
   *
   * @param frameworkId The id of the framework.
   * @param extensionId The id of the extension.
   * @param code The code for the extension.
   */
  void addFrameworkExtension(uint8_t frameworkId, uint8_t extensionId,
                             std::string code);
  /**
   * If the last received bundle contains a Framework MEB it will return the
   * BundleState (a json in string format) of the given framework.
   *
   * @param frameworkId The id of the framework.
   * @return The BundleState (a json in string format).
   */
  std::string getBundleState(uint8_t frameworkId);

 private:
  /**
   * The IP of the node, it the IP for sending if an IP has been given for register,
   * if not is the IP for both actions.
   */
  std::string m_nodeIp;
  /**
   * The IP for register the application, it can be null, if this is the case the
   * m_nodeIp is used instead.
   */
  std::string m_listeningIp;
  /**
   * The port used to send the messages.
   */
  int m_sendPort;
  /**
   * The port used to register the application.
   */
  int m_recvPort;
  /**
   * The source name used to notify the node.
   */
  std::string m_nodeName;
  /**
   * The source name used in the bundle source.
   */
  std::string m_sourceName;
  /**
   * The socket used to receive the bundles.
   */
  int m_recvSocket;
  /**
   * Lists of canonical blocks that need to be added when creating a new bundle.
   */
  std::vector<std::shared_ptr<CanonicalBlock>> m_blocksToAdd;
  /**
   * Map to save the frameworks and its extensions.
   */
  std::map<uint8_t, std::map<uint8_t, std::string>> m_frameworkExtensions;
  /**
   * The last received bundle.
   */
  Bundle *m_lastBundle;
};

#endif  // LIB_ADTNSOCKET_H_
