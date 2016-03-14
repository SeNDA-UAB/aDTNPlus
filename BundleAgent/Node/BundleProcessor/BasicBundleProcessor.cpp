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
 * FILE BasicBundleProcessor.cpp
 * AUTHOR Blackcatn13
 * DATE Dec 17, 2015
 * VERSION 1
 *
 */

#include "Node/BundleProcessor/BasicBundleProcessor.h"
#include <memory>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include "Node/BundleProcessor/BundleProcessor.h"
#include "Node/BundleQueue/BundleQueue.h"
#include "Node/Neighbour/NeighbourTable.h"
#include "Node/Neighbour/Neighbour.h"
#include "Node/Config.h"
#include "Node/BundleQueue/BundleContainer.h"
#include "Bundle/Bundle.h"
#include "Bundle/PrimaryBlock.h"
#include "Utils/Logger.h"
#include "Utils/TimestampManager.h"
#include "Node/AppListener/ListeningAppsTable.h"
#include "Node/BundleProcessor/RoutingSelectionBundleProcessor.h"
#include "Node/BundleProcessor/PluginAPI.h"
#include "Utils/globals.h"

#ifdef BASE_PLUGIN
NEW_PLUGIN(BasicBundleProcessor,
    "Basic bundle processor", "1.0",
    "Forwards a bundle this processor only checks for anti-rebooting.")
#endif

const std::string BasicBundleProcessor::m_header = "#include <vector>\n"
    "#include <string>\n"
    "#include <algorithm>\n"
    "#include \"adtnPlus/Json.h\"\n"
    "extern \"C\" {"
    " std::vector<std::string> f(Json nodeState) {";
const std::string BasicBundleProcessor::m_footer = "}}";
const std::string BasicBundleProcessor::m_commandLine =
    "g++ -w -fPIC -shared -std=c++11 %s -o %s 2>&1";

BasicBundleProcessor::BasicBundleProcessor()
    : m_worker(m_header, m_footer, "f", m_commandLine) {
}

BasicBundleProcessor::~BasicBundleProcessor() {
  std::ofstream nodeState(m_config.getNodeStatePath());
  nodeState << m_nodeState.dump(2);
  nodeState.close();
}

void BasicBundleProcessor::start(
    Config config, std::shared_ptr<BundleQueue> bundleQueue,
    std::shared_ptr<NeighbourTable> neighbourTable,
    std::shared_ptr<ListeningAppsTable> listeningAppsTable) {
  BundleProcessor::start(config, bundleQueue, neighbourTable,
                         listeningAppsTable);
  std::ifstream nodeState(m_config.getNodeStatePath());
  m_nodeState.start(m_neighbourTable);
  if (nodeState) {
    nodeState >> m_nodeState;
    m_oldNodeState = m_nodeState;
    nodeState.close();
    std::string defaultCode =
        m_nodeState["configuration"]["defaultCodes"]["forwarding"];
    try {
      m_worker.generateFunction(defaultCode);
    } catch (const WorkerException &e) {
      LOG(11) << "Cannot create code worker, reason: " << e.what();
    }
  } else {
    LOG(11) << "Cannot open the file " << m_config.getNodeStatePath();
  }
}

void BasicBundleProcessor::processBundle(
    std::unique_ptr<BundleContainer> bundleContainer) {
  LOG(51) << "Processing a bundle container.";
  LOG(55) << "Checking destination node.";
  if (checkDestination(*bundleContainer)) {
    LOG(55) << "We are the destination node.";
    LOG(55) << "Checking destination app listening.";
    std::vector<std::string> destinations = checkDispatch(*bundleContainer);
    if (destinations.size() > 0) {
      LOG(55) << "There is a listening app, dispatching the bundle.";
      try {
        dispatch(bundleContainer->getBundle(), destinations);
        discard(std::move(bundleContainer));
      } catch (const TableException &e) {
        LOG(55) << "Restoring not dispatched bundle.";
        restore(std::move(bundleContainer));
      }
    } else {
      LOG(55) << "No listening app, restoring the bundle.";
      restore(std::move(bundleContainer));
    }
  } else {
    LOG(55) << "We are not the destination node.";
    LOG(55) << "Checking lifetime.";
    if (checkLifetime(*bundleContainer)) {
      LOG(55) << "Bundle expired, discarding it.";
      discard(std::move(bundleContainer));
    } else {
      LOG(55) << "Bundle is not expired yet.";
      LOG(55) << "Checking neighbours. " << "";
      std::vector<std::string> neighbours = checkForward(*bundleContainer);
      if (neighbours.size() > 0) {
        LOG(55) << "There are some neighbours.";
        LOG(55) << "Checking if one of them is the bundle destination.";
        auto it = std::find(
            neighbours.begin(),
            neighbours.end(),
            bundleContainer->getBundle().getPrimaryBlock()->getDestination()
                .substr(
                0,
                bundleContainer->getBundle().getPrimaryBlock()->getDestination()
                    .find(":")));
        if (it != neighbours.end()) {
          LOG(55) << "Destination found, sending the bundle to it.";
          std::vector<std::string> nextHop = std::vector<std::string>();
          nextHop.push_back(*it);
          try {
            forward(bundleContainer->getBundle(), nextHop);
            LOG(55) << "Discarding the bundle.";
            discard(std::move(bundleContainer));
          } catch (const ForwardException &e) {
            LOG(1) << e.what();
            LOG(55) << "The bundle has not been send, restoring the bundle.";
            restore(std::move(bundleContainer));
          }
        } else {
          LOG(55) << "Destination not found, "
                  << "sending the bundle to all the neighbours.";
          try {
            forward(bundleContainer->getBundle(), neighbours);
            LOG(55) << "Discarding the bundle.";
            discard(std::move(bundleContainer));
          } catch (const ForwardException &e) {
            LOG(1) << e.what();
            LOG(55) << "The bundle has not been send, restoring the bundle.";
            restore(std::move(bundleContainer));
          }
        }
      } else {
        LOG(55) << "No neighbours found, restoring the bundle.";
        restore(std::move(bundleContainer));
      }
    }
  }
  checkNodeStateChanges();
}

