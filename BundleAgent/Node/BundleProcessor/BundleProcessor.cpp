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
#include <chrono>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <map>
#include "Node/BundleQueue/BundleQueue.h"
#include "Node/Neighbour/NeighbourTable.h"
#include "Node/Neighbour/Neighbour.h"
#include "Node/Config.h"
#include "Node/BundleQueue/BundleContainer.h"
#include "Node/EndpointListener/ListeningEndpointsTable.h"
#include "Node/EndpointListener/Endpoint.h"
#include "Bundle/Bundle.h"
#include "Bundle/PrimaryBlock.h"
#include "Bundle/PayloadBlock.h"
#include "Utils/globals.h"
#include "Utils/Logger.h"
#include "Utils/TimestampManager.h"
#include "Utils/Functions.h"

BundleProcessor::BundleProcessor() {
}

BundleProcessor::~BundleProcessor() {
}

void BundleProcessor::start(
    Config config, std::shared_ptr<BundleQueue> bundleQueue,
    std::shared_ptr<NeighbourTable> neighbourTable,
    std::shared_ptr<ListeningEndpointsTable> listeningAppsTable) {
  m_config = config;
  m_bundleQueue = bundleQueue;
  m_neighbourTable = neighbourTable;
  m_listeningAppsTable = listeningAppsTable;
  LOG(10) << "Starting BundleProcessor";
  std::thread t = std::thread(&BundleProcessor::processBundles, this);
  t.detach();
  t = std::thread(&BundleProcessor::receiveBundles, this);
  t.detach();
}

void BundleProcessor::processBundles() {
  Logger::getInstance()->setThreadName(std::this_thread::get_id(),
                                       "Bundle Processor");
  g_startedThread++;
  g_processed = 0;
  while (!g_stop.load()) {
    g_queueSize = m_bundleQueue->getSize();
    while (g_processed.load() < g_queueSize.load() && !g_stop.load()) {
      if (g_processed.load() == 0) {
        g_queueSize = m_bundleQueue->getSize();
      }
      try {
        LOG(60) << "Checking for bundles in the queue";
        m_bundleQueue->wait_for(m_config.getSocketTimeout());
        std::unique_ptr<BundleContainer> bc = m_bundleQueue->dequeue();
        processBundle(std::move(bc));
        g_processed++;
      } catch (const std::exception &e) {
      }
    }
    std::unique_lock<std::mutex> lck(g_processorMutex);
    while (g_processorConditionVariable.wait_for(
        lck, std::chrono::seconds(m_config.getSocketTimeout()))
        == std::cv_status::timeout) {
      break;
    }
    lck.unlock();
  }
  LOG(12) << "Exit Process bundles thread.";
  g_stopped++;
}

