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
 * FILE AppListener.h
 * AUTHOR Blackcatn13
 * DATE Jan 19, 2016
 * VERSION 1
 * This file contains the App listener class.
 */
#ifndef BUNDLEAGENT_NODE_APPLISTENER_APPLISTENER_H_
#define BUNDLEAGENT_NODE_APPLISTENER_APPLISTENER_H_

#include <memory>
#include "Node/AppListener/ListeningAppsTable.h"
#include "Node/Config.h"

/**
 * CLASS AppListener
 * This class implements the App listener system.
 * A socket will be listening for incoming connections, this connections are
 * used to subscribe for receiving them.
 *
 * When the external application connects to the socket, it will send a message.
 * This messages has the following format:
 *
 * an uint8_t with the value 0.
 * an int with the App value.
 *
 * The connection will be maintained open, an used
 * when a bundle is received and dispatched to the application.
 *
 * This class use the following parameters (all of them are in the adtn.ini file
 * under the AppListener section) :
 *
 * listenerAddres : address where the socket must listen.
 *
 * listenerPort : port where the socket must listen.
 */
class AppListener {
 public:
  /**
   * @brief Constructor
   *
   * This will create a AppListener object using the values in the Config.
   *
   * @param config Config with the parameters.
   * @param listeningAppsTable a listeningAppsTable.
   */
  AppListener(Config config,
              std::shared_ptr<ListeningAppsTable> listeningAppsTable);
  /**
   * Destructor of the class.
   */
  virtual ~AppListener();

 protected:
  /**
   * Config initialised.
   */
  Config m_config;
  /**
   * The Listening Apps table reference.
   */
  std::shared_ptr<ListeningAppsTable> m_listeningAppsTable;

 private:
  /**
   * Function to start the listening socket.
   */
  void listenApps();
  /**
   * Function that gets the app to listen.
   */
  void startListening(int sock);
};

#endif  // BUNDLEAGENT_NODE_APPLISTENER_APPLISTENER_H_
