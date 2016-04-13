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
 * FILE EndpointListener.h
 * AUTHOR Blackcatn13
 * DATE Jan 19, 2016
 * VERSION 1
 * This file contains the Endpoint listener class.
 */
#ifndef BUNDLEAGENT_NODE_ENDPOINTLISTENER_ENDPOINTLISTENER_H_
#define BUNDLEAGENT_NODE_ENDPOINTLISTENER_ENDPOINTLISTENER_H_

#include <memory>
#include <string>
#include "Node/EndpointListener/ListeningEndpointsTable.h"
#include "Node/Config.h"

/**
 * CLASS EndpointListener
 * This class implements the Endpoint listener system.
 * A socket will be listening for incoming connections, this connections are
 * used to subscribe for receiving them.
 *
 * When the external application connects to the socket, it will send a message.
 * This messages has the following format:
 *
 * an uint8_t with the value 0.
 * an string with the endpoint value.
 *
 * The connection will be maintained open, an used
 * when a bundle is received and dispatched to the application.
 *
 * This class use the following parameters (all of them are in the adtn.ini file
 * under the EndpointListener section) :
 *
 * listenerAddres : address where the socket must listen.
 *
 * listenerPort : port where the socket must listen.
 */
class EndpointListener {
 public:
  /**
   * @brief Constructor
   *
   * This will create a EndpointListener object using the values in the Config.
   *
   * @param config Config with the parameters.
   * @param listeningEndpointsTable a listeningEndpointsTable.
   */
  EndpointListener(Config config,
              std::shared_ptr<ListeningEndpointsTable> listeningEndpointsTable);
  /**
   * Destructor of the class.
   */
  virtual ~EndpointListener();

 protected:
  /**
   * Config initialised.
   */
  Config m_config;
  /**
   * The Listening Endpoints table reference.
   */
  std::shared_ptr<ListeningEndpointsTable> m_listeningEndpointsTable;

 private:
  /**
   * Function to start the listening socket.
   */
  void listenEndpoints();
  /**
   * Function that gets the Endpoint to listen.
   */
  void startListening(int sock);
};

#endif  // BUNDLEAGENT_NODE_ENDPOINTLISTENER_ENDPOINTLISTENER_H_
