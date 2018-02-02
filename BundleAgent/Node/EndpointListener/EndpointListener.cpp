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
 * FILE EndpointListener.cpp
 * AUTHOR Blackcatn13
 * DATE Jan 19, 2016
 * VERSION 1
 * This file contains the implementation of the EndpointListener class.
 */

#include "Node/EndpointListener/EndpointListener.h"
#include <memory>
#include <thread>
#include <string>
#include "Utils/Logger.h"
#include "Utils/globals.h"
#include "Utils/Socket.h"

EndpointListener::EndpointListener(
    Config config,
    std::shared_ptr<ListeningEndpointsTable> listeningEndpointsTable)
    : m_config(config),
      m_listeningEndpointsTable(listeningEndpointsTable) {
  std::thread t = std::thread(&EndpointListener::listenEndpoints, this);
  t.detach();
  LOG(68) << "Creating Endpoint listener.";
}

EndpointListener::~EndpointListener() {
}

void EndpointListener::listenEndpoints() {
  Logger::getInstance()->setThreadName(std::this_thread::get_id(),
                                       "Endpoint listener");
  LOG(17) << "Creating Listening Endpoints thread.";
  Socket s = Socket();
  if (!s) {
    // Stop the application
    LOG(1) << "Cannot create socket into listenEndpoints thread, reason: "
           << s.getLastError();
    g_stop = true;
  } else {
    s.setReuseAddress();
    if (!s.bind(m_config.getListenerAddress(), m_config.getListenerPort())) {
      // Stop the application
      LOG(1) << "Cannot bind socket to " << m_config.getListenerAddress()
             << ", reason: " << s.getLastError();
      g_stop = true;
    } else {
      if (!s.listen(50)) {
        // Stop the application
        LOG(1) << "Cannot set the socket to listen, reason: "
               << s.getLastError();
        g_stop = true;
      } else {
        LOG(17) << "Listening petitions at (" << m_config.getListenerAddress()
                << ":" << m_config.getListenerPort() << ")";
        g_startedThread++;
        while (!g_stop.load()) {
          Socket newSocket = Socket(-1);
          if (!s.accept(m_config.getSocketTimeout(), newSocket)) {
            continue;
          }
          if (!newSocket) {
            LOG(4) << "Cannot accept connection, reason: "
                   << newSocket.getLastError();
            continue;
          } else {
            LOG(80) << "Connection received.";
            std::thread(&EndpointListener::startListening, this, newSocket)
                .detach();
          }
        }
      }
    }
    s.close();
  }
  LOG(17) << "Exit Endpoint Listener thread.";
  g_stopped++;
}

void EndpointListener::startListening(Socket sock) {
  Logger::getInstance()->setThreadName(std::this_thread::get_id(),
                                       "Connection thread");
  LOG(69) << "Processing new connection";
  LOG(17) << "Receiving endpoint petition from " << sock.getPeerName();
  uint8_t type;
  sock >> type;
  if (type == 0) {
    LOG(17) << "Someone asked to add an EndpointId";
    uint32_t eid = 0;
    sock >> eid;
    std::string buffer;
    StringWithSize sws = StringWithSize(buffer, eid);
    if (!(sock >> sws)) {
      LOG(4) << "Error receiving endpoint, reason: " << sock.getLastError();
    }
    m_listeningEndpointsTable->update(
        buffer, std::make_shared<Endpoint>(buffer, "", 0, sock));
    LOG(17) << "Registered endpoint: " << buffer;
  }
}
