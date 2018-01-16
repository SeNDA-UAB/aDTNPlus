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

#include "Bundle/Bundle.h"
#include "Bundle/BundleInfo.h"
#include "Bundle/MetadataExtensionBlock.h"
#include "Bundle/PrimaryBlock.h"
#include "Bundle/ControlMetricsMEB.h"
#include "Bundle/ControlDirectiveMEB.h"
#include "ExternTools/json/json.hpp"
#include "Node/BundleProcessor/OppnetFlow/ForwardingAlgorithm.h"
#include "Node/BundleProcessor/OppnetFlow/ForwardingAlgorithmFactory.h"
#include "Node/BundleProcessor/OppnetFlow/OppnetFlowBundleProcessor.h"
#include "Node/BundleProcessor/PluginAPI.h"
#include "Node/BundleQueue/BundleQueue.h"
#include "Node/BundleQueue/BundleContainer.h"
#include "Node/Config.h"
#include "Node/EndpointListener/ListeningEndpointsTable.h"
#include "Node/Neighbour/NeighbourTable.h"
#include "Utils/globals.h"
#include "Utils/Logger.h"
#include "Utils/TimestampManager.h"
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

NEW_PLUGIN(OppnetFlowBundleProcessor, "Basic bundle processor", "1.0",
           "Implements oppnetFlow protocol.")

OppnetFlowBundleProcessor::OppnetFlowBundleProcessor() {
}

OppnetFlowBundleProcessor::~OppnetFlowBundleProcessor() {
}

OppnetFlowBundleProcessor::RemoveBundleFromDiskException::RemoveBundleFromDiskException(
    std::string bundleId)
    : m_bundleId(bundleId) {
}
OppnetFlowBundleProcessor::RemoveBundleFromDiskException::~RemoveBundleFromDiskException(){}

const char*  OppnetFlowBundleProcessor::RemoveBundleFromDiskException::what() const throw(){
  std::stringstream ss;

  ss << "The bundle with ID: " << m_bundleId <<
      " has not been removed from disk";

  return ss.str().c_str();
}

const bool OppnetFlowBundleProcessor::ControlState::isControlReportingActive() const{
  return static_cast<bool>(m_nodeState["oppnetFlow"]["control"]["controlReportings"]["active"]);;
}

const bool OppnetFlowBundleProcessor::ControlState::isJoinedAsAController() const {
  return static_cast<bool>(m_nodeState["oppnetFlow"]["control"]["joinedAsAController"]);
}

const std::string& OppnetFlowBundleProcessor::ControlState::getControllersGroupId() const {
  return m_nodeState["oppnetFlow"]["control"]["controllersGroupId"];
}

const std::string& OppnetFlowBundleProcessor::ControlState::getLastControlBundleId() const {
  return m_nodeState["oppnetFlow"]["control"]["controlReportings"]["lastControlBundleId"];
}

void OppnetFlowBundleProcessor::ControlState::setLastControlBundleId(const std::string& lastControlBundleId) {
  m_nodeState["oppnetFlow"]["control"]["controlReportings"]["lastControlBundleId"] = lastControlBundleId;
}


const uint16_t OppnetFlowBundleProcessor::ControlState::getReportFrequency() const {
  return m_nodeState["oppnetFlow"]["control"]["controlReportings"]["frequency"];
}

void OppnetFlowBundleProcessor::removeBundleFromDisk(std::string bundleId) {
  std::stringstream ss;
  ss << m_config.getDataPath() << bundleId << ".bundle";
  int success = std::remove(ss.str().c_str());
  if (success != 0) {
    throw OppnetFlowBundleProcessor::RemoveBundleFromDiskException(bundleId);
  }
}

void OppnetFlowBundleProcessor::sendNetworkMetrics() {
  std::unique_ptr<Bundle> bundle_ptr(
      new Bundle(m_nodeState["id"], m_controlState.getControllersGroupId(), ""));
  std::shared_ptr<ControlMetricsMEB> metricsMEB_ptr(
      new ControlMetricsMEB(m_networkMetrics));
  std::unique_ptr<BundleContainer> bc_ptr = createBundleContainer(
      std::move(bundle_ptr));


  bc_ptr->getState()["bundleType"] = BundleType::CONTROL;
  m_controlState.setLastControlBundleId(bundle_ptr->getId());

  bundle_ptr->addBlock(
      std::static_pointer_cast<CanonicalBlock>(metricsMEB_ptr));
  m_bundleQueue->saveBundleToDisk(m_config.getDataPath(), *bc_ptr);

  try {
    m_bundleQueue->enqueue(std::move(bc_ptr));
    removeBundleFromDisk(bundle_ptr->getId());
  } catch (const DroppedBundleQueueException &e) {
    LOG(40) << e.what();
  } catch (OppnetFlowBundleProcessor::RemoveBundleFromDiskException &e) {
    LOG(40) << e.what();
  }
}

