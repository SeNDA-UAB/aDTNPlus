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
 * FILE BundleProcessor.cpp
 * AUTHOR Blackcatn13
 * DATE Dec 17, 2015
 * VERSION 1
 *
 */

#include "Node/BundleProcessor/BundleProcessor.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <cstdio>
#include <chrono>
#include <algorithm>
#include <fstream>
#include "Node/BundleQueue/BundleQueue.h"
#include "Node/Neighbour/NeighbourTable.h"
#include "Node/Neighbour/Neighbour.h"
#include "Node/Config.h"
#include "Node/BundleQueue/BundleContainer.h"
#include "Node/AppListener/ListeningAppsTable.h"
#include "Node/AppListener/App.h"
#include "Bundle/Bundle.h"
#include "Bundle/PrimaryBlock.h"
#include "Bundle/PayloadBlock.h"
#include "Utils/globals.h"
#include "Utils/Logger.h"

BundleProcessor::BundleProcessor(
    Config config, std::shared_ptr<BundleQueue> bundleQueue,
    std::shared_ptr<NeighbourTable> neighbourTable,
    std::shared_ptr<ListeningAppsTable> listeningAppsTable)
    : m_config(config),
      m_bundleQueue(bundleQueue),
      m_neighbourTable(neighbourTable),
      m_listeningAppsTable(listeningAppsTable) {
}

BundleProcessor::~BundleProcessor() {
}

void BundleProcessor::start() {
  LOG(10) << "Starting BundleProcessor";
  std::thread t = std::thread(&BundleProcessor::processBundles, this);
  t.detach();
  t = std::thread(&BundleProcessor::receiveBundles, this);
  t.detach();
}

void BundleProcessor::processBundles() {
  while (!g_stop.load()) {
    try {
      // LOG(60) << "Trying to dequeue a bundle";
      std::unique_ptr<BundleContainer> bc = m_bundleQueue->dequeue();
      processBundle(std::move(bc));
    } catch (const std::exception &e) {
    }
  }
  LOG(12) << "Exit Process bundles thread.";
  g_stopped++;
}

void BundleProcessor::receiveBundles() {
  LOG(10) << "Creating receive bundles thread";
  sockaddr_in receiveAddr = { 0 };
  receiveAddr.sin_family = AF_INET;
  receiveAddr.sin_port = htons(m_config.getNodePort());
  receiveAddr.sin_addr.s_addr = inet_addr(m_config.getNodeAddress().c_str());
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1) {
    // Stop the application
    LOG(1) << "Cannot create socket into receiveBundles thread, reason: "
           << strerror(errno);
    g_stop = true;
  } else {
    if (bind(sock, reinterpret_cast<sockaddr*>(&receiveAddr),
             sizeof(receiveAddr)) == -1) {
      // Stop the application
      LOG(1) << "Cannot bind socket to " << m_config.getNodeAddress()
             << ", reason: " << strerror(errno);
      g_stop = true;
    } else {
      if (listen(sock, 50) != 0) {
        // Stop the application
        LOG(1) << "Cannot set the socket to listen, reason: "
               << strerror(errno);
        g_stop = true;
      } else {
        struct timeval tv;
        tv.tv_sec = 10;
        LOG(10) << "Listening petitions at (" << m_config.getNodeAddress()
                << ":" << m_config.getNodePort() << ")";
        fd_set readfds;
        while (!g_stop.load()) {
          FD_ZERO(&readfds);
          FD_SET(sock, &readfds);
          struct timeval tv1;
          tv1.tv_sec = m_config.getSocketTimeout();
          int sel = select(sock + 1, &readfds, NULL, NULL, &tv1);
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
            LOG(41) << "Connection received.";
            // Set timeout to socket
            setsockopt(newsock, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *) &tv,
                       sizeof(struct timeval));
            std::thread(&BundleProcessor::receiveMessage, this,
                        newsock).detach();
          }
        }
      }
    }
    close(sock);
  }
  LOG(10) << "Exit Receive bundle thread.";
  g_stopped++;
}

