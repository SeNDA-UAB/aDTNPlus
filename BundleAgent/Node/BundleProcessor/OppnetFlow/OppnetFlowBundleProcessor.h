/*
 * Copyright (c) 2017 SeNDA
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
 * FILE OppnetFlowBundleProcessor.h
 * AUTHOR MCarmen
 * DATE Nov 20, 2017
 * VERSION 1
 *
 */
#ifndef BUNDLEAGENT_NODE_BUNDLEPROCESSOR_OPPNETFLOWBUNDLEPROCESSOR_H_
#define BUNDLEAGENT_NODE_BUNDLEPROCESSOR_OPPNETFLOWBUNDLEPROCESSOR_H_

#include "Bundle/BundleTypes.h"
#include "Node/BundleProcessor/BundleProcessor.h"
#include "Node/BundleProcessor/OppnetFlow/ForwardingAlgorithmFactory.h"
#include "Node/JsonFacades/NodeStateJson.h"
#include <memory>
#include <string>
#include <vector>

class MetadataExtensionBlock;

class BundleContainer;

class OppnetFlowBundleProcessor : public BundleProcessor {

 public:
  /**
   * @brief Constructor
   */
  OppnetFlowBundleProcessor();
  /**
   * Destructor of the class.
   */
  virtual ~OppnetFlowBundleProcessor();
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
  void start(Config config, std::shared_ptr<BundleQueue> bundleQueue,
             std::shared_ptr<NeighbourTable> neighbourTable,
             std::shared_ptr<ListeningEndpointsTable> listeningAppsTable);

  /**
   * Function that returns an extension block from the bundle
   *
   * @param extensionType the type of the extension block to be fetched.
   * @param The bundle to process.
   * @return the metadata extension bloc or nullptr if the extension bloc
   * has not been found.
   */
  static std::shared_ptr<MetadataExtensionBlock>
      findMetadataExtensionBlock(const MetadataTypes extensionType,
                                 Bundle& bundle);

 protected:
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
  bool checkLifetime(BundleContainer &bundleContainer);
  /**
   * Function that checks if we are the destination node.
   *
   * @param bundleContainer The container with the bundle.
   * @return True if the bundle is for us.
   */
  bool checkDestination(BundleContainer &bundleContainer);

  /**
   * Function that processes one given bundle container.
   * This processing ends up forwarding the bundle either to the
   * application layer or to the other neighbours deppending on the routing
   * algorithm specified in the nodestate.json file.
   * Virtual function, all the bundleProcessors must implement it.
   *
   * @param bundleContainer The bundle container to process.
   */
  void processBundle(std::unique_ptr<BundleContainer> bundleContainer);
  /**
   * Function that creates a bundle container.
   * Virtual function, all the bundleProcessors must implement it.
   *
   * @param from The neighbour who has sent the bundle.
   * @param Bundle The bundle received.
   */
  std::unique_ptr<BundleContainer> createBundleContainer(
      std::unique_ptr<Bundle> bundle);

  /**
   * Variable that holds the parameters used in the processor calls.
   */
  NodeStateJson m_nodeState;

  /**
   * Factory to get from a forwardingAlgorithm to forward the bundle.
   */
  ForwardingAlgorithmFactory m_forwardingAlgorithmFactory;


};

#endif  // BUNDLEAGENT_NODE_BUNDLEPROCESSOR_OPPNETFLOWBUNDLEPROCESSOR_H_