void OppnetFlowBundleProcessor::sendNetworkMetricsAndSleep(){
  uint32_t sleepTime = m_controlState.getReportFrequency();
  g_startedThread++;

  LOG(14) << "Creating reportingNetworkMetrics thread";
  while (!g_stop.load()) {
    std::this_thread::sleep_for(std::chrono::seconds(sleepTime));
    LOG(55) << "[OppnetFlowProcessor] " <<
        "Sending network control metrics: " << m_networkMetrics.toString();
    sendNetworkMetrics();
    m_networkMetrics.reset();
  }//end while true loop
  LOG(14) << "Exit reportingNetworkMetrics thread.";
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
    } catch (const std::invalid_argument &e) {
      LOG(1) << "Error in NodeState json: " << e.what();
      LOG(15) << "No ForwardingAlgorithmFactory has been created due to ";
      LOG(15) << "no nodeState ";
      g_stop = true;
    }
    if (m_controlState.isControlReportingActive()){
      scheduleReportingNetworkMetrics();
    }
  } else {
    LOG(11) << "Cannot open the file " << m_config.getNodeStatePath();
  }
}

void OppnetFlowBundleProcessor::drop(){
  m_networkMetrics.incrementField(NetworkMetricsControlCode::NR_OF_DROPS);
}

void OppnetFlowBundleProcessor::delivered(){
  m_networkMetrics.incrementField(NetworkMetricsControlCode::NR_OF_DELIVERIES);
}

void OppnetFlowBundleProcessor::processControlBundle(BundleContainer &bundleContainer) {
  std::shared_ptr<ControlDirectiveMEB> controlMEB = std::static_pointer_cast<
      ControlDirectiveMEB>(
      OppnetFlowBundleProcessor::findMetadataExtensionBlock(
          MetadataTypes::CONTROL_DIRECTIVE_MEB, bundleContainer.getBundle()));

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
  BundleInfo bi(bundleContainer.getBundle());
  if (
      (bi.getLifetime()
      < (time(NULL) - g_timeFrom2000 - bi.getCreationTimestamp())) &&
      isTheFresherControlBundle(bi)
      )
    return true;
  else
    return false;
}


bool OppnetFlowBundleProcessor::isTheFresherControlBundle(
    const BundleInfo& bundleInfo) {
    return (isAControlBundle(bundleInfo) && (bundleInfo.getId() == m_controlState.getLastControlBundleId()) );
  }



bool OppnetFlowBundleProcessor::checkDestination(
    BundleContainer &bundleContainer) {
  BundleInfo bi = BundleInfo(bundleContainer.getBundle());
  return bi.getDestination() == m_config.getNodeId();
}

std::shared_ptr<MetadataExtensionBlock> OppnetFlowBundleProcessor::findMetadataExtensionBlock(
    const MetadataTypes extensionType, Bundle& bundle) {
  std::shared_ptr<MetadataExtensionBlock> found_meb = nullptr;
  std::vector<std::shared_ptr<Block>> blocks = bundle.getBlocks();

  blocks.erase(blocks.begin());
  for (std::size_t i = 0; i < blocks.size(); i++) {
    LOG(55) << "Checking canonical blocks.";
    std::shared_ptr<CanonicalBlock> canonical_block = std::static_pointer_cast<
        CanonicalBlock>(blocks[i]);
    if (static_cast<CanonicalBlockTypes>(canonical_block->getBlockType())
        == CanonicalBlockTypes::METADATA_EXTENSION_BLOCK) {
      LOG(55) << "There is metadata extension block.";
      std::shared_ptr<MetadataExtensionBlock> meb = std::static_pointer_cast<
          MetadataExtensionBlock>(canonical_block);
      if (static_cast<MetadataTypes>(meb->getMetadataType()) == extensionType) {
        found_meb = meb;
        break;
      }
    }
  }
  return found_meb;
}

void OppnetFlowBundleProcessor::processBundle(
    std::unique_ptr<BundleContainer> bundleContainer) {
  using namespace std::placeholders;
  std::vector<std::string> nextHop = std::vector<std::string>();

  LOG(51) << "Processing a bundle container.";
  LOG(55) << "Checking destination node.";
  if (checkDestination(*bundleContainer)) {
    LOG(55) << "We are the destination node.";
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
  } else {
    LOG(55) << "We are not the destination node.";
    LOG(55) << "Checking lifetime.";
    if (checkLifetime(*bundleContainer)) {
      LOG(55) << "Bundle expired, discarding it.";
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
            auto fp = std::bind(&OppnetFlowBundleProcessor::forward, this, _1,
                                _2);
            forwardingAlgorithm->doForward(bundleContainer->getBundle(),
                                           neighbours, fp);
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
}

std::unique_ptr<BundleContainer> OppnetFlowBundleProcessor::createBundleContainer(
    std::unique_ptr<Bundle> bundle) {
  std::unique_ptr<BundleContainer> bc(new BundleContainer(std::move(bundle)));
  bc->getState()["bundleType"] = BundleType::DEFAULT;

  return bc;
}

bool OppnetFlowBundleProcessor::isAControlBundle(
    const BundleContainer& bc) const {
  bool isAControlBundle = (bc.getState()["bundleType"] == BundleType::DEFAULT) ?
      false : true;
  return isAControlBundle;
}
