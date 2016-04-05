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
#include "Bundle/BundleInfo.h"
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

const std::string BasicBundleProcessor::m_forwardHeader = "#include <vector>\n"
    "#include <string>\n"
    "#include <algorithm>\n"
    "#include <cstdlib>\n"
    "#include \"adtnPlus/Json.h\"\n"
    "extern \"C\" {"
    " std::vector<std::string> f(Json ns) {";
const std::string BasicBundleProcessor::m_lifeHeader = "#include <cstdint>\n"
    "#include \"Bundle/BundleInfo.h\"\n"
    "#include \"adtnPlus/Json.h\"\n"
    "extern \"C\" {"
    "const uint64_t g_timeFrom2000 = 946684800;"
    "bool f(Json ns, BundleInfo bi) {";
const std::string BasicBundleProcessor::m_destinationHeader =
    "#include <string>\n"
        "#include \"adtnPlus/Json.h\"\n"
        "#include \"Bundle/BundleInfo.h\"\n"
        "extern \"C\" {"
        "bool f(Json ns, BundleInfo bi) {";
const std::string BasicBundleProcessor::m_footer = "}}";
const std::string BasicBundleProcessor::m_commandLine =
    "g++ -w -fPIC -shared -std=c++11 %s -o %s 2>&1";

BasicBundleProcessor::BasicBundleProcessor()
    : m_forwardWorker(m_forwardHeader, m_footer, "f", m_commandLine, "./"),
      m_lifeWorker(m_lifeHeader, m_footer, "f", m_commandLine, "./"),
      m_destinationWorker(m_destinationHeader, m_footer, "f", m_commandLine,
                          "./") {
}

BasicBundleProcessor::~BasicBundleProcessor() {
  if (!m_nodeState.empty()) {
    std::ofstream nodeState(m_config.getNodeStatePath());
    nodeState << m_nodeState.dump(2);
    nodeState.close();
  }
}

void BasicBundleProcessor::start(
    Config config, std::shared_ptr<BundleQueue> bundleQueue,
    std::shared_ptr<NeighbourTable> neighbourTable,
    std::shared_ptr<ListeningAppsTable> listeningAppsTable) {
  BundleProcessor::start(config, bundleQueue, neighbourTable,
                         listeningAppsTable);
  std::ifstream nodeState(m_config.getNodeStatePath());
  m_nodeState.start(m_neighbourTable);
  m_forwardWorker.setPath(m_config.getCodesPath());
  m_lifeWorker.setPath(m_config.getCodesPath());
  m_destinationWorker.setPath(m_config.getCodesPath());
  if (nodeState) {
    try {
      nodeState >> m_nodeState;
      m_oldNodeState = m_nodeState;
      nodeState.close();
      std::string defaultForwardingCode =
          m_nodeState["configuration"]["defaultCodes"]["forwarding"];
      std::string defaultLifeCode =
          m_nodeState["configuration"]["defaultCodes"]["lifetime"];
      std::string defaultDestinationCode =
          m_nodeState["configuration"]["defaultCodes"]["destination"];
      try {
        m_forwardWorker.generateFunction(defaultForwardingCode);
        m_lifeWorker.generateFunction(defaultLifeCode);
        m_destinationWorker.generateFunction(defaultDestinationCode);
      } catch (const WorkerException &e) {
        LOG(11) << "Cannot create code worker, reason: " << e.what();
      }
    } catch (const std::invalid_argument &e) {
      LOG(1) << "Error in NodeState json: " << e.what();
      g_stop = true;
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
    std::unique_ptr<Bundle> bundle) {
  return std::unique_ptr<BundleContainer>(
      new BundleContainer(std::move(bundle)));
}

bool BasicBundleProcessor::checkDestination(BundleContainer &bundleContainer) {
  try {
    BundleInfo bi = BundleInfo(bundleContainer.getBundle());
    m_destinationWorker.execute(m_nodeState, bi);
    return m_destinationWorker.getResult();
  } catch (const WorkerException &e) {
    LOG(11) << "Cannot execute code, reason: " << e.what();
  }
  return false;
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
    m_forwardWorker.execute(m_nodeState);
    return m_forwardWorker.getResult();
  } catch (const WorkerException &e) {
    LOG(11) << "Cannot execute code, reason: " << e.what()
            << " Executing flooding.";
    return neighbours;
  }
}

bool BasicBundleProcessor::checkLifetime(BundleContainer &bundleContainer) {
  try {
    BundleInfo bi = BundleInfo(bundleContainer.getBundle());
    m_lifeWorker.execute(m_nodeState, bi);
    return m_lifeWorker.getResult();
  } catch (const WorkerException &e) {
    LOG(11) << "Cannot create code worker, reason: " << e.what();
  }
  return false;
}

void BasicBundleProcessor::checkNodeStateChanges() {
  if (m_nodeState["state"]["changed"]) {
    m_nodeState["state"]["changed"] = false;
    // Check what changed and act accordingly
    if (m_nodeState["state"]["stop"]) {
      m_nodeState["state"]["stop"] = false;
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
        m_forwardWorker.generateFunction(code);
      } catch (const WorkerException &e) {
        LOG(11) << "Cannot create forward code worker, reason: " << e.what();
      }
    }
    code = m_nodeState["configuration"]["defaultCodes"]["lifetime"];
    if (code.compare(
        m_oldNodeState["configuration"]["defaultCodes"]["lifetime"]) != 0) {
      try {
        m_lifeWorker.generateFunction(code);
      } catch (const WorkerException &e) {
        LOG(11) << "Cannot create life code worker, reason: " << e.what();
      }
    }
    m_oldNodeState = m_nodeState;
  }
}
