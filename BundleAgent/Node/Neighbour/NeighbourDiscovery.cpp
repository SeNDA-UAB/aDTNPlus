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
#include <string>
#include <thread>
#include <chrono>
#include "Node/Neighbour/NeighbourTable.h"
#include "Node/Neighbour/Beacon.h"
#include "Utils/Logger.h"

NeighbourDiscovery::NeighbourDiscovery(ConfigLoader config)
    : m_stop(false),
      m_testMode(false),
      m_config(config),
      m_neighbourCleanerThread(&NeighbourDiscovery::neighbourCleaner, this),
      m_sendBeaconsThread(&NeighbourDiscovery::sendBeacons, this),
      m_receiveBeaconsThread(&NeighbourDiscovery::receiveBeacons, this) {
  m_neighbourCleanerThread.detach();
  m_sendBeaconsThread.detach();
  m_receiveBeaconsThread.detach();
  LOG(68) << "Creating neighbour discovery";
}

NeighbourDiscovery::~NeighbourDiscovery() {
  LOG(68) << "Deleting neighbour discovery";
  m_stop = true;
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
  // Bind the socket.
  bind(sock, reinterpret_cast<sockaddr*>(&discoveryAddr),
       sizeof(discoveryAddr));
  // Generate the destination address.
  sockaddr_in discoveryDestinationAddr = { 0 };
  discoveryDestinationAddr.sin_family = AF_INET;
  discoveryDestinationAddr.sin_port = htons(
      m_config.m_reader.GetInteger("NeighbourDiscovery", "discoveryPort", 0));
  discoveryDestinationAddr.sin_addr.s_addr =
      inet_addr(
          m_config.m_reader.Get("NeighbourDiscovery", "discoveryAddress", "")
              .c_str());
  // Create the beacon with our information.
  LOG(64)
      << "Sending beacons to "
      << m_config.m_reader.Get("NeighbourDiscovery", "discoveryAddress", "")
      << ":"
      << m_config.m_reader.GetInteger("NeighbourDiscovery", "discoveryPort", 0);
  Beacon b = Beacon(nodeId, nodeAddress, nodePort);
  int sleepTime = m_config.m_reader.GetInteger("NeighbourDiscovery",
                                               "discoveryPeriod", 1);
  while (!m_stop.load()) {
    std::this_thread::sleep_for(std::chrono::seconds(sleepTime));
    LOG(14) << "Sending beacon from " << nodeId << " " << nodeAddress << ":"
            << nodePort;
    size_t t = sendto(sock, b.getRaw().c_str(), b.getRaw().size(), 0,
                      reinterpret_cast<sockaddr*>(&discoveryDestinationAddr),
                      sizeof(discoveryDestinationAddr));
  }
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
  int flagOn = 1;
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &flagOn, sizeof(flagOn));
  // Bind the socket.
  bind(sock, reinterpret_cast<sockaddr*>(&discoveryAddr),
       sizeof(discoveryAddr));
  // Join the multicast group.
  ip_mreq mcReq = { 0 };
  mcReq.imr_multiaddr.s_addr = inet_addr(discoveryAddress.c_str());
  mcReq.imr_interface.s_addr = inet_addr(nodeAddress.c_str());
  setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
             reinterpret_cast<void*>(&mcReq), sizeof(mcReq));
  while (!m_stop.load()) {
    char* buffer = reinterpret_cast<char*>(malloc(
        Beacon::MAX_BEACON_SIZE * sizeof(char)));
    size_t recvLength = recv(sock, buffer, Beacon::MAX_BEACON_SIZE, 0);
    // Create a thread to add the new neighbour and let this
    // receiving more beacons
    Beacon b = Beacon(std::string(buffer, recvLength));
    if (b.m_nodeId != nodeId || (b.m_nodeId == nodeId && m_testMode.load())) {
      LOG(15) << "Received beacon from " << b.m_nodeId << " " << b.m_nodeAddress
              << ":" << b.m_nodePort;
      std::thread([b]() {
        NeighbourTable::getInstance()->update(b.m_nodeId, b.m_nodeAddress,
            b.m_nodePort);
      }).detach();
    }
    free(buffer);
  }
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
  while (!m_stop.load()) {
    std::this_thread::sleep_for(std::chrono::seconds(sleepTime));
    LOG(67) << "Calling to clean neighbours";
    NeighbourTable::getInstance()->cleanNeighbours(expirationTime);
  }
}

void NeighbourDiscovery::stop() {
  LOG(16) << "Stopping neighbour discovery";
  m_stop = true;
}

void NeighbourDiscovery::setTestMode(bool mode) {
  LOG(4) << "Setting neighbour discovery to test mode."
         << " This will make this node a neighbour also.";
  m_testMode = mode;
}

