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
#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <cstdio>
#include <chrono>
#include <algorithm>
#include "Node/BundleQueue/BundleQueue.h"
#include "Node/Neighbour/NeighbourTable.h"
#include "Node/Neighbour/Neighbour.h"
#include "Node/Config.h"
#include "Node/BundleQueue/BundleContainer.h"
#include "Bundle/Bundle.h"
#include "Bundle/PrimaryBlock.h"
#include "Utils/globals.h"
#include "Utils/Logger.h"

BundleProcessor::BundleProcessor(
    Config config, std::shared_ptr<BundleQueue> bundleQueue,
    std::shared_ptr<NeighbourTable> neighbourTable
    /*, std::shared_ptr<ListeningAppsTable> listeningAppsTable*/)
    : m_config(config),
      m_bundleQueue(bundleQueue),
      m_neighbourTable(neighbourTable) {
  std::thread t = std::thread(&BundleProcessor::processBundles, this);
  t.detach();
  t = std::thread(&BundleProcessor::receiveBundles, this);
  t.detach();
}

BundleProcessor::~BundleProcessor() {
}

void BundleProcessor::processBundles() {
  while (!g_stop.load()) {
    try {
      std::unique_ptr<BundleContainer> bc = m_bundleQueue->dequeue();
      processBundle(std::move(bc));
    } catch (const std::exception &e) {
    }
  }
}

void BundleProcessor::receiveBundles() {
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
        while (!g_stop.load()) {
          sockaddr_in clientAddr = { 0 };
          socklen_t clientLen = sizeof(clientAddr);
          int newsock = accept(sock, reinterpret_cast<sockaddr*>(&clientAddr),
                               &clientLen);
          if (newsock == -1) {
            LOG(1) << "Cannot accept connection, reason: " << strerror(errno);
            continue;
          }
          // Set timeout to socket
          setsockopt(newsock, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *) &tv,
                     sizeof(struct timeval));
          std::thread([newsock, this]() {

          }).detach();
        }
      }
    }
  }
}

void BundleProcessor::dispatch(const Bundle& bundle,
                               std::vector<std::string> destinations) {
}

void BundleProcessor::forward(Bundle bundle, std::vector<std::string> nextHop) {
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
          sockaddr_in remoteAddr = {0};
          std::shared_ptr<Neighbour> nb = m_neighbourTable->getNeighbour(nh);
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
                if (connect(sock, reinterpret_cast<sockaddr*>(&remoteAddr),
                        sizeof(remoteAddr)) < 0) {
                  LOG(1) << "Cannot connect with neighbour " << nh
                  << ", reason: " << strerror(errno);
                } else {
                  // 1023 is the max size of an id (RFC 5050)
                  int writed = send(sock, m_config.getNodeId().c_str(), 1024,
                      0);
                  if (writed < 0) {
                    LOG(1) << "Cannot write to socket, reason: "
                    << strerror(errno);
                  } else {
                    writed = send(sock, &bundleLength, sizeof(uint32_t), 0);
                    if (writed < 0) {
                      LOG(1) << "Cannot write to socket, reason: "
                      << strerror(errno);
                    } else {
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
                          LOG(10) << "A bundle of length " << bundleLength
                          << "has been sent to " << nb->getNodeAddress()
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
          }
        });
  }
}

void BundleProcessor::discard(
    std::unique_ptr<BundleContainer> bundleContainer) {
  std::stringstream ss;
  ss << m_config.getDataPath()
     << bundleContainer->getBundle().getPrimaryBlock()->getCreationTimestamp()
     << "_"
     << bundleContainer->getBundle().getPrimaryBlock()
         ->getCreationTimestampSeqNumber()
     << ".bundle";
  int success = std::remove(ss.str().c_str());
  if (success != 0) {
    LOG(3) << "Cannot delete bundle " << ss.str();
  }
  LOG(41) << "Deleting bundleContainer.";
  bundleContainer.reset();
}

void BundleProcessor::restore(
    std::unique_ptr<BundleContainer> bundleContainer) {
  m_bundleQueue->enqueue(std::move(bundleContainer));
}
