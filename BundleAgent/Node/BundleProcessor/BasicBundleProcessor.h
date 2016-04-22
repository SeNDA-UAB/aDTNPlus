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

#include "Utils/Json.h"
#include "Node/JsonFacades/NodeStateJson.h"
#include "Node/BundleProcessor/BundleProcessor.h"
#include "Node/Executor/Worker.h"

class Bundle;
class BundleQueue;
class BundleContainer;
class NeighbourTable;
class ListeningEndpointsTable;
class Neighbour;
class BundleInfo;

/**
 * CLASS BasicBundleProcessor
 * This class is an specification of the BundleProcessor.
 */
class BasicBundleProcessor : public BundleProcessor {
 public:
  /**
   * @brief Generates a BasicBundleProcessor.
   */
  BasicBundleProcessor();
  /**
   * Destructor of the class.
   */
  virtual ~BasicBundleProcessor();
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
  virtual void start(Config config, std::shared_ptr<BundleQueue> bundleQueue,
                     std::shared_ptr<NeighbourTable> neighbourTable,
                     std::shared_ptr<ListeningEndpointsTable> listeningAppsTable);

 protected:
  /**
   * Function that processes one given bundle container.
   *
   * @param bundleContainer The bundle container to process.
   */
  virtual void processBundle(std::unique_ptr<BundleContainer> bundleContainer);
  /**
   * Function that creates a bundle container.
   *
   * @param Bundle The bundle received.
   */
  virtual std::unique_ptr<BundleContainer> createBundleContainer(
      std::unique_ptr<Bundle> Bundle);
  /**
   * Function that checks the possible dispatching apps.
   *
   * @param bundleContainer The container with the bundle.
   * @return A list with the possible dispatching apps.
   */
  std::vector<std::string> checkDispatch(BundleContainer &bundleContainer);
  /**
   * Function that checks the lifetime of a bundle.
   *
   * @param bundleContainer The container with the bundle.
   * @return True if the lifetime has expired, false otherwise.
   */
  virtual bool checkLifetime(BundleContainer &bundleContainer);
  /**
   * Function that checks the possible forwards.
   *
   * @param bundleContainer The container with the bundle.
   * @return A list with the possible forwards.
   */
  virtual std::vector<std::string> checkForward(
      BundleContainer &bundleContainer);
  /**
   * Function that checks if we are the destination node.
   *
   * @param bundleContainer The container with the bundle.
   * @return True if the bundle is for us.
   */
  virtual bool checkDestination(BundleContainer &bundleContainer);
  /**
   * Function that checks the changes in the node state.
   *
   * This function implements which changes in the node state should be checked
   * and what actions the node should do.
   */
  virtual void checkNodeStateChanges();
  /**
   * Worker to execute default forwarding code.
   */
  Worker<std::vector<std::string>, Json> m_forwardWorker;
  /**
   * Worker to execute the default lifetime code.
   */
  Worker<bool, Json, BundleInfo> m_lifeWorker;
  /**
   * Worker to execute the default destination code.
   */
  Worker<bool, Json, BundleInfo> m_destinationWorker;
  /**
   * Variable that holds the parameters used in the processor calls.
   */
  NodeStateJson m_nodeState;
  /**
   * Variable that holds the old parameters to check what changed.
   */
  NodeStateJson m_oldNodeState;

  static const std::string m_forwardHeader;
  static const std::string m_lifeHeader;
  static const std::string m_destinationHeader;
  static const std::string m_footer;
  static const std::string m_commandLine;
};

#endif  // BUNDLEAGENT_NODE_BUNDLEPROCESSOR_BASICBUNDLEPROCESSOR_H_