void BundleProcessor::receiveMessage(int sock) {
  LOG(41) << "Processing new connection";
  sockaddr_in bundleSrc = { 0 };
  socklen_t bundleSrcLength = sizeof(bundleSrc);
  if (getpeername(sock, reinterpret_cast<sockaddr*>(&bundleSrc),
                  &bundleSrcLength) != 0) {
    LOG(3) << "Cannot get peer name, reason: " << strerror(errno);
  } else {
    LOG(10) << "Receiving bundle from " << inet_ntoa(bundleSrc.sin_addr) << ":"
            << ntohs(bundleSrc.sin_port);
  }
  // First read is the node identifier
  char buff[1024];
  int receivedSize = recv(sock, buff, sizeof(buff), 0);
  if (receivedSize != sizeof(buff)) {
    LOG(1) << "Error receiving origin's platform id from "
           << inet_ntoa(bundleSrc.sin_addr);
  } else {
    std::string srcNodeId = std::string(buff);
    LOG(42) << "Received node id: " << srcNodeId;
    uint32_t bundleLength;
    receivedSize = recv(sock, &bundleLength, sizeof(bundleLength), 0);
    if (receivedSize != sizeof(bundleLength)) {
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
      bundleLength = ntohl(bundleLength);
      LOG(42) << "Received bundle length: " << bundleLength;
      char* rawBundle = new char[bundleLength];
      uint32_t receivedLength = 0;
      LOG(42) << "Receiving bundle...";
      while (receivedLength != bundleLength) {
        receivedSize = recv(sock, rawBundle + receivedLength,
                            bundleLength - receivedLength, 0);
        if (receivedSize == -1) {
          LOG(1) << "Error receiving bundle from "
                 << inet_ntoa(bundleSrc.sin_addr) << ", reason: "
                 << strerror(errno);
          break;
        } else if (receivedSize == 0) {
          LOG(1) << "Peer " << inet_ntoa(bundleSrc.sin_addr)
                 << " closed the connection.";
          break;
        }
        receivedLength += receivedSize;
      }
      if (receivedLength != bundleLength) {
        LOG(1) << "Bundle not received correctly from "
               << inet_ntoa(bundleSrc.sin_addr);
      } else {
        LOG(10) << "Received bundle from " << srcNodeId << "("
                << inet_ntoa(bundleSrc.sin_addr) << ":"
                << ntohs(bundleSrc.sin_port) << ") with length: "
                << receivedLength;
        close(sock);
        std::string bundleStringRaw = std::string(rawBundle, bundleLength);
        try {
          // Create the bundle
          LOG(42) << "Creating bundle from received raw";
          std::unique_ptr<Bundle> b = std::unique_ptr<Bundle>(
              new Bundle(bundleStringRaw));
          LOG(42) << "Creating bundle container";
          // Create the bundleContainer
          std::shared_ptr<Neighbour> neighbour;
          try {
            neighbour = m_neighbourTable->getValue(srcNodeId);
          } catch (const TableException &nte) {
            neighbour = std::make_shared<Neighbour>(srcNodeId, "", 0);
          }
          std::unique_ptr<BundleContainer> bc = createBundleContainer(
              neighbour, std::move(b));
          // Save the bundleContainer to disk
          LOG(42) << "Saving bundle " << bc->getBundle().getId() << " to disk";
          std::ofstream bundleFile;
          std::stringstream ss;
          ss << m_config.getDataPath() << bc->getBundle().getId() << ".bundle";
          bundleFile.open(ss.str(), std::ofstream::out | std::ofstream::binary);
          bundleFile << bc->serialize();
          bundleFile.close();
          // Enqueue the bundleContainer
          LOG(42) << "Saving bundle to queue";
          m_bundleQueue->enqueue(std::move(bc));
        } catch (const BundleCreationException &e) {
          LOG(1) << "Error constructing received bundle, reason: " << e.what();
        }
      }
    }
  }
}

void BundleProcessor::dispatch(Bundle bundle,
                               std::vector<std::string> destinations) {
  LOG(11) << "Dispatching bundle";
  std::string payload = bundle.getPayloadBlock()->getPayload();
  int payloadSize = payload.length();
  std::for_each(
      destinations.begin(), destinations.end(),
      [this, payload, payloadSize] (std::string &appId) {
        try {
          std::shared_ptr<App> app = m_listeningAppsTable->getValue(appId);
          send(app->getSocket(), &payloadSize, sizeof(payloadSize), 0);
          send(app->getSocket(), payload.c_str(), payloadSize, 0);
          LOG(17) << "Send the payload: " << payload << " to the appId: "
          << appId;
        } catch (const TableException &e) {
          LOG(1) << "Error getting appId, reason: " << e.what();
          throw;
        }
      });
}