void BundleProcessor::receiveBundles() {
  Logger::getInstance()->setThreadName(std::this_thread::get_id(),
                                       "Bundle Receiver");
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
        g_startedThread++;
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
              LOG(3) << "Cannot accept connection, reason: " << strerror(errno);
              continue;
            }
            LOG(41) << "Connection received.";
            // Set timeout to socket
            setsockopt(newsock, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *) &tv,
                       sizeof(struct timeval));
            std::thread(&BundleProcessor::receiveMessage, this, newsock).detach();
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
  Logger::getInstance()->setThreadName(std::this_thread::get_id(),
                                       "Bundle connection");
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
  char buff[1024];
  int receivedSize = recv(sock, buff, sizeof(buff), 0);
  if (receivedSize != sizeof(buff)) {
    LOG(3) << "Error receiving origin's platform id from "
           << inet_ntoa(bundleSrc.sin_addr);
    close(sock);
  } else {
    std::string srcNodeId = std::string(buff);
    LOG(42) << "Received node id: " << srcNodeId;
    uint32_t bundleLength;
    receivedSize = recv(sock, &bundleLength, sizeof(bundleLength), 0);
    if (receivedSize != sizeof(bundleLength)) {
      if (receivedSize == 0) {
        LOG(3) << "Error receiving bundle length from "
               << inet_ntoa(bundleSrc.sin_addr)
               << " Probably peer has disconnected.";
      } else if (receivedSize < 0) {
        LOG(3) << "Error receiving bundle length from "
               << inet_ntoa(bundleSrc.sin_addr);
      } else {
        LOG(3) << "Error receiving bundle length from "
               << inet_ntoa(bundleSrc.sin_addr)
               << " Length not in the correct format.";
      }
      close(sock);
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
          LOG(3) << "Error receiving bundle from "
                 << inet_ntoa(bundleSrc.sin_addr) << ", reason: "
                 << strerror(errno);
          break;
        } else if (receivedSize == 0) {
          LOG(3) << "Peer " << inet_ntoa(bundleSrc.sin_addr)
                 << " closed the connection.";
          break;
        }
        receivedLength += receivedSize;
      }
      if (receivedLength != bundleLength) {
        LOG(3) << "Bundle not received correctly from "
               << inet_ntoa(bundleSrc.sin_addr);
        close(sock);
      } else {
        uint8_t ack = static_cast<uint8_t>(BundleACK::CORRECT_RECEIVED);
        LOG(10) << "Received bundle from " << inet_ntoa(bundleSrc.sin_addr)
                << ":" << ntohs(bundleSrc.sin_port) << " with length: "
                << receivedLength;
        std::string bundleStringRaw = std::string(rawBundle, bundleLength);
        try {
          // Create the bundle
          LOG(42) << "Creating bundle from received raw";
          std::unique_ptr<Bundle> b = std::unique_ptr<Bundle>(
              new Bundle(bundleStringRaw));
          // If the source node is the library, change the timestamp to a one
          // generated from this node
          if (srcNodeId == "_ADTN_LIB_") {
            b->getPrimaryBlock()->setTimestamp(
                TimestampManager::getInstance()->getTimestamp());
            // If the source of the bundle is the library,
            // change it to this node id.
            if (b->getPrimaryBlock()->getSource() == "_ADTN_LIB_") {
              b->getPrimaryBlock()->setSource(m_config.getNodeId());
            }
          }
          LOG(42) << "Creating bundle container";
          // Create the bundleContainer
          std::unique_ptr<BundleContainer> bc = createBundleContainer(
              std::move(b));
          // Save the bundleContainer to disk
          LOG(42) << "Saving bundle " << bc->getBundle().getId() << " to disk";
          m_bundleQueue->saveBundleToDisk(m_config.getDataPath(), *bc);
          // Execute process control
          processControl(*bc);
          // Enqueue the bundleContainer
          LOG(42) << "Saving bundle to queue";
          try {
            m_bundleQueue->enqueue(std::move(bc));
          } catch (const DroppedBundleQueueException &e) {
            std::stringstream ss;
            ss << m_config.getDataPath() << bc->getBundle().getId()
               << ".bundle";
            int success = std::remove(ss.str().c_str());
            if (success != 0) {
              LOG(3) << "Cannot delete bundle " << ss.str();
            }
            LOG(40) << e.what();
            drop();
            ack = static_cast<uint8_t>(BundleACK::QUEUE_FULL);
          } catch (const InBundleQueueException &e) {
            LOG(40) << e.what();
            ack = static_cast<uint8_t>(BundleACK::ALREADY_IN_QUEUE);
          }
          // Sending ACK
          LOG(42) << "Sending Bundle ACK: " << static_cast<unsigned int>(ack);
          int writed = send(sock, &ack, sizeof(ack), 0);
          if (writed < 0) {
            LOG(3) << "Cannot write to socket, reason: " << strerror(errno);
          }
          close(sock);
          // Notify Processor that a new bundle can be processed
          g_processed = 0;
          std::unique_lock<std::mutex> lck(g_processorMutex);
          g_processorConditionVariable.notify_one();
        } catch (const BundleCreationException &e) {
          LOG(3) << "Error constructing received bundle, reason: " << e.what();
        }
      }
    }
  }
}

