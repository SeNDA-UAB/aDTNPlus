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
 * FILE BasicBundleProcessor.h
 * AUTHOR Blackcatn13
 * DATE Dec 17, 2015
 * VERSION 1
 *
 */
#ifndef BUNDLEAGENT_NODE_BUNDLEPROCESSOR_BASICBUNDLEPROCESSOR_H_
#define BUNDLEAGENT_NODE_BUNDLEPROCESSOR_BASICBUNDLEPROCESSOR_H_

#include <memory>
#include <vector>
#include <string>
#include "Node/BundleProcessor/BundleProcessor.h"

class Bundle;
class BundleQueue;
class BundleContainer;
class NeighbourTable;
class ListeningAppsTable;
class Neighbour;

/**
 * CLASS BasicBundleProcessor
 * This class is an specification of the BundleProcessor.
 */
class BasicBundleProcessor : public BundleProcessor {
 public:
  /**
   * @brief Generates a BasicBundleProcessor with the given parameters
   *
   * The parameters are the configuration, the bundle queue, the neighbour table
   * and the listening apps table.
   *
   * @param config The object holding all the configuration.
   * @param bundleQueue The queue that will hold all the bundles.
   * @param neighbourTable The neighbour table to check the neighbours.
   */
  BasicBundleProcessor(Config config, std::shared_ptr<BundleQueue> bundleQueue,
                       std::shared_ptr<NeighbourTable> neighbourTable,
                       std::shared_ptr<ListeningAppsTable> listeningAppsTable);
  /**
   * Destructor of the class.
   */
  virtual ~BasicBundleProcessor();

 private:
  /**
   * Function that processes one given bundle container.
   *
   * @param bundleContainer The bundle container to process.
   */
  void processBundle(std::unique_ptr<BundleContainer> bundleContainer);
  /**
   * Function that creates a bundle container.
   *
   * @param from The neighbour who has sent the bundle.
   * @param Bundle The bundle received.
   */
  std::unique_ptr<BundleContainer> createBundleContainer(
      std::shared_ptr<Neighbour> from, std::unique_ptr<Bundle> Bundle);
  /**
   * Function that checks the possible destinations.
   *
   * @return A list with the possible destinations.
   */
  std::vector<std::string> checkDestination(BundleContainer &bundleContainer);
  /**
   * Function that checks the lifetime of a bundle.
   *
   * @return True if the lifetime has expired, false otherwise.
   */
  bool checkLifetime(BundleContainer &bundleContainer);
  /**
   * Function that checks the possible forwards.
   *
   * @return A list with the possible forwards.
   */
  std::vector<std::string> checkForward(const BundleContainer &bundleContainer);
};

#endif  // BUNDLEAGENT_NODE_BUNDLEPROCESSOR_BASICBUNDLEPROCESSOR_H_
