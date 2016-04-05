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
 * FILE Framework1Processor.cpp
 * AUTHOR Blackcatn13
 * DATE Apr 4, 2016
 * VERSION 1
 *
 */

#include "Node/BundleProcessor/FirstADTNPlusFwk.h"
#include <memory>
#include <vector>
#include <string>
#include <fstream>
#include "Node/BundleProcessor/BundleProcessor.h"
#include "Node/BundleQueue/BundleQueue.h"
#include "Node/Neighbour/NeighbourTable.h"
#include "Node/Config.h"
#include "Node/BundleQueue/BundleContainer.h"
#include "Bundle/Bundle.h"
#include "Bundle/PrimaryBlock.h"
#include "Bundle/FrameworkMEB.h"
#include "Bundle/FrameworkExtension.h"
#include "Bundle/BundleInfo.h"
#include "Utils/Logger.h"
#include "Node/AppListener/ListeningAppsTable.h"
#include "Node/BundleProcessor/PluginAPI.h"
#include "Utils/globals.h"

NEW_PLUGIN(FirstADTNPlusFwk, "First active DTN framework", "1.0",
           "This processor allows to implement up to 5 functions.")

const std::string FirstADTNPlusFwk::m_header = "#include <vector>\n"
    "#include <exception>\n"
    "#include <stdexcept>\n"
    "#include <string>\n"
    "#include <algorithm>\n"
    "#include <cstdlib>\n"
    "#include <ctime>\n"
    "#include \"Bundle/BundleInfo.h\"\n"
    "#include \"adtnPlus/Json.h\"\n"
    "#include \"adtnPlus/Worker.h\"\n"
    "extern \"C\" {\n"
    "const uint64_t g_timeFrom2000 = 946684800;\n";
const std::string FirstADTNPlusFwk::m_bigSignature =
    "%s f(Json ns, nlohmann::json bs, nlohmann::json bps, BundleInfo bi,"
        " Worker<%s, Json, nlohmann::json, BundleInfo> worker) {\n"
        "auto super = [&]() {try{\n"
        "worker.execute(ns, bps, bi);\n"
        "return worker.getResult();\n"
        "} catch (const WorkerException &e) {\n"
        "throw e;\n"
        "}};";
const std::string FirstADTNPlusFwk::m_littleSignature =
    "%s f(Json ns, nlohmann::json bps, BundleInfo bi) {\n";
const std::string FirstADTNPlusFwk::m_footer = "return %s;}}";
const std::string FirstADTNPlusFwk::m_commandLine =
    "g++ -w -fPIC -shared -std=c++14 %s -o %s 2>&1";

FirstADTNPlusFwk::FirstADTNPlusFwk()
    : m_voidWorker(m_header + stringFormat(m_bigSignature, "bool", "bool"),
                   stringFormat(m_footer, "true"), "f", m_commandLine, "./"),
      m_boolWorker(m_header + stringFormat(m_bigSignature, "bool", "bool"),
                   stringFormat(m_footer, "true"), "f", m_commandLine, "./"),
      m_vectorWorker(
          m_header
              + stringFormat(m_bigSignature, "std::vector<std::string>",
                             "std::vector<std::string>"),
          m_footer, "f", m_commandLine, "./"),
      m_ext1DefaultWorker(m_header + stringFormat(m_littleSignature, "bool"),
                          stringFormat(m_footer, "true"), "f", m_commandLine,
                          "./"),
      m_ext2DefaultWorker(m_header + stringFormat(m_littleSignature, "bool"),
                          stringFormat(m_footer, "true"), "f", m_commandLine,
                          "./"),
      m_ext3DefaultWorker(m_header + stringFormat(m_littleSignature, "bool"),
                          stringFormat(m_footer, "false"), "f", m_commandLine,
                          "./"),
      m_ext4DefaultWorker(m_header + stringFormat(m_littleSignature, "bool"),
                          stringFormat(m_footer, "false"), "f", m_commandLine,
                          "./"),
      m_ext5DefaultWorker(
          m_header
              + stringFormat(m_littleSignature, "std::vector<std::string>"),
          stringFormat(m_footer, "std::vector<std::string>()"), "f",
          m_commandLine, "./") {
}

