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
 * FILE OppnetFlowBundleProcessor.cpp
 * AUTHOR MCarmen
 * DATE Nov 20, 2017
 * VERSION 1
 *
 */

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <ctime>
#include <fstream>
#include <sstream>
#include <functional>
#include <iterator>
#include <stdexcept>
#include <memory>
#include <thread>
#include <iostream>
#include "Bundle/Bundle.h"
#include "Bundle/MetadataExtensionBlock.h"
#include "Bundle/PrimaryBlock.h"
#include "Bundle/ControlMetricsMEB.h"
#include "Bundle/ControlDirectiveMEB.h"
#include "ExternTools/json/json.hpp"
#include "Node/BundleProcessor/OppnetFlow/ForwardingAlgorithm.h"
#include "Node/BundleProcessor/OppnetFlow/ForwardingAlgorithmFactory.h"
#include "Node/BundleProcessor/OppnetFlow/SprayAndWaitAlgorithm.h"
#include "Node/BundleProcessor/OppnetFlow/OppnetFlowBundleProcessor.h"
#include "Node/BundleProcessor/OppnetFlow/ControlDrivenDropPolicy.h"
#include "Node/BundleProcessor/PluginAPI.h"
#include "Node/BundleQueue/BundleQueue.h"
#include "Node/BundleQueue/BundleContainer.h"
#include "Node/Config.h"
#include "Node/EndpointListener/ListeningEndpointsTable.h"
#include "Node/Neighbour/NeighbourTable.h"
#include "Utils/globals.h"
#include "Utils/Logger.h"
#include "Utils/TimestampManager.h"

NEW_PLUGIN(OppnetFlowBundleProcessor, "Basic bundle processor", "1.0",
           "Implements oppnetFlow protocol.")

OppnetFlowBundleProcessor::OppnetFlowBundleProcessor()
    : m_controlState(m_nodeState) {
}

OppnetFlowBundleProcessor::~OppnetFlowBundleProcessor() {
}

OppnetFlowBundleProcessor::ControlState::ControlState(NodeStateJson& nodeState)
    : m_nodeState_ref(nodeState) {
}

OppnetFlowBundleProcessor::ControlState::~ControlState() {
}

const bool OppnetFlowBundleProcessor::ControlState::isControlReportingActive() const {
  return static_cast<bool>(m_nodeState_ref["oppnetFlow"]["control"]["controlReportings"]["active"]);
}

const bool OppnetFlowBundleProcessor::ControlState::hasJoinedAsAController() const {
  return static_cast<bool>(m_nodeState_ref["oppnetFlow"]["control"]["joinedAsAController"]);
}

const std::string OppnetFlowBundleProcessor::ControlState::getControllersGroupId() const {
  return m_nodeState_ref["oppnetFlow"]["control"]["controllersGroupId"];
}

const std::string OppnetFlowBundleProcessor::ControlState::getLastControlBundleId() const {
  return m_nodeState_ref["oppnetFlow"]["control"]["controlReportings"]["lastControlBundleId"];
}

void OppnetFlowBundleProcessor::ControlState::setLastControlBundleId(
    const std::string& lastControlBundleId) {
  m_nodeState_ref["oppnetFlow"]["control"]["controlReportings"]["lastControlBundleId"] =
      lastControlBundleId;
}

const bool OppnetFlowBundleProcessor::ControlState::doWeHaveToExecuteControlDirectives() const {
  return m_nodeState_ref["oppnetFlow"]["control"]["executeControlDirectives"];
}

OppnetFlowBundleProcessor::ControlParameters::ControlParameters(
    NodeStateJson& nodeState) {
  int frequency =
      (int) nodeState["oppnetFlow"]["control"]["controlReportings"]["frequency"];
  m_fields[ControlParameterCode::REPORT_FREQUENCY] = frequency;

}

OppnetFlowBundleProcessor::ControlParameters::~ControlParameters() {
}

void OppnetFlowBundleProcessor::removeBundleFromDisk(std::string bundleId) {
  std::stringstream ss;
  ss << m_config.getDataPath() << bundleId << ".bundle";
  int success = std::remove(ss.str().c_str());
  if (success != 0) {
    throw RemoveBundleFromDiskException(bundleId);
  }
}

