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

#include "Node/Neighbour/NeighbourDiscovery.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <thread>
#include <chrono>
#include <vector>
#include <stdexcept>
#include "Node/Neighbour/NeighbourTable.h"
#include "Node/EndpointListener/ListeningEndpointsTable.h"
#include "Node/Neighbour/Beacon.h"
#include "Utils/Logger.h"
#include "Utils/globals.h"
#include "Utils/Socket.h"

#define LOG_NEIGHBOUR 98 //before 22
#define LOG_CLEAN 98 //before 67
#define LOG_START_CLEAN_THREAD 98//before 16

NeighbourDiscovery::NeighbourDiscovery(
    Config config, std::shared_ptr<NeighbourTable> neighbourTable,
    std::shared_ptr<ListeningEndpointsTable> listeningEndpointsTable)
    : m_config(config),
      m_neighbourTable(neighbourTable),
      m_listeningEndpointsTable(listeningEndpointsTable) {
  std::thread t = std::thread(&NeighbourDiscovery::cleanNeighbours, this);
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
  Logger::getInstance()->setThreadName(std::this_thread::get_id(),
                                       "Beacon Sender");
  LOG(14) << "Creating send beacons thread";
  // Get node configuration
  std::string nodeId = m_config.getNodeId();
  std::string nodeAddress = m_config.getNodeAddress();
  uint16_t nodePort = m_config.getNodePort();
  // Generate this node address information.
  LOG(64) << "Starting socket into " << nodeAddress << ":0";
  // Create the socket.
  Socket s = Socket(false);
  if (!s) {
    // Stop the application
    LOG(1) << "Cannot create socket into sendBeacons thread, reason: "
           << s.getLastError();
    g_stop = true;
  } else {
    // Bind the socket.
    if (!s.bind(nodeAddress, 0)) {
      // Stop the application
      LOG(1) << "Cannot bind socket to " << nodeAddress << ", reason: "
             << strerror(errno);
      g_stop = true;
    } else {
      // Generate the destination address.
      s.setDestination(m_config.getDiscoveryAddress(),
                       m_config.getDiscoveryPort());
      // Create the beacon with our information.
      LOG(64) << "Sending beacons to " << m_config.getDiscoveryAddress() << ":"
              << m_config.getDiscoveryPort();
      int sleepTime = m_config.getDiscoveryPeriod();
      g_startedThread++;
      while (!g_stop.load()) {
        std::this_thread::sleep_for(std::chrono::seconds(sleepTime));
        Beacon b = Beacon(nodeId, nodeAddress, nodePort,
                          m_listeningEndpointsTable->getValues());
        LOG(LOG_NEIGHBOUR) << "Sending beacon from " << nodeId << " " << nodeAddress << ":"
                << nodePort;
        std::string rawBeacon = b.getRaw();
        if (!(s << rawBeacon)) {
          LOG(4) << "Error sending beacon " << strerror(errno);
        }
      }
      s.close();
    }
    LOG(14) << "Exit Beacon sender thread.";
    g_stopped++;
  }
}

void NeighbourDiscovery::receiveBeacons() {
  // Get node configuration
  Logger::getInstance()->setThreadName(std::this_thread::get_id(),
      "Beacon Receiver");
  LOG(15) << "Starting receiver beacon thread";
  std::string nodeId = m_config.getNodeId();
  std::string nodeAddress = m_config.getNodeAddress();
  uint16_t discoveryPort = m_config.getDiscoveryPort();
  std::string discoveryAddress = m_config.getDiscoveryAddress();
  bool testMode = m_config.getNeighbourTestMode();
  // Generate this node address information.
  LOG(65) << "Starting socket into " << discoveryAddress << ":"
  << discoveryPort;
  // Create the socket.
  Socket s = Socket(false);
  if (!s) {
    // Stop the application
    LOG(1) << "Cannot create socket into receiveBeacons thread reason: "
    << s.getLastError();
    g_stop = true;
  } else {
    if (!s.setReuseAddress()) {
      // Stop the application
      LOG(1) << "Cannot set flag REUSEADDR to socket, reason: "
      << s.getLastError();
      g_stop = true;
    } else {
      // Bind the socket.
      if (!s.bind(discoveryAddress, discoveryPort)) {
        // Stop the application
        LOG(1) << "Cannot bind the socket to " << discoveryAddress << ":"
        << discoveryPort << " reason: " << s.getLastError();
        g_stop = true;
      } else {
        // Join the multicast group.
        if (!s.joinMulticastGroup(nodeAddress)) {
          // Stop the application
          LOG(1) << "Cannot join the multicast group, reason: "
          << s.getLastError();
          g_stop = true;
        } else {
          // Add a timeout to the recv socket
          s.setRcvTimeOut(m_config.getSocketTimeout());
          g_startedThread++;
          while (!g_stop.load()) {
            uint32_t beaconLength = 65507;
            std::string buffer;
            StringWithSize sws = StringWithSize(buffer, beaconLength);
            s >> sws;
            // Create a thread to add the new neighbour and let this
            // receiving more beacons
            Beacon b = Beacon(buffer);
            if (b.getNodeId() != nodeId || testMode) {
              LOG(LOG_NEIGHBOUR) << "Received beacon from " << b.getNodeId() << " "
                      << b.getNodeAddress() << ":" << b.getNodePort();
              std::thread([b, this]() {
                    m_neighbourTable->update(std::make_shared<Neighbour>(
                            b.getNodeId(),
                            b.getNodeAddress(),
                            b.getNodePort(),
                            b.getEndpoints()));
                  }).detach();
            }
          }

          // Leave from the multicast group
          if (!s.leaveMulticastGroup()) {
            LOG(4) << "Cannot leave the multicast group, reason: "
            << s.getLastError();
          }
        }
      }
    }
    s.close();
  }
  LOG(15) << "Exit Beacon receiver thread.";
  g_stopped++;
}

void NeighbourDiscovery::cleanNeighbours() {
  Logger::getInstance()->setThreadName(std::this_thread::get_id(),
      "Neighbour cleaner");
  int sleepTime = m_config.getNeighbourCleanerTime();
  int expirationTime = m_config.getNeighbourExpirationTime();
  LOG(LOG_START_CLEAN_THREAD) << "Starting Cleaner thread cleaning every " << sleepTime
          << "s all the nodes with inactivity for a period of "
          << expirationTime << "s";
  g_startedThread++;
  while (!g_stop.load()) {
    std::this_thread::sleep_for(std::chrono::seconds(sleepTime));
    LOG(LOG_CLEAN) << "Calling to clean neighbours";
    m_neighbourTable->clean(expirationTime);
  }
  LOG(LOG_START_CLEAN_THREAD) << "Exit Neighbour cleaner thread";
  g_stopped++;
}