FirstADTNPlusFwk::~FirstADTNPlusFwk() {
  if (!m_nodeState.empty()) {
    std::ofstream nodeState(m_config.getNodeStatePath());
    nodeState << m_nodeState.dump(2);
    nodeState.close();
  }
}

void FirstADTNPlusFwk::start(
    Config config, std::shared_ptr<BundleQueue> bundleQueue,
    std::shared_ptr<NeighbourTable> neighbourTable,
    std::shared_ptr<ListeningAppsTable> listeningAppsTable) {
  BundleProcessor::start(config, bundleQueue, neighbourTable,
                         listeningAppsTable);
  std::ifstream nodeState(m_config.getNodeStatePath());
  m_nodeState.start(m_neighbourTable);
  m_voidWorker.setPath(m_config.getCodesPath());
  m_boolWorker.setPath(m_config.getCodesPath());
  m_vectorWorker.setPath(m_config.getCodesPath());
  m_ext1DefaultWorker.setPath(m_config.getCodesPath());
  m_ext2DefaultWorker.setPath(m_config.getCodesPath());
  m_ext3DefaultWorker.setPath(m_config.getCodesPath());
  m_ext4DefaultWorker.setPath(m_config.getCodesPath());
  m_ext5DefaultWorker.setPath(m_config.getCodesPath());
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
      std::string defaultBundleCreation =
          m_nodeState["configuration"]["defaultCodes"]["creation"];
      std::string defaultBundleDeletion =
          m_nodeState["configuration"]["defaultCodes"]["deletion"];
      try {
        m_ext5DefaultWorker.generateFunction(defaultForwardingCode);
        m_ext4DefaultWorker.generateFunction(defaultLifeCode);
        m_ext3DefaultWorker.generateFunction(defaultDestinationCode);
        m_ext2DefaultWorker.generateFunction(defaultBundleCreation);
        m_ext1DefaultWorker.generateFunction(defaultBundleDeletion);
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

void FirstADTNPlusFwk::processBundle(
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

std::unique_ptr<BundleContainer> FirstADTNPlusFwk::createBundleContainer(
    std::unique_ptr<Bundle> bundle) {
  LOG(55) << "Creating bundle container.";
  std::unique_ptr<BundleContainer> bc = std::unique_ptr<BundleContainer>(
      new BundleContainer(std::move(bundle)));
  nlohmann::json &bundleProcessState = bc->getState();
  BundleInfo bi = BundleInfo(bc->getBundle());
  try {
    LOG(55)
        << "Checking if bundle contains an extension of value: "
        << static_cast<int>(FirstFrameworkExtensionsIds::CONTAINER_CREATION);
    std::string code = bc->getBundle().getFwkExt(
        static_cast<uint8_t>(FrameworksIds::FIRST_FRAMEWORK),
        static_cast<uint8_t>(FirstFrameworkExtensionsIds::CONTAINER_CREATION))
        ->getSwSrcCode();
    m_voidWorker.generateFunction(code);
    nlohmann::json &bundleState = bc->getBundle().getFwk(
        static_cast<uint8_t>(FrameworksIds::FIRST_FRAMEWORK))->getBundleState();
    m_voidWorker.execute(m_nodeState, bundleState, bundleProcessState, bi,
                         m_ext1DefaultWorker);
    m_voidWorker.getResult();
  } catch (const std::runtime_error &e) {
    LOG(51) << "The code in the bundle has not been executed, : " << e.what();
    try {
      LOG(55) << "Trying to execute the default code.";
      m_ext1DefaultWorker.execute(m_nodeState, bundleProcessState, bi);
      m_ext1DefaultWorker.getResult();
    } catch (const WorkerException &e) {
      LOG(11) << "[Extension 1] Cannot execute any code in "
              "Bundle container creation.";
    }
  }
  return std::move(bc);
}

bool FirstADTNPlusFwk::checkDestination(BundleContainer &bundleContainer) {
  LOG(55) << "Checking destination.";
  nlohmann::json &bundleProcessState = bundleContainer.getState();
  BundleInfo bi = BundleInfo(bundleContainer.getBundle());
  try {
    LOG(55) << "Checking if bundle contains an extension of value: "
            << static_cast<int>(FirstFrameworkExtensionsIds::DESTINATION);
    std::string code = bundleContainer.getBundle().getFwkExt(
        static_cast<uint8_t>(FrameworksIds::FIRST_FRAMEWORK),
        static_cast<uint8_t>(FirstFrameworkExtensionsIds::DESTINATION))
        ->getSwSrcCode();
    m_boolWorker.generateFunction(code);
    nlohmann::json &bundleState = bundleContainer.getBundle().getFwk(
        static_cast<uint8_t>(FrameworksIds::FIRST_FRAMEWORK))->getBundleState();
    m_boolWorker.execute(m_nodeState, bundleState, bundleProcessState, bi,
                         m_ext3DefaultWorker);
    return m_boolWorker.getResult();
  } catch (const std::runtime_error &e) {
    LOG(51) << "The code in the bundle has not been executed, : " << e.what();
    try {
      LOG(55) << "Trying to execute the default code.";
      m_ext3DefaultWorker.execute(m_nodeState, bundleProcessState, bi);
      return m_ext3DefaultWorker.getResult();
    } catch (const WorkerException &e) {
      LOG(11) << "[Extension 3] Cannot execute any code to check destination.";
      return false;
    }
  }
}

std::vector<std::string> FirstADTNPlusFwk::checkDispatch(
    BundleContainer &bundleContainer) {
  std::string destination = bundleContainer.getBundle().getPrimaryBlock()
      ->getDestination();
  std::string appId = destination.substr(destination.find(":") + 1);
  std::vector<std::string> dispatch;
  dispatch.push_back(appId);
  return dispatch;
}

std::vector<std::string> FirstADTNPlusFwk::checkForward(
    BundleContainer &bundleContainer) {
  LOG(55) << "Checking forward.";
  nlohmann::json &bundleProcessState = bundleContainer.getState();
  BundleInfo bi = BundleInfo(bundleContainer.getBundle());
  try {
    LOG(55) << "Checking if bundle contains an extension of value: "
            << static_cast<int>(FirstFrameworkExtensionsIds::FORWARD);
    std::string code = bundleContainer.getBundle().getFwkExt(
        static_cast<uint8_t>(FrameworksIds::FIRST_FRAMEWORK),
        static_cast<uint8_t>(FirstFrameworkExtensionsIds::FORWARD))
        ->getSwSrcCode();
    m_vectorWorker.generateFunction(code);
    nlohmann::json &bundleState = bundleContainer.getBundle().getFwk(
        static_cast<uint8_t>(FrameworksIds::FIRST_FRAMEWORK))->getBundleState();
    m_vectorWorker.execute(m_nodeState, bundleState, bundleProcessState, bi,
                           m_ext5DefaultWorker);
    auto result = m_vectorWorker.getResult();
    return result;
  } catch (const std::runtime_error &e) {
    LOG(51) << "The code in the bundle has not been executed, : " << e.what();
    try {
      LOG(55) << "Trying to execute the default code.";
      m_ext5DefaultWorker.execute(m_nodeState, bundleProcessState, bi);
      return m_ext5DefaultWorker.getResult();
    } catch (const WorkerException &e) {
      LOG(11) << "[Extension 5] Cannot execute any code to check forward.";
      return std::vector<std::string>();
    }
  }
}

bool FirstADTNPlusFwk::checkLifetime(BundleContainer &bundleContainer) {
  LOG(55) << "Checking lifetime.";
  nlohmann::json &bundleProcessState = bundleContainer.getState();
  BundleInfo bi = BundleInfo(bundleContainer.getBundle());
  try {
    LOG(55) << "Checking if bundle contains an extension of value: "
            << static_cast<int>(FirstFrameworkExtensionsIds::LIFETIME);
    std::string code = bundleContainer.getBundle().getFwkExt(
        static_cast<uint8_t>(FrameworksIds::FIRST_FRAMEWORK),
        static_cast<uint8_t>(FirstFrameworkExtensionsIds::LIFETIME))
        ->getSwSrcCode();
    m_boolWorker.generateFunction(code);
    nlohmann::json &bundleState = bundleContainer.getBundle().getFwk(
        static_cast<uint8_t>(FrameworksIds::FIRST_FRAMEWORK))->getBundleState();
    m_boolWorker.execute(m_nodeState, bundleState, bundleProcessState, bi,
                         m_ext4DefaultWorker);
    return m_boolWorker.getResult();
  } catch (const std::runtime_error &e) {
    LOG(51) << "The code in the bundle has not been executed, : " << e.what();
    try {
      LOG(55) << "Trying to execute the default code.";
      m_ext4DefaultWorker.execute(m_nodeState, bundleProcessState, bi);
      return m_ext4DefaultWorker.getResult();
    } catch (const WorkerException &e) {
      LOG(11) << "[Extension 4] Cannot execute any code to check lifetime.";
      return false;
    }
  }
}

void FirstADTNPlusFwk::discard(
    std::unique_ptr<BundleContainer> bundleContainer) {
  nlohmann::json &bundleProcessState = bundleContainer->getState();
  BundleInfo bi = BundleInfo(bundleContainer->getBundle());
  try {
    LOG(55)
        << "Checking if bundle contains an extension of value: "
        << static_cast<int>(FirstFrameworkExtensionsIds::CONTAINER_DELETION);
    std::string code = bundleContainer->getBundle().getFwkExt(
        static_cast<uint8_t>(FrameworksIds::FIRST_FRAMEWORK),
        static_cast<uint8_t>(FirstFrameworkExtensionsIds::CONTAINER_DELETION))
        ->getSwSrcCode();
    m_voidWorker.generateFunction(code);
    nlohmann::json &bundleState = bundleContainer->getBundle().getFwk(
        static_cast<uint8_t>(FrameworksIds::FIRST_FRAMEWORK))->getBundleState();
    m_voidWorker.execute(m_nodeState, bundleState, bundleProcessState, bi,
                         m_ext2DefaultWorker);
    m_voidWorker.getResult();
  } catch (const std::runtime_error &e) {
    LOG(51) << "The code in the bundle has not been executed, : " << e.what();
    try {
      LOG(55) << "Trying to execute the default code.";
      m_ext2DefaultWorker.execute(m_nodeState, bundleProcessState, bi);
      m_ext2DefaultWorker.getResult();
    } catch (const WorkerException &e) {
      LOG(11) << "[Extension 2] Cannot execute any code in "
              "Bundle container deletion.";
    }
  }
  BundleProcessor::discard(std::move(bundleContainer));
}

void FirstADTNPlusFwk::checkNodeStateChanges() {
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
        // m_forwardWorker.generateFunction(code);
      } catch (const WorkerException &e) {
        LOG(11) << "Cannot create forward code worker, reason: " << e.what();
      }
    }
    code = m_nodeState["configuration"]["defaultCodes"]["lifetime"];
    if (code.compare(
        m_oldNodeState["configuration"]["defaultCodes"]["lifetime"]) != 0) {
      try {
        // m_lifeWorker.generateFunction(code);
      } catch (const WorkerException &e) {
        LOG(11) << "Cannot create life code worker, reason: " << e.what();
      }
    }
    m_oldNodeState = m_nodeState;
  }
}