void OppnetFlowBundleProcessor::sendNetworkMetrics() {
  if (!m_networkMetrics.isEmpty()) {
    LOG(LOG_SEND_METRICS) << "[OppnetFlowProcessor] "
                          << "Sending network control metrics: "
                          << m_networkMetrics.toString().c_str();
    std::unique_ptr<Bundle> bundle_ptr(
        new Bundle(m_nodeState["id"], m_controlState.getControllersGroupId(),
                   ""));
    std::shared_ptr<ControlMetricsMEB> metricsMEB_ptr(
        new ControlMetricsMEB(m_networkMetrics.getSetMapedFields()));

    std::string bundleId = bundle_ptr->getId();
    m_controlState.setLastControlBundleId(bundleId);
    bundle_ptr->addBlock(
        std::static_pointer_cast<CanonicalBlock>(metricsMEB_ptr));
    std::unique_ptr<BundleContainer> bc_ptr = createBundleContainer(
        std::move(bundle_ptr));
    bc_ptr->getState()["bundleType"] = 1;  //BundleType::CONTROL;
    m_bundleQueue->saveBundleToDisk(m_config.getDataPath(), *bc_ptr);

    try {
      //forcing the control messages to fit in the queue.
      ControlDrivenDropPolicy cdp =
          *(std::dynamic_pointer_cast<ControlDrivenDropPolicy>(getDropPolicy()));

      m_bundleQueue->enqueue(std::move(bc_ptr), false, cdp, true);
      //m_bundleQueue->enqueue(std::move(bc_ptr), false);
      removeBundleFromDisk(bundleId);
    } catch (const DroppedBundleQueueException &e) {
      LOG(55) << e.what();
    } catch (RemoveBundleFromDiskException &e) {
      LOG(55) << e.what();
    }
  } else {
    LOG(LOG_NO_METRICS_TO_BE_SENT) << "[OppnetFlowProcessor] "
                                   << "No metrics to be sent: ";
  }
}

void OppnetFlowBundleProcessor::sendNetworkMetricsAndSleep() {
  uint32_t sleepTime =
      m_controlParameters[ControlParameterCode::REPORT_FREQUENCY];
  g_startedThread++;

  LOG(LOG_SEND_METRICS_THREAD) << "Creating reportingNetworkMetrics thread";
  while (!g_stop.load()) {
    std::this_thread::sleep_for(std::chrono::seconds(sleepTime));
    sendNetworkMetrics();
    m_networkMetrics.reset();
  }  //end while true loop
  LOG(LOG_SEND_METRICS_THREAD) << "Exit reportingNetworkMetrics thread.";
  g_stopped++;
}

void OppnetFlowBundleProcessor::scheduleReportingNetworkMetrics() {
  std::thread networkMetricsReporter(
      &OppnetFlowBundleProcessor::sendNetworkMetricsAndSleep, this);
  networkMetricsReporter.detach();
}

void OppnetFlowBundleProcessor::start(
    Config config, std::shared_ptr<BundleQueue> bundleQueue,
    std::shared_ptr<NeighbourTable> neighbourTable,
    std::shared_ptr<ListeningEndpointsTable> listeningAppsTable) {
  BundleProcessor::start(config, bundleQueue, neighbourTable,
                         listeningAppsTable);
  m_nodeState.start(
      std::bind(&NeighbourTable::getConnectedEID, m_neighbourTable),
      std::bind(&NeighbourTable::getSingletonConnectedEID, m_neighbourTable),
      std::bind(&ListeningEndpointsTable::getValues, m_listeningAppsTable));
  std::ifstream nodeState(m_config.getNodeStatePath());
  if (nodeState) {
    try {
      nodeState >> m_nodeState;
      nodeState.close();
      m_forwardingAlgorithmFactory = ForwardingAlgorithmFactory(m_nodeState);
      m_controlParameters = ControlParameters(m_nodeState);
    } catch (const std::invalid_argument &e) {
      LOG(1) << "Error in NodeState json: " << e.what();
      LOG(15) << "No ForwardingAlgorithmFactory has been created due to ";
      LOG(15) << "no nodeState ";
      g_stop = true;
    }
    if (m_controlState.isControlReportingActive()) {
      scheduleReportingNetworkMetrics();
    }
  } else {
    LOG(11) << "Cannot open the file " << m_config.getNodeStatePath();
  }
}