void BundleProcessor::delivery(BundleContainer &bundleContainer,
                               std::vector<std::string> destinations) {
  LOG(11) << "Dispatching bundle";
  std::string payload = bundleContainer.getBundle().toRaw();
  int payloadSize = payload.length();
  for (auto destination : destinations) {
    try {
      auto endpoints = m_listeningAppsTable->getValue(destination);
      for (auto endpoint : endpoints) {
        if (!endpoint->checkDeliveredId(bundleContainer.getBundle().getId())) {
          int sended = send(endpoint->getSocket(), &payloadSize,
                            sizeof(payloadSize), 0);
          if (sended < 0) {
            LOG(11) << "Saving not delivered bundle to disk.";
            m_bundleQueue->saveBundleToDisk(m_config.getDeliveryPath(),
                                            bundleContainer);
            continue;
          }
          send(endpoint->getSocket(), payload.c_str(), payloadSize, 0);
          LOG(60) << "Send the payload: " << payload << " to the appId: "
                  << destination;
          endpoint->addDeliveredId(bundleContainer.getBundle().getId());
        } else {
          LOG(11) << "Saving trashed bundle to disk.";
          m_bundleQueue->saveBundleToDisk(m_config.getTrashDelivery(),
                                          bundleContainer, true);
        }
      }
    } catch (const TableException &e) {
      LOG(3) << "Error getting appId, reason: " << e.what();
      LOG(11) << "Saving not delivered bundle to disk.";
      m_bundleQueue->saveBundleToDisk(m_config.getDeliveryPath(),
                                      bundleContainer);
    }
  }
}

