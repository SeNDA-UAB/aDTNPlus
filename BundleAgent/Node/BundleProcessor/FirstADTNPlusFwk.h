/*
 * Copyright (c) 2016 SeNDA
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
 * FILE Framework1Processor.h
 * AUTHOR Blackcatn13
 * DATE Apr 4, 2016
 * VERSION 1
 *
 */
#ifndef BUNDLEAGENT_NODE_BUNDLEPROCESSOR_FIRSTADTNPLUSFWK_H_
#define BUNDLEAGENT_NODE_BUNDLEPROCESSOR_FIRSTADTNPLUSFWK_H_

#include <memory>
#include <vector>
#include <string>
#include "Utils/Json.h"
#include "Node/BundleProcessor/BundleProcessor.h"
#include "Node/Executor/Worker.h"
#include "ExternTools/json/json.hpp"

class Bundle;
class BundleQueue;
class BundleContainer;
class NeighbourTable;
class ListeningAppsTable;
class BundleInfo;

/**
 * CLASS FirstADTNPlusFwk
 * This class is an specification of the BundleProcessor, who defines a framework
 * and a number of points where code can be executed.
 */
class FirstADTNPlusFwk : public BundleProcessor {
 public:
  /**
   * @brief Generates a FirstADTNPlusFwk.
   */
  FirstADTNPlusFwk();
  /**
   * Destructor of the class.
   */
  virtual ~FirstADTNPlusFwk();
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
                     std::shared_ptr<ListeningAppsTable> listeningAppsTable);

 private:
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
   * @brief Function that discards a bundle container.
   *
   * This function discards an eliminates a bundle from the node.
   *
   * @param bundleContainer The bundle container to discard.
   */
  virtual void discard(std::unique_ptr<BundleContainer> bundleContainer);
  /**
   * Worker to execute the void functions.
   * Like a void worker cannot be created, a bool one with an always return
   * true is used.
   */
  Worker<bool, Json, nlohmann::json, nlohmann::json, BundleInfo,
      Worker<bool, Json, nlohmann::json, BundleInfo>> m_voidWorker;
  /**
   * Worker to execute the bool functions.
   */
  Worker<bool, Json, nlohmann::json, nlohmann::json, BundleInfo,
      Worker<bool, Json, nlohmann::json, BundleInfo>> m_boolWorker;
  /**
   * Worker to execute the functions that return a vector of strings.
   */
  Worker<std::vector<std::string>, Json, nlohmann::json, nlohmann::json,
      BundleInfo,
      Worker<std::vector<std::string>, Json, nlohmann::json, BundleInfo>> m_vectorWorker;
  /**
   * Worker for the default function in the extension 1 (BundleContainer creation)
   */
  Worker<bool, Json, nlohmann::json, BundleInfo> m_ext1DefaultWorker;
  /**
   * Worker for the default function in the extension 2 (BundleContainer deletion)
   */
  Worker<bool, Json, nlohmann::json, BundleInfo> m_ext2DefaultWorker;
  /**
   * Worker for the default function in the extension 3 (Check Destination)
   */
  Worker<bool, Json, nlohmann::json, BundleInfo> m_ext3DefaultWorker;
  /**
   * Worker for the default function in the extension 4 (Check Lifetime)
   */
  Worker<bool, Json, nlohmann::json, BundleInfo> m_ext4DefaultWorker;
  /**
   * Worker for the default function in the extension 5 (Check Forward)
   */
  Worker<std::vector<std::string>, Json, nlohmann::json, BundleInfo> m_ext5DefaultWorker;
  /**
   * Variable that holds the parameters used in the processor calls.
   */
  Json m_nodeState;
  /**
   * Variable that holds the old parameters to check what changed.
   */
  Json m_oldNodeState;
  /**
   * Variable that holds the common header for all the workers.
   */
  static const std::string m_header;
  /**
   * Variable that holds the function signature of the workers that have a worker
   * as parameter.
   */
  static const std::string m_bigSignature;
  /**
   * Variable that holds the function signature of the default workers.
   */
  static const std::string m_littleSignature;
  /**
   * Variable that holds the footer for the worker.
   */
  static const std::string m_footer;
  /**
   * Variable that holds the command line for the workers.
   */
  static const std::string m_commandLine;
};

#endif  // BUNDLEAGENT_NODE_BUNDLEPROCESSOR_FIRSTADTNPLUSFWK_H_