void OppnetFlowBundleProcessor::drop() {
  m_networkMetrics.incrementField(NetworkMetricsControlCode::NR_OF_DROPS);
}

void OppnetFlowBundleProcessor::delivered() {
  m_networkMetrics.incrementField(NetworkMetricsControlCode::NR_OF_DELIVERIES);
}

void OppnetFlowBundleProcessor::processRecivedControlBundleIfNecessary(
    BundleContainer &bundleContainer) {
  if (m_controlState.doWeHaveToExecuteControlDirectives()) {
    std::shared_ptr<ControlDirectiveMEB> controlMEB_ptr =
        std::static_pointer_cast<ControlDirectiveMEB>(
            (bundleContainer.getBundle()).findMetadataExtensionBlock(
                MetadataTypes::CONTROL_DIRECTIVE_MEB));
    if (controlMEB_ptr != nullptr) {
      m_controlDirectives = NumericMapedFields<DirectiveControlCode>(
          controlMEB_ptr->getFields());
    }
    for (auto& controlDirective : m_controlDirectives.getSetMapedFields()) {
      switch (controlDirective.first) {
        case DirectiveControlCode::NR_OF_COPIES:
          m_controlParameters[ControlParameterCode::NR_OF_COPIES] =
              controlDirective.second;
          break;
        case DirectiveControlCode::REPORT_FREQUENCY:
          m_controlParameters[ControlParameterCode::REPORT_FREQUENCY] =
              controlDirective.second;
          break;
        default:
          break;
      }
    }
  }
}

void OppnetFlowBundleProcessor::applyControlSetupToForwardingAlgorithmIfNecessary(
    ForwardingAlgorithm& forwardingAlgorithm) {
  if (areThereControlDirectives()) {
    if (forwardingAlgorithm.getType() == ForwardAlgorithmType::SPRAYANDWAIT) {
      try {
        (static_cast<SprayAndWaitAlgorithm&>(forwardingAlgorithm)).setNrofCopies(
            static_cast<int16_t>(m_controlParameters.getFieldAt(
                ControlParameterCode::NR_OF_COPIES)));
      } catch (std::out_of_range& e) {
      }
    }
  }
}

std::vector<std::string> OppnetFlowBundleProcessor::checkDispatch(
    BundleContainer &bundleContainer) {
  BundleInfo bi(bundleContainer.getBundle());
  std::vector<std::string> dispatch;
  dispatch.push_back(bi.getDestination());
  return dispatch;
}

bool OppnetFlowBundleProcessor::checkLifetime(
    BundleContainer &bundleContainer) {
  bool expired = false;
  BundleInfo bi(bundleContainer.getBundle());
  if (bi.getLifetime()
      < (time(NULL) - g_timeFrom2000 - bi.getCreationTimestamp())) {
    expired = true;
  } else if (isAControlBundle(bundleContainer) && amITheOriginOfTheBundle(bi)
      && !isTheFresherControlBundle(bundleContainer)) {
    expired = true;
  }
  return expired;
}

bool OppnetFlowBundleProcessor::isTheFresherControlBundle(
    BundleContainer& bc) const {

  return (bc.getBundle().getId() == m_controlState.getLastControlBundleId());
}

bool OppnetFlowBundleProcessor::checkDestination(
    BundleContainer &bundleContainer) {
  BundleInfo bi = BundleInfo(bundleContainer.getBundle());
  bool amIDestination = ((m_controlState.hasJoinedAsAController()
      && (bi.getDestination() == m_controlState.getControllersGroupId()))
      || (bi.getDestination() == m_config.getNodeId()));

  return amIDestination;
}