void BundleProcessor::forward(Bundle bundle, std::vector<std::string> nextHop) {
  LOG(11) << "Forwarding bundle";
  std::string bundleRaw = bundle.toRaw();
// Bundle length, this will limit the max length of a bundle to 2^32 ~ 4GB
  uint32_t bundleLength = bundleRaw.length();
  if (bundleLength <= 0) {
    LOG(3) << "The bundle to forward has a length of 0, aborting forward.";
  } else {
    auto forwardFunction =
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
            std::stringstream ss;
            ss << "Cannot create socket into forward thread, reason: "
            << strerror(errno);
            throw ForwardNetworkException(ss.str(),
                static_cast<uint8_t>(NetworkError::SOCKET_ERROR));
          } else {
            struct timeval tv;
            tv.tv_sec = m_config.getNeighbourExpirationTime();
            tv.tv_usec = 0;
            if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,
                    (struct timeval *) &tv, sizeof(struct timeval)) == -1) {
              std::stringstream ss;
              ss << "Cannot set receiving timeout to socket, reason: "
              << strerror(errno);
              close(sock);
              throw ForwardNetworkException(ss.str(),
                  static_cast<uint8_t>(NetworkError::SOCKET_TIMEOUT_ERROR));
            } else {
              if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO,
                      (struct timeval *) &tv, sizeof(struct timeval)) == -1) {
                std::stringstream ss;
                ss << "Cannot set sending timeout to socket, reason: "
                << strerror(errno);
                close(sock);
                throw ForwardNetworkException(ss.str(),
                    static_cast<uint8_t>(NetworkError::SOCKET_TIMEOUT_ERROR));
              } else {
                LOG(46) << "Connecting to neighbour...";
                if (connect(sock, reinterpret_cast<sockaddr*>(&remoteAddr),
                        sizeof(remoteAddr)) < 0) {
                  std::stringstream ss;
                  ss << "Cannot connect with neighbour " << nh
                  << ", reason: " << strerror(errno);
                  close(sock);
                  throw ForwardNetworkException(ss.str(),
                      static_cast<uint8_t>(NetworkError::SOCKET_CONNECT_ERROR));
                } else {
                  LOG(46) << "Sending node id: " << m_config.getNodeId();
                  int writed = send(sock, m_config.getNodeId().c_str(), 1024, 0);
                  if (writed < 0) {
                    std::stringstream ss;
                    ss << "Cannot write to socket, reason: "
                    << strerror(errno);
                    close(sock);
                    throw ForwardNetworkException(ss.str(),
                        static_cast<uint8_t>(NetworkError::SOCKET_WRITE_ERROR));
                  } else {
                    uint32_t nBundleLength = htonl(bundleLength);
                    LOG(46) << "Sending bundle length: " << bundleLength;
                    writed = send(sock, &nBundleLength, sizeof(nBundleLength),
                        0);
                    if (writed < 0) {
                      std::stringstream ss;
                      ss << "Cannot write to socket, reason: "
                      << strerror(errno);
                      close(sock);
                      throw ForwardNetworkException(ss.str(),
                          static_cast<uint8_t>(NetworkError::SOCKET_WRITE_ERROR));
                    } else {
                      LOG(46) << "Sending bundle...";
                      uint32_t bundleSizeSend = 0;
                      while (bundleSizeSend < bundleLength) {
                        writed = send(sock, bundleRaw.c_str() + bundleSizeSend,
                            bundleLength - bundleSizeSend, 0);
                        if (writed < 0) {
                          std::stringstream ss;
                          ss << "Cannot write to socket, reason: "
                          << strerror(errno);
                          close(sock);
                          throw ForwardNetworkException(ss.str(),
                              static_cast<uint8_t>(NetworkError::SOCKET_WRITE_ERROR));
                        }
                        bundleSizeSend += writed;
                      }
                      uint8_t ack;
                      int receivedSize = recv(sock, &ack, sizeof(ack), 0);
                      if (receivedSize != sizeof(ack)) {
                        if (receivedSize == 0) {
                          std::stringstream ss;
                          ss << "Error receiving bundle ACK"
                          << " Probably peer has disconnected.";
                          close(sock);
                          throw ForwardNetworkException(ss.str(),
                              static_cast<uint8_t>(NetworkError::SOCKET_RECEIVE_ERROR));
                        } else if (receivedSize < 0) {
                          std::stringstream ss;
                          ss << "Error receiving bundle ACK";
                          close(sock);
                          throw ForwardNetworkException(ss.str(),
                              static_cast<uint8_t>(NetworkError::SOCKET_RECEIVE_ERROR));
                        } else {
                          std::stringstream ss;
                          ss << "Error receiving bundle ACK"
                          << " Length not in the correct format.";
                          close(sock);
                          throw ForwardNetworkException(ss.str(),
                              static_cast<uint8_t>(NetworkError::SOCKET_RECEIVE_ERROR));
                        }
                      } else {
                        LOG(46) << "Received bundle ACK: " << static_cast<unsigned int>(ack);
                        if (ack == static_cast<uint8_t>(BundleACK::CORRECT_RECEIVED)) {
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
                        } else {
                          std::stringstream ss;
                          uint8_t error;
                          if (ack == static_cast<uint8_t>(BundleACK::ALREADY_IN_QUEUE)) {
                            ss << "Node already has the bundle in queue.";
                            error = static_cast<uint8_t>(NetworkError::NEIGHBOUR_IN_QUEUE);
                          } else if (ack == static_cast<uint8_t>(BundleACK::QUEUE_FULL)) {
                            ss << "Full queue of neighbour.";
                            error = static_cast<uint8_t>(NetworkError::NEIGHBOUR_FULL_QUEUE);
                          } else {
                            ss << "Bad ack received.";
                            error = static_cast<uint8_t>(NetworkError::NEIGHBOUR_BAD_ACK);
                          }
                          throw ForwardNetworkException(ss.str(), error);
                        }
                      }
                    }
                  }
                }
              }
            }
            close(sock);
          }
        };
    int hops = 0;
    std::map<std::string, uint8_t> errors;
    for (auto hop : nextHop) {
      try {
        forwardFunction(hop);
        hops++;
      } catch (const ForwardNetworkException &e) {
        LOG(10) << e.what();
        errors[hop] = e.error();
      }
    }
    if (hops == 0) {
      g_processed--;
      throw ForwardException("The bundle has not been send to any neighbour.",
                             errors);
    }
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
  m_bundleQueue->resetLast();
}

void BundleProcessor::restore(
    std::unique_ptr<BundleContainer> bundleContainer) {
  try {
    m_bundleQueue->resetLast();
    m_bundleQueue->enqueue(std::move(bundleContainer));
  } catch (const DroppedBundleQueueException &e) {
    LOG(40) << e.what();
    drop();
  }
}

void BundleProcessor::restoreRawBundleContainer(const std::string &data) {
  try {
    std::unique_ptr<BundleContainer> bundleContainer = std::unique_ptr<
        BundleContainer>(new BundleContainer(data));
    try {
      m_bundleQueue->enqueue(std::move(bundleContainer));
    } catch (const DroppedBundleQueueException &e) {
      LOG(40) << e.what();
      drop();
    }
  } catch (const BundleContainerCreationException &e) {
    LOG(3) << e.what();
  }
}

void BundleProcessor::drop() {
}

void BundleProcessor::processControl(BundleContainer &bundleContainer) {
}