std::unique_ptr<BundleContainer> BasicBundleProcessor::createBundleContainer(
    std::shared_ptr<Neighbour> from, std::unique_ptr<Bundle> bundle) {
  return std::unique_ptr<BundleContainer>(
      new BundleContainer(from->getId(), std::move(bundle)));
}

bool BasicBundleProcessor::checkDestination(BundleContainer &bundleContainer) {
  std::string destination = bundleContainer.getBundle().getPrimaryBlock()
      ->getDestination();
  std::string destinationId = destination.substr(0, destination.find(":"));
  return destinationId == m_config.getNodeId();
}

std::vector<std::string> BasicBundleProcessor::checkDispatch(
    BundleContainer &bundleContainer) {
  std::string destination = bundleContainer.getBundle().getPrimaryBlock()
      ->getDestination();
  std::string appId = destination.substr(destination.find(":") + 1);
  std::vector<std::string> dispatch;
  dispatch.push_back(appId);
  return dispatch;
}

std::vector<std::string> BasicBundleProcessor::checkForward(
    BundleContainer &bundleContainer) {
  std::vector<std::string> neighbours = m_neighbourTable->getValues();
  try {
    m_nodeState.setLastFrom(bundleContainer.getFrom());
    m_worker.execute(m_nodeState);
    return m_worker.getResult();
  } catch (const WorkerException &e) {
    LOG(11) << "Cannot execute code, reason: " << e.what()
            << " Executing anti-rebooting.";
    LOG(55) << "Removing bundle source if we have it as neighbour.";
    auto it = std::find(neighbours.begin(), neighbours.end(),
                        bundleContainer.getFrom());
    if (it != neighbours.end()) {
      neighbours.erase(it);
    }
    return neighbours;
  }
}

bool BasicBundleProcessor::checkLifetime(BundleContainer &bundleContainer) {
  uint64_t creationTimestamp = bundleContainer.getBundle().getPrimaryBlock()
      ->getCreationTimestamp();
  if (bundleContainer.getBundle().getPrimaryBlock()->getLifetime()
      < (time(NULL) - g_timeFrom2000 - creationTimestamp))
    return true;
  else
    return false;
}

void BasicBundleProcessor::checkNodeStateChanges() {
  if (m_nodeState["state"]["changed"]) {
    // Check what changed and act accordingly
    if (m_nodeState["state"]["stop"]) {
      g_stop = true;
    }
    if (m_nodeState["configuration"]["logLevel"]
        != m_oldNodeState["configuration"]["logLevel"]) {
      Logger::getInstance()->setLogLevel(
          m_nodeState["configuration"]["logLevel"]);
    }
    std::string code =
        m_nodeState["configuration"]["defaultCodes"]["forwarding"];
    if (code.compare(
        m_oldNodeState["configuration"]["defaultCodes"]["forwarding"]) != 0) {
      try {
        m_worker.generateFunction(
            m_nodeState["configuration"]["defaultCodes"]["forwarding"]);
      } catch (const WorkerException &e) {
        LOG(11) << "Cannot create code worker, reason: " << e.what();
      }
    }
    m_nodeState["state"]["changed"] = false;
    m_oldNodeState = m_nodeState;
  }
}