bool OppnetFlowBundleProcessor::processBundle(
    std::unique_ptr<BundleContainer> bundleContainer) {
  using namespace std::placeholders;
  std::vector<std::string> nextHop = std::vector<std::string>();
  bool processed = true;

  LOG(51) << "Processing a bundle container.";
  LOG(55) << "Checking destination node.";
  if (checkDestination(*bundleContainer)) { //I am a destination as a node or as a controller.
    LOG(55) << "We are the destination node.";
    processRecivedControlBundleIfNecessary(*bundleContainer);
    delivered();
    LOG(55) << "Checking destination app listening.";
    std::vector<std::string> destinations = checkDispatch(*bundleContainer);
    if (destinations.size() > 0) {
      LOG(55) << "There is a listening app, dispatching the bundle.";
      delivery(*bundleContainer, destinations);
      discard(std::move(bundleContainer));
    } else {
      LOG(55) << "No listening app, restoring the bundle.";
      restore(std::move(bundleContainer));
    }
  } else { //We are not the destination node
    LOG(55) << "We are not the destination node.";
    LOG(55) << "Checking lifetime.";
    if (checkLifetime(*bundleContainer)) {
      LOG(55) << "Bundle: " << bundleContainer->getBundle().getId()
              << " expired, discarding it.";
      discard(std::move(bundleContainer));
    } else {
      LOG(55) << "Bundle is not expired yet.";
      LOG(55) << "Checking neighbors. " << "";
      std::vector<std::string> neighbours = m_neighbourTable
          ->getSingletonConnectedEID();
      if (neighbours.size() > 0) {
        LOG(55) << "There are some neighbours.";
        LOG(55) << "Checking if one of them is the bundle destination.";
        auto it = std::find(
            neighbours.begin(), neighbours.end(),
            bundleContainer->getBundle().getPrimaryBlock()->getDestination());
        if (it != neighbours.end()) {
          LOG(55) << "Destination found, sending the bundle to it.";
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
          LOG(55)
              << "Destination not found, "
              << "Forwarding the bundle following sprayAndWait algorithm to "
              << "The neighbors list.";
          try {
            std::unique_ptr<ForwardingAlgorithm> forwardingAlgorithm =
                m_forwardingAlgorithmFactory.getForwardingAlgorithm(
                    bundleContainer->getBundle());
            applyControlSetupToForwardingAlgorithmIfNecessary(
                *forwardingAlgorithm);
            auto f_ptr = std::bind(&OppnetFlowBundleProcessor::forward, this,
                                   _1, _2);
            processed = processed
                && (forwardingAlgorithm->doForward(bundleContainer->getBundle(),
                                                   neighbours, f_ptr));
            restore(std::move(bundleContainer));
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
  return processed;
}

std::unique_ptr<BundleContainer> OppnetFlowBundleProcessor::createBundleContainer(
    std::unique_ptr<Bundle> bundle) {
  std::unique_ptr<BundleContainer> bc(new BundleContainer(std::move(bundle)));
  //bc->getState()["bundleType"] = static_cast<uint8_t>(BundleType::DEFAULT);

  return bc;
}

bool OppnetFlowBundleProcessor::isAControlBundle(BundleContainer& bc) const {
  Bundle bundle = bc.getBundle();

  bool isAControlBundle = (bc.getState()["bundleType"] == 0) ?  //BundleType::DEFAULT) ?
      false : true;
  return isAControlBundle;
}

bool OppnetFlowBundleProcessor::amITheOriginOfTheBundle(BundleInfo& bi) {
  return (m_nodeState["id"] == bi.getSource());
}

std::shared_ptr<DropPolicy> OppnetFlowBundleProcessor::getDropPolicy() {
  if (m_dropPolicy == nullptr) {
    m_dropPolicy = std::shared_ptr<DropPolicy>(
        new ControlDrivenDropPolicy(
            m_nodeState["id"],
            m_nodeState["oppnetFlow"]["control"]["forwardPriorization"]));
  }
  return m_dropPolicy;
}

const bool OppnetFlowBundleProcessor::areThereControlDirectives() const {
  return (!m_controlDirectives.isEmpty());
}