void BundleProcessor::forward(Bundle bundle, std::vector<std::string> nextHop) {
  LOG(11) << "Forwarding bundle";
  std::string bundleRaw = bundle.toRaw();
// Bundle length, this will limit the max length of a bundle to 2^32 ~ 4GB
  uint32_t bundleLength = bundleRaw.length();
  if (bundleLength <= 0) {
    LOG(1) << "The bundle to forward has a length of 0, aborting forward.";
  } else {
    std::for_each(
        nextHop.begin(),
        nextHop.end(),
        [this, bundleRaw, bundleLength](std::string &nh) {
          LOG(45) << "Forwarding bundle to " << nh;
          LOG(50) << "Bundle to forward " << bundleRaw;
          sockaddr_in remoteAddr = {0};
          std::shared_ptr<Neighbour> nb = m_neighbourTable->getValue(nh);
          remoteAddr.sin_family = AF_INET;
          remoteAddr.sin_port = htons(nb->getNodePort());
          remoteAddr.sin_addr.s_addr = inet_addr(nb->getNodeAddress().c_str());
          int sock = socket(AF_INET, SOCK_STREAM, 0);
          if (sock == -1) {
            LOG(1) << "Cannot create socket into forward thread, reason: "
            << strerror(errno);
          } else {
            struct timeval tv;
            tv.tv_sec = m_config.getNeighbourExpirationTime();
            tv.tv_usec = 0;
            if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,
                    (struct timeval *) &tv, sizeof(struct timeval)) == -1) {
              LOG(1) << "Cannot set receiving timeout to socket, reason: "
              << strerror(errno);
            } else {
              if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO,
                      (struct timeval *) &tv, sizeof(struct timeval)) == -1) {
                LOG(1) << "Cannot set sending timeout to socket, reason: "
                << strerror(errno);
              } else {
                LOG(46) << "Connecting to neighbour...";
                if (connect(sock, reinterpret_cast<sockaddr*>(&remoteAddr),
                        sizeof(remoteAddr)) < 0) {
                  LOG(1) << "Cannot connect with neighbour " << nh
                  << ", reason: " << strerror(errno);
                } else {
                  LOG(46) << "Sending node id: " << m_config.getNodeId();
                  // 1023 is the max size of an id (RFC 5050)
                  int writed = send(sock, m_config.getNodeId().c_str(), 1024,
                      0);
                  if (writed < 0) {
                    LOG(1) << "Cannot write to socket, reason: "
                    << strerror(errno);
                  } else {
                    uint32_t nBundleLength = htonl(bundleLength);
                    LOG(46) << "Sending bundle length: " << bundleLength;
                    writed = send(sock, &nBundleLength, sizeof(nBundleLength),
                        0);
                    if (writed < 0) {
                      LOG(1) << "Cannot write to socket, reason: "
                      << strerror(errno);
                    } else {
                      LOG(46) << "Sending bundle...";
                      writed = send(sock, bundleRaw.c_str(), bundleLength, 0);
                      if (writed < 0) {
                        LOG(1) << "Cannot write to socket, reason: "
                        << strerror(errno);
                      } else {
                        sockaddr_in bundleSrc = {0};
                        socklen_t bundleSrcLength = sizeof(bundleSrc);
                        if (getsockname(sock,
                                reinterpret_cast<sockaddr*>(&bundleSrc),
                                &bundleSrcLength) != 0) {
                          LOG(3) << "Cannot get peer name, reason: "
                          << strerror(errno);
                        } else {
                          LOG(11) << "A bundle of length " << bundleLength
                          << " has been sent to " << nb->getNodeAddress()
                          << ":" << nb->getNodePort() << " from "
                          << inet_ntoa(bundleSrc.sin_addr) << ":"
                          << ntohs(bundleSrc.sin_port);
                        }
                      }
                    }
                  }
                }
              }
            }
            close(sock);
          }
        });
  }
}

void BundleProcessor::discard(
    std::unique_ptr<BundleContainer> bundleContainer) {
  std::stringstream ss;
  ss << m_config.getDataPath() << bundleContainer->getBundle().getId()
     << ".bundle";
  int success = std::remove(ss.str().c_str());
  if (success != 0) {
    LOG(3) << "Cannot delete bundle " << ss.str();
  }
  LOG(51) << "Deleting bundleContainer.";
  bundleContainer.reset();
}

void BundleProcessor::restore(
    std::unique_ptr<BundleContainer> bundleContainer) {
  m_bundleQueue->enqueue(std::move(bundleContainer));
}
