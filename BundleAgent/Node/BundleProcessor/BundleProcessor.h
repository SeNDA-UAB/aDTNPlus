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

#include <memory>
#include <vector>
#include <string>
#include "Node/Config.h"

class Bundle;
class BundleQueue;
class BundleContainer;
class NeighbourTable;
// class ListeningAppsTable;
class Neighbour;

/**
 * CLASS BundleProcessor
 * This class implements the receiving and process bundle methods.
 * It also defines an interface to process a single bundle and to create the
 * bundle container.
 */
class BundleProcessor {
 public:
  /**
   * @brief Generates a BundleProcessor with the given parameters
   *
   * The parameters are the configuration, the bundle queue, the neighbour table
   * and the listening apps table.
   *
   * @param config The object holding all the configuration.
   * @param bundleQueue The queue that will hold all the bundles.
   * @param neighbourTable The neighbour table to check the neighbours.
   */
  BundleProcessor(Config config, std::shared_ptr<BundleQueue> bundleQueue,
                  std::shared_ptr<NeighbourTable> neighbourTable
                  /*, std::shared_ptr<ListeningAppsTable> listeningAppsTable*/);
  /**
   * Destructor of the class.
   */
  virtual ~BundleProcessor();

 protected:
  /**
   * @brief Function that dispatches a bundle.
   *
   * This function will dispatch a bundle to the given destinations.
   *
   * @param bundle Bundle to dispatch.
   * @param destinations List of all the destinations to dispatch the bundle.
   */
  void dispatch(const Bundle& bundle,
                std::vector<std::string> destinations);
  /**
   * @brief Function that forwards a bundle.
   *
   * This function will forward a bundle to the given destinations.
   *
   * @param bundle Bundle to forward.
   * @param nextHop List of all the destinations to forward the bundle.
   */
  void forward(Bundle bundle,
               std::vector<std::string> nextHop);
  /**
   * @brief Function that discards a bundle container.
   *
   * This function discards an eliminates a bundle from the node.
   *
   * @param bundleContainer The bundle container to discard.
   */
  void discard(std::unique_ptr<BundleContainer> bundleContainer);
  /**
   * @brief Function that restores a bundle container.
   *
   * This function saves again the bundle into the queue.
   *
   * @param bundleContainer The bundle container to restore.
   */
  void restore(std::unique_ptr<BundleContainer> bundleContainer);
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
  // std::shared_ptr<ListeningAppsTable> m_listeningAppsTable;

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
  virtual void createBundleContainer(std::shared_ptr<Neighbour> from,
                                     std::unique_ptr<Bundle> Bundle) = 0;
};

#endif  // BUNDLEAGENT_NODE_BUNDLEPROCESSOR_BUNDLEPROCESSOR_H_
