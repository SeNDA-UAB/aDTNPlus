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
 * FILE AppListener.cpp
 * AUTHOR Blackcatn13
 * DATE Jan 19, 2016
 * VERSION 1
 * This file contains the implementation of the AppListener class.
 */

#include "Node/AppListener/AppListener.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <unistd.h>
#include <memory>
#include <thread>
#include "Utils/Logger.h"
#include "Utils/globals.h"

AppListener::AppListener(Config config,
                         std::shared_ptr<ListeningAppsTable> listeningAppsTable)
    : m_config(config),
      m_listeningAppsTable(listeningAppsTable) {
  std::thread t = std::thread(&AppListener::listenApps, this);
  t.detach();
  LOG(68) << "Creating App listener.";
}

AppListener::~AppListener() {
}

void AppListener::listenApps() {
  LOG(17) << "Creating Listening Apps thread.";
  sockaddr_in listenAddr = { 0 };
  listenAddr.sin_family = AF_INET;
  listenAddr.sin_port = htons(m_config.getListenerPort());
  listenAddr.sin_addr.s_addr = inet_addr(m_config.getListenerAddress().c_str());
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1) {
    // Stop the application
    LOG(1) << "Cannot create socket into listenApps thread, reason: "
           << strerror(errno);
    g_stop = true;
  } else {
    if (bind(sock, reinterpret_cast<sockaddr*>(&listenAddr), sizeof(listenAddr))
        == -1) {
      // Stop the application
      LOG(1) << "Cannot bind socket to " << m_config.getListenerAddress()
             << ", reason: " << strerror(errno);
      g_stop = true;
    } else {
      if (listen(sock, 50) != 0) {
        // Stop the application
        LOG(1) << "Cannot set the socket to listen, reason: "
               << strerror(errno);
        g_stop = true;
      } else {
        LOG(17) << "Listening petitions at (" << m_config.getListenerAddress()
                << ":" << m_config.getListenerPort() << ")";
        fd_set readfds;
        while (!g_stop.load()) {
          FD_ZERO(&readfds);
          FD_SET(sock, &readfds);
          struct timeval tv;
          tv.tv_sec = m_config.getSocketTimeout();
          int sel = select(sock + 1, &readfds, NULL, NULL, &tv);
          if (sel <= 0)
            continue;
          if (FD_ISSET(sock, &readfds)) {
            sockaddr_in clientAddr = { 0 };
            socklen_t clientLen = sizeof(clientAddr);
            int newsock = accept(sock, reinterpret_cast<sockaddr*>(&clientAddr),
                                 &clientLen);
            if (newsock == -1) {
              LOG(1) << "Cannot accept connection, reason: " << strerror(errno);
              continue;
            }
            LOG(80) << "Connection received.";
            std::thread(&AppListener::startListening, this, newsock).detach();
          }
        }
      }
    }
    close(sock);
  }
  LOG(17) << "Exit App Listener thread.";
  g_stopped++;
}

void AppListener::startListening(int sock) {
  LOG(69) << "Processing new connection";
  sockaddr_in bundleSrc = { 0 };
  socklen_t bundleSrcLength = sizeof(bundleSrc);
  if (getpeername(sock, reinterpret_cast<sockaddr*>(&bundleSrc),
                  &bundleSrcLength) != 0) {
    LOG(3) << "Cannot get peer name, reason: " << strerror(errno);
  } else {
    LOG(10) << "Receiving bundle from " << inet_ntoa(bundleSrc.sin_addr) << ":"
            << ntohs(bundleSrc.sin_port);
  }
  uint8_t type = 100;
  int receivedSize = recv(sock, &type, sizeof(type), 0);
  if (type == 0) {
    LOG(17) << "Someone asked to add an AppId";
    uint32_t appId = 0;
    receivedSize = recv(sock, &appId, sizeof(appId), 0);
    if (receivedSize != sizeof(appId)) {
      if (receivedSize == 0) {
        LOG(1) << "Error receiving bundle length from "
               << inet_ntoa(bundleSrc.sin_addr)
               << " Probably peer has disconnected.";
      } else if (receivedSize < 0) {
        LOG(1) << "Error receiving bundle length from "
               << inet_ntoa(bundleSrc.sin_addr);
      } else {
        LOG(1) << "Error receiving bundle length from "
               << inet_ntoa(bundleSrc.sin_addr)
               << " Length not in the correct format.";
      }
    } else {
      m_listeningAppsTable->update(std::to_string(ntohl(appId)), "", 0, sock);
    }
  }
}
