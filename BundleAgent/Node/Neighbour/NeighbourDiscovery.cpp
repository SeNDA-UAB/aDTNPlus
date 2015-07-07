/*
 * Copyright (c) 2015 SeNDA
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
 * FILE NeighbourDiscovery.cpp
 * AUTHOR Blackcatn13
 * DATE Jun 29, 2015
 * VERSION 1
 * This file contains the implementation of the NeighbourDiscovery class.
 */

#include "NeighbourDiscovery.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <thread>
#include <chrono>
#include <stdexcept>
#include "Node/Neighbour/NeighbourTable.h"
#include "Node/Neighbour/Beacon.h"
#include "Utils/Logger.h"
#include "Utils/globals.h"

NeighbourDiscovery::NeighbourDiscovery(ConfigLoader config)
    : m_testMode(false),
      m_config(config) {
  std::thread t = std::thread(&NeighbourDiscovery::neighbourCleaner, this);
  t.detach();
  t = std::thread(&NeighbourDiscovery::sendBeacons, this);
  t.detach();
  t = std::thread(&NeighbourDiscovery::receiveBeacons, this);
  t.detach();
  LOG(68) << "Creating neighbour discovery";
}

NeighbourDiscovery::~NeighbourDiscovery() {
  LOG(68) << "Deleting neighbour discovery";
}

void NeighbourDiscovery::sendBeacons() {
  LOG(14) << "Creating send beacons thread";
// Get node configuration
  std::string nodeId = m_config.m_reader.Get("Node", "nodeId", "");
  std::string nodeAddress = m_config.m_reader.Get("Node", "nodeAddress", "");
  uint16_t nodePort = m_config.m_reader.GetInteger("Node", "nodePort", 0);
// Generate this node address information.
  LOG(64) << "Starting socket into " << nodeAddress << ":0";
  sockaddr_in discoveryAddr = { 0 };
  discoveryAddr.sin_family = AF_INET;
  discoveryAddr.sin_port = htons(0);
  discoveryAddr.sin_addr.s_addr = inet_addr(nodeAddress.c_str());
// Create the socket.
  int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sock == -1) {
    // Stop the application
    LOG(1) << "Cannot create socket into sendBeacons thread reason: "
           << strerror(errno);
    g_stop = true;
  } else {
    // Bind the socket.
    if (bind(sock, reinterpret_cast<sockaddr*>(&discoveryAddr),
             sizeof(discoveryAddr)) == -1) {
      // Stop the application
      LOG(1) << "Cannot bind socket to " << nodeAddress << " reason: "
             << strerror(errno);
      g_stop = true;
    } else {
      // Generate the destination address.
      sockaddr_in discoveryDestinationAddr = { 0 };
      discoveryDestinationAddr.sin_family = AF_INET;
      discoveryDestinationAddr.sin_port = htons(
          m_config.m_reader.GetInteger("NeighbourDiscovery", "discoveryPort",
                                       0));
      discoveryDestinationAddr.sin_addr.s_addr = inet_addr(
          m_config.m_reader.Get("NeighbourDiscovery", "discoveryAddress", "")
              .c_str());
      // Create the beacon with our information.
      LOG(64)
          << "Sending beacons to "
          << m_config.m_reader.Get("NeighbourDiscovery", "discoveryAddress", "")
          << ":"
          << m_config.m_reader.GetInteger("NeighbourDiscovery", "discoveryPort",
                                          0);
      Beacon b = Beacon(nodeId, nodeAddress, nodePort);
      int sleepTime = m_config.m_reader.GetInteger("NeighbourDiscovery",
                                                   "discoveryPeriod", 1);
      while (!g_stop.load()) {
        std::this_thread::sleep_for(std::chrono::seconds(sleepTime));
        LOG(14) << "Sending beacon from " << nodeId << " " << nodeAddress << ":"
                << nodePort;
        std::string rawBeacon = b.getRaw();
        int sendSize = sendto(
            sock, rawBeacon.c_str(), rawBeacon.size(), 0,
            reinterpret_cast<sockaddr*>(&discoveryDestinationAddr),
            sizeof(discoveryDestinationAddr));
        if (sendSize == -1) {
          LOG(1) << "Error sending beacon " << strerror(errno);
        } else if ((size_t) sendSize != rawBeacon.size()) {
          LOG(4) << "Cannot send all the beacon";
        }
      }
    }
    // Close the socket
    close(sock);
  }
  LOG(14) << "Exit Beacon sender thread.";
}

