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
#include "Utils/PerfLogger.h"
#include "Utils/TimestampManager.h"
#include "Utils/Functions.h"
#include "Utils/Socket.h"

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
  while (!g_stop.load()) {
    uint32_t oldValue;
    while (((oldValue = g_queueProcessEvents) > 0) && !g_stop.load()) {
      uint32_t queueSize = m_bundleQueue->getSize();
      uint32_t i = 0;
      while (i < queueSize && !g_stop.load()) {
        try {
          LOG(60) << "Checking for bundles in the queue";
          m_bundleQueue->wait_for(m_config.getProcessTimeout());
          std::unique_ptr<BundleContainer> bc = m_bundleQueue->dequeue();
          if (!processBundle(std::move(bc))) {
            g_queueProcessEvents++;
          }
          i++;
        } catch (const std::exception &e) {
        }
      }
      g_queueProcessEvents -= oldValue;
    }
    std::unique_lock<std::mutex> lck(g_processorMutex);
    if (g_processorConditionVariable.wait_for(
        lck, std::chrono::seconds(m_config.getProcessTimeout()))
        == std::cv_status::timeout) {
      g_queueProcessEvents++;
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
  Socket s = Socket();
  if (!s) {
    // Stop the application
    LOG(1) << "Cannot create socket into receiveBundles thread, reason: "
           << s.getLastError();
    g_stop = true;
  } else {
    if (!s.bind(m_config.getNodeAddress(), m_config.getNodePort())) {
      // Stop the application
      LOG(1) << "Cannot bind socket to " << m_config.getNodeAddress()
             << ", reason: " << s.getLastError();
      g_stop = true;
    } else {
      if (!s.listen(50)) {
        // Stop the application
        LOG(1) << "Cannot set the socket to listen, reason: "
               << s.getLastError();
        g_stop = true;
      } else {
        LOG(10) << "Listening petitions at (" << m_config.getNodeAddress()
                << ":" << m_config.getNodePort() << ")";
        g_startedThread++;
        while (!g_stop.load()) {
          Socket newSocket = Socket(-1);
          if (!s.accept(m_config.getSocketTimeout(), newSocket)) {
            continue;
          }
          if (!newSocket) {
            LOG(3) << "Cannot accept connection, reason: "
                   << newSocket.getLastError();
            continue;
          }
          LOG(41) << "Connection received.";
          // Set timeout to socket
          newSocket.setRcvTimeOut(m_config.getSocketTimeout());
          std::thread(&BundleProcessor::receiveMessage, this, newSocket).detach();
        }
      }
    }
    s.close();
  }
  LOG(10) << "Exit Receive bundle thread.";
  g_stopped++;
}

void BundleProcessor::receiveMessage(Socket sock) {
  Logger::getInstance()->setThreadName(std::this_thread::get_id(),
                                       "Bundle connection");
  LOG(41) << "Processing new connection";
  LOG(10) << "Receiving bundle from " << sock.getPeerName();
  std::string srcNodeId;
  StringWithSize sws = StringWithSize(srcNodeId, 1024);
  if (!(sock >> sws)) {
    LOG(3) << "Error receiving origin's platform id from "
           << sock.getPeerName();
    sock.close();
  } else {
    srcNodeId = std::string(srcNodeId.c_str());
    LOG(42) << "Received node id: " << srcNodeId;
    uint32_t bundleLength;
    if (!(sock >> bundleLength)) {
      LOG(3) << "Error receiving bundle length from " << sock.getPeerName()
             << sock.getLastError();
      sock.close();
    } else {
      LOG(42) << "Received bundle length: " << bundleLength;
      std::string bundleStringRaw;
      StringWithSize sws1 = StringWithSize(bundleStringRaw, bundleLength);
      LOG(42) << "Receiving bundle...";
      if (!(sock >> sws1)) {
        LOG(3) << "Bundle not received correctly from " << sock.getPeerName()
               << sock.getLastError();
        sock.close();
      } else {
        uint8_t ack = static_cast<uint8_t>(BundleACK::CORRECT_RECEIVED);
        LOG(10) << "Received bundle from " << sock.getPeerName()
                << " with length: " << bundleLength;
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
          std::string bundleId = bc->getBundle().getId();
          LOG(42) << "Saving bundle " << bundleId << " to disk";
          m_bundleQueue->saveBundleToDisk(m_config.getDataPath(), *bc);
          // Execute process control
          processControl(*bc);
          // Enqueue the bundleContainer
          LOG(42) << "Saving bundle to queue";
          try {
            m_bundleQueue->enqueue(std::move(bc));
            // Notify Processor that a new bundle can be processed
            g_queueProcessEvents++;
            std::unique_lock<std::mutex> lck(g_processorMutex);
            g_processorConditionVariable.notify_one();

          } catch (const DroppedBundleQueueException &e) {
            std::stringstream ss;
            ss << m_config.getDataPath() << bundleId << ".bundle";
            int success = std::remove(ss.str().c_str());
            if (success != 0) {
              LOG(3) << "Cannot delete bundle " << ss.str();
            }
            LOG(40) << e.what();
            drop();
            ack = static_cast<uint8_t>(BundleACK::QUEUE_FULL);
            PERF(PerfMessages::MESSAGE_DROPPED) << bundleId;
          } catch (const InBundleQueueException &e) {
            LOG(40) << e.what();
            ack = static_cast<uint8_t>(BundleACK::ALREADY_IN_QUEUE);
          }
          // Sending ACK
          LOG(42) << "Sending Bundle ACK: " << static_cast<unsigned int>(ack);
          if (!(sock << ack)) {
            LOG(3) << "Cannot write to socket, reason: " << sock.getLastError();
          }
          if (srcNodeId == "_ADTN_LIB_") {
            PERF(PerfMessages::MESSAGE_CREATED) << bundleId;
          } else if (ack == static_cast<uint8_t>(BundleACK::CORRECT_RECEIVED)) {
            PERF(PerfMessages::MESSAGE_RELAYED_FROM) << bundleId << " "
                                                     << sock.getPeerName()
                                                     << " " << bundleLength;
          }
          sock.close();
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
  uint32_t payloadSize = payload.length();
  for (auto destination : destinations) {
    try {
      auto endpoints = m_listeningAppsTable->getValue(destination);
      for (auto endpoint : endpoints) {
        if (!endpoint->checkDeliveredId(bundleContainer.getBundle().getId())) {
          if (!(endpoint->getSocket() << payloadSize)) {
            LOG(11) << "Saving not delivered bundle to disk.";
            m_bundleQueue->saveBundleToDisk(m_config.getDeliveryPath(),
                                            bundleContainer);
            continue;
          }
          endpoint->getSocket() << payload;
          LOG(1) << endpoint->getSocket().getLastError();
          LOG(60) << "Send the payload: " << payload << " to the appId: "
                  << destination;
          endpoint->addDeliveredId(bundleContainer.getBundle().getId());
        } else {
          LOG(11) << "Saving trashed bundle to disk.";
          m_bundleQueue->saveBundleToDisk(m_config.getTrashDelivery(),
                                          bundleContainer, true);
        }
      }
      PERF(MESSAGE_RECEIVED)
          << bundleContainer.getBundle().getId() << " " << payloadSize << " "
          << BundleInfo(bundleContainer.getBundle()).getCreationTimestamp();
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
  std::string bundleId = bundle.getId();
  if (bundleLength <= 0) {
    LOG(3) << "The bundle to forward has a length of 0, aborting forward.";
  } else {
    auto forwardFunction =
        [this, bundleRaw, bundleLength, bundleId](std::string &nh) {
          LOG(45) << "Forwarding bundle to " << nh;
          LOG(50) << "Bundle to forward " << bundleRaw;
          std::shared_ptr<Neighbour> nb = m_neighbourTable->getValue(nh);
          Socket s = Socket();
          if (!s) {
            std::stringstream ss;
            ss << "Cannot create socket into forward thread, reason: "
            << s.getLastError();
            throw ForwardNetworkException(ss.str(),
                static_cast<uint8_t>(NetworkError::SOCKET_ERROR));
          } else {
            if (!s.setRcvTimeOut(m_config.getNeighbourExpirationTime())) {
              std::stringstream ss;
              ss << "Cannot set receiving timeout to socket, reason: "
              << s.getLastError();
              s.close();
              throw ForwardNetworkException(ss.str(),
                  static_cast<uint8_t>(NetworkError::SOCKET_TIMEOUT_ERROR));
            } else {
              if (!s.setSendTimeOut(m_config.getNeighbourExpirationTime())) {
                std::stringstream ss;
                ss << "Cannot set sending timeout to socket, reason: "
                << s.getLastError();
                s.close();
                throw ForwardNetworkException(ss.str(),
                    static_cast<uint8_t>(NetworkError::SOCKET_TIMEOUT_ERROR));
              } else {
                LOG(46) << "Connecting to neighbour...";
                if (!s.connect(nb->getNodeAddress(), nb->getNodePort())) {
                  std::stringstream ss;
                  ss << "Cannot connect with neighbour " << nh
                  << ", reason: " << s.getLastError();
                  s.close();
                  throw ForwardNetworkException(ss.str(),
                      static_cast<uint8_t>(NetworkError::SOCKET_CONNECT_ERROR));
                } else {
                  LOG(46) << "Sending node id: " << m_config.getNodeId();
                  ConstStringWithSize csws = ConstStringWithSize(m_config.getNodeId(), 1024);
                  if (!(s << csws)) {
                    std::stringstream ss;
                    ss << "Cannot write to socket, reason: "
                    << s.getLastError();
                    s.close();
                    throw ForwardNetworkException(ss.str(),
                        static_cast<uint8_t>(NetworkError::SOCKET_WRITE_ERROR));
                  } else {
                    LOG(46) << "Sending bundle length: " << bundleLength;
                    if (!(s << bundleLength)) {
                      std::stringstream ss;
                      ss << "Cannot write to socket, reason: "
                      << s.getLastError();
                      s.close();
                      throw ForwardNetworkException(ss.str(),
                          static_cast<uint8_t>(NetworkError::SOCKET_WRITE_ERROR));
                    } else {
                      LOG(46) << "Sending bundle...";
                      if (!(s << bundleRaw)) {
                        std::stringstream ss;
                        ss << "Cannot write to socket, reason: "
                        << s.getLastError();
                        s.close();
                        throw ForwardNetworkException(ss.str(),
                            static_cast<uint8_t>(NetworkError::SOCKET_WRITE_ERROR));
                      }
                      uint8_t ack;
                      if (!(s >> ack)) {
                        std::stringstream ss;
                        ss << "Error receiving bundle ACK";
                        s.close();
                        throw ForwardNetworkException(ss.str(),
                            static_cast<uint8_t>(NetworkError::SOCKET_RECEIVE_ERROR));
                      } else {
                        LOG(46) << "Received bundle ACK: " << static_cast<unsigned int>(ack);
                        if (ack == static_cast<uint8_t>(BundleACK::CORRECT_RECEIVED) || ack == static_cast<uint8_t>(BundleACK::QUEUE_FULL)) {
                          LOG(11) << "A bundle of length " << bundleLength
                          << " has been sent to " << nb->getNodeAddress()
                          << ":" << nb->getNodePort() << " from "
                          << s.getPeerName();
                          PERF(MESSAGE_RELAYED) << bundleId << " " << s.getPeerName() << " " << bundleLength;
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
            s.close();
          }
        };
    int hops = 0;
    std::map<std::string, uint8_t> errors;
    std::vector<std::string> minNextHop = m_neighbourTable->getMinNeighbours(nextHop);
    for (auto hop : minNextHop) {
      try {
        forwardFunction(hop);
        hops++;
      } catch (const ForwardNetworkException &e) {
        LOG(10) << e.what();
        errors[hop] = e.error();
      }
    }
    if (hops == 0) {
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
  PERF(PerfMessages::MESSAGE_REMOVED) << bundleContainer->getBundle().getId();
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
