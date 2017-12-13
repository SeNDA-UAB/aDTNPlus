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
 * FILE BundleProcessor.h
 * AUTHOR Blackcatn13
 * DATE Dec 17, 2015
 * VERSION 1
 *
 */
#ifndef BUNDLEAGENT_NODE_BUNDLEPROCESSOR_BUNDLEPROCESSOR_H_
#define BUNDLEAGENT_NODE_BUNDLEPROCESSOR_BUNDLEPROCESSOR_H_

#include <cstdint>
#include <memory>
#include <vector>
#include <string>
#include <exception>
#include <map>
#include "Node/Config.h"

class Bundle;
class BundleQueue;
class BundleContainer;
class NeighbourTable;
class ListeningEndpointsTable;
class Neighbour;

/**
 * Esception with a list of what error occurred at each neighbour.
 */
class ForwardException : public std::runtime_error {
 public:
  explicit ForwardException(const std::string &what, std::map<std::string, uint8_t> errorList)
      : runtime_error(what),
        m_errorList(errorList) {
  }

  std::map<std::string, uint8_t> errors() const {
    return m_errorList;
  }

 private:
  std::map<std::string, uint8_t> m_errorList;
};

/**
 * Exception that saves the error value
 */
class ForwardNetworkException : public std::runtime_error {
 public:
  explicit ForwardNetworkException(const std::string &what, uint8_t error)
      : runtime_error(what),
        m_error(error) {
  }

  uint8_t error() const {
    return m_error;
  }

 private:
  uint8_t m_error;
};

/**
 * Enum for ACK possible values
 */
enum class BundleACK : uint8_t {
  CORRECT_RECEIVED = 0x00,
  ALREADY_IN_QUEUE = 0x01,
  QUEUE_FULL = 0x02
};

/**
 * Error codes for network.
 */
enum class NetworkError : uint8_t {
  SOCKET_ERROR = 0x00,
  SOCKET_TIMEOUT_ERROR = 0x01,
  SOCKET_CONNECT_ERROR = 0x02,
  SOCKET_WRITE_ERROR = 0x03,
  SOCKET_RECEIVE_ERROR = 0x04,
  NEIGHBOUR_FULL_QUEUE = 0x05,
  NEIGHBOUR_IN_QUEUE = 0x06,
  NEIGHBOUR_BAD_ACK = 0x07
};

/**
 * CLASS BundleProcessor
 * This class implements the receiving and process bundle methods.
 * It also defines an interface to process a single bundle and to create the
 * bundle container.
 */
class BundleProcessor {
 public:
  /**
   * @brief Generates a BundleProcessor.
   */
  BundleProcessor();
  /**
   * Destructor of the class.
   */
  virtual ~BundleProcessor();
  /**
   * @brief Function that starts all the process.
   *
   * This function will start the process Bundles and receive Bundles process.
   *
   * The parameters are the configuration, the bundle queue, the neighbour table
   * and the listening apps table.
   *
   * @param config The object holding all the configuration.
   * @param bundleQueue The queue that will hold all the bundles.
   * @param neighbourTable The neighbour table to check the neighbours.
   */
  virtual void start(
      Config config, std::shared_ptr<BundleQueue> bundleQueue,
      std::shared_ptr<NeighbourTable> neighbourTable,
      std::shared_ptr<ListeningEndpointsTable> listeningAppsTable);
  /**
   * @brief Function that restores a bundle container from disk.
   *
   * This function generates the bundle container from a serialized data.
   *
   * @param data The serialized BundleContainer.
   */
  virtual void restoreRawBundleContainer(const std::string &data);

 protected:
  /**
   * @brief Function that dispatches a bundle.
   *
   * This function will dispatch a bundle to the given destinations.
   *
   * @param bundle Bundle to delivery.
   * @param destinations List of all the destinations to delivery the bundle.
   */
  void delivery(BundleContainer &bundleContainer,
                std::vector<std::string> destinations);
  /**
   * @brief Function that forwards a bundle.
   *
   * This function will forward a bundle to the given destinations.
   *
   * @param bundle Bundle to forward.
   * @param nextHop List of all the destinations to forward the bundle.
   */
  void forward(Bundle bundle, std::vector<std::string> nextHop);
  /**
   * @brief Function that discards a bundle container.
   *
   * This function discards an eliminates a bundle from the node.
   *
   * @param bundleContainer The bundle container to discard.
   */
  virtual void discard(std::unique_ptr<BundleContainer> bundleContainer);
  /**
   * @brief Function that restores a bundle container.
   *
   * This function saves again the bundle into the queue.
   *
   * @param bundleContainer The bundle container to restore.
   */
  void restore(std::unique_ptr<BundleContainer> bundleContainer);
  /**
   * @bief Function that is called when a drop occur.
   *
   * This function by default does nothing.
   */
  virtual void drop();
  /**
   * Variable that holds the configuration.
   */
  Config m_config;
  /**
   * Variable that holds the bundle queue.
   */
  std::shared_ptr<BundleQueue> m_bundleQueue;
  /**
   * Variable that holds the neighbour table.
   */
  std::shared_ptr<NeighbourTable> m_neighbourTable;
  /**
   * Variable that holds the listening apps table.
   */
  std::shared_ptr<ListeningEndpointsTable> m_listeningAppsTable;

 private:
  /**
   * Function that processes the bundles.
   */
  void processBundles();
  /**
   * Function that receives the bundles.
   */
  void receiveBundles();
  /**
   * Function that parses, creates and saves a receiving bundle.
   *
   * @param sock Socket to read the message.
   */
  void receiveMessage(int sock);
  /**
   * Function that processes one given bundle container.
   * Virtual function, all the bundleProcessors must implement it.
   *
   * @param bundleContainer The bundle container to process.
   */
  virtual void processBundle(
      std::unique_ptr<BundleContainer> bundleContainer) = 0;
  /**
   * Function that creates a bundle container.
   * Virtual function, all the bundleProcessors must implement it.
   *
   * @param from The neighbour who has sent the bundle.
   * @param Bundle The bundle received.
   */
  virtual std::unique_ptr<BundleContainer> createBundleContainer(
      std::unique_ptr<Bundle> Bundle) = 0;
};

#endif  // BUNDLEAGENT_NODE_BUNDLEPROCESSOR_BUNDLEPROCESSOR_H_