void NeighbourDiscovery::receiveBeacons() {
// Get node configuration
  LOG(15) << "Starting receiver beacon thread";
  std::string nodeId = m_config.m_reader.Get("Node", "nodeId", "");
  std::string nodeAddress = m_config.m_reader.Get("Node", "nodeAddress", "");
  uint16_t discoveryPort = m_config.m_reader.GetInteger("NeighbourDiscovery",
                                                        "discoveryPort", 0);
  std::string discoveryAddress = m_config.m_reader.Get("NeighbourDiscovery",
                                                       "discoveryAddress", "");
// Generate this node address information.
  sockaddr_in discoveryAddr = { 0 };
  discoveryAddr.sin_family = AF_INET;
  discoveryAddr.sin_port = htons(discoveryPort);
  discoveryAddr.sin_addr.s_addr = inet_addr(discoveryAddress.c_str());
  LOG(65) << "Starting socket into " << discoveryAddress << ":"
          << discoveryPort;
// Create the socket.
  int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sock == -1) {
    // Stop the application
    LOG(1) << "Cannot create socket into receiveBeacons thread reason: "
           << strerror(errno);
    g_stop = true;
  } else {
    int flagOn = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &flagOn, sizeof(flagOn))
        == -1) {
      // Stop the application
      LOG(1) << "Cannot set flag REUSEADDR to socket, reason: "
             << strerror(errno);
      g_stop = true;
    } else {
      // Bind the socket.
      if (bind(sock, reinterpret_cast<sockaddr*>(&discoveryAddr),
               sizeof(discoveryAddr)) == -1) {
        // Stop the application
        LOG(1) << "Cannot bind the socket to " << discoveryAddress << ":"
               << discoveryPort << " reason: " << strerror(errno);
        g_stop = true;
      } else {
        // Join the multicast group.
        ip_mreq mcReq = { 0 };
        mcReq.imr_multiaddr.s_addr = inet_addr(discoveryAddress.c_str());
        mcReq.imr_interface.s_addr = inet_addr(nodeAddress.c_str());
        if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                       reinterpret_cast<void*>(&mcReq), sizeof(mcReq)) == -1) {
          // Stop the application
          LOG(1) << "Cannot join the multicast group, reason: "
                 << strerror(errno);
          g_stop = true;
        } else {
          while (!g_stop.load()) {
            char* buffer = reinterpret_cast<char*>(malloc(
                Beacon::MAX_BEACON_SIZE * sizeof(char)));
            size_t recvLength = recv(sock, buffer, Beacon::MAX_BEACON_SIZE, 0);
            // Create a thread to add the new neighbour and let this
            // receiving more beacons
            Beacon b = Beacon(std::string(buffer, recvLength));
            if (b.m_nodeId != nodeId
                || (b.m_nodeId == nodeId && m_testMode.load())) {
              LOG(15) << "Received beacon from " << b.m_nodeId << " "
                      << b.m_nodeAddress << ":" << b.m_nodePort;
              std::thread([b]() {
                NeighbourTable::getInstance()->update(b.m_nodeId,
                    b.m_nodeAddress,
                    b.m_nodePort);
              }).detach();
            }
            free(buffer);
          }
          // Leave from the multicast group
          if (setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP,
                         reinterpret_cast<void*>(&mcReq), sizeof(mcReq))
              == -1) {
            LOG(1) << "Cannot leave the multicast group, reason: "
                   << strerror(errno);
          }
        }
      }
    }
    // Close the socket
    close(sock);
  }
  LOG(15) << "Exit Beacon receiver thread.";
}

void NeighbourDiscovery::neighbourCleaner() {
  int sleepTime = m_config.m_reader.GetInteger("NeighbourDiscovery",
                                               "neighbourCleanerTime", 1);
  int expirationTime = m_config.m_reader.GetInteger("NeighbourDiscovery",
                                                    "neighbourExpirationTime",
                                                    10);
  LOG(16) << "Starting Cleaner thread cleaning every " << sleepTime
          << "s all the nodes with inactivity for a period of "
          << expirationTime << "s";
  while (!g_stop.load()) {
    std::this_thread::sleep_for(std::chrono::seconds(sleepTime));
    LOG(67) << "Calling to clean neighbours";
    NeighbourTable::getInstance()->cleanNeighbours(expirationTime);
  }
  LOG(16) << "Exit Neighbour cleaner thread";
}

void NeighbourDiscovery::setTestMode(bool mode) {
  LOG(4) << "Setting neighbour discovery test mode to " << mode
         << " If true this will make this node a neighbour also.";
  m_testMode = mode;
}

