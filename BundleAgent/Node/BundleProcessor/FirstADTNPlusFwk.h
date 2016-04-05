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

class FirstADTNPlusFwk : public BundleProcessor {
 public:
  FirstADTNPlusFwk();
  virtual ~FirstADTNPlusFwk();

  virtual void start(Config config, std::shared_ptr<BundleQueue> bundleQueue,
                     std::shared_ptr<NeighbourTable> neighbourTable,
                     std::shared_ptr<ListeningAppsTable> listeningAppsTable);

 private:
  virtual void processBundle(std::unique_ptr<BundleContainer> bundleContainer);

  virtual std::unique_ptr<BundleContainer> createBundleContainer(
      std::unique_ptr<Bundle> Bundle);

  std::vector<std::string> checkDispatch(BundleContainer &bundleContainer);

  virtual bool checkLifetime(BundleContainer &bundleContainer);

  virtual std::vector<std::string> checkForward(
      BundleContainer &bundleContainer);

  virtual bool checkDestination(BundleContainer &bundleContainer);

  virtual void checkNodeStateChanges();

  Worker<bool, Json, nlohmann::json, nlohmann::json, BundleInfo,
      Worker<bool, Json, nlohmann::json, BundleInfo>> m_voidWorker;

  Worker<bool, Json, nlohmann::json, nlohmann::json, BundleInfo,
      Worker<bool, Json, nlohmann::json, BundleInfo>> m_boolWorker;

  Worker<std::vector<std::string>, Json, nlohmann::json, nlohmann::json,
      BundleInfo,
      Worker<std::vector<std::string>, Json, nlohmann::json, BundleInfo>> m_vectorWorker;

  Worker<bool, Json, nlohmann::json, BundleInfo> m_ext1DefaultWorker;

  Worker<bool, Json, nlohmann::json, BundleInfo> m_ext2DefaultWorker;

  Worker<bool, Json, nlohmann::json, BundleInfo> m_ext3DefaultWorker;

  Worker<bool, Json, nlohmann::json, BundleInfo> m_ext4DefaultWorker;

  Worker<std::vector<std::string>, Json, nlohmann::json, BundleInfo> m_ext5DefaultWorker;
  /**
   * Variable that holds the parameters used in the processor calls.
   */
  Json m_nodeState;
  /**
   * Variable that holds the old parameters to check what changed.
   */
  Json m_oldNodeState;

  static const std::string m_header;
  static const std::string m_bigSignature;
  static const std::string m_littleSignature;
  static const std::string m_footer;
  static const std::string m_voidFooter;
  static const std::string m_commandLine;
};

#endif  // BUNDLEAGENT_NODE_BUNDLEPROCESSOR_FIRSTADTNPLUSFWK_H_
