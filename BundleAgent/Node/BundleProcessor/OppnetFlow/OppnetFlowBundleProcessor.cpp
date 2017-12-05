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
#include "ExternTools/json/json.hpp"
#include "Node/BundleProcessor/OppnetFlow/ForwardingAlgorithm.h"
#include "Node/BundleProcessor/OppnetFlow/ForwardingAlgorithmFactory.h"
#include "Node/BundleProcessor/OppnetFlow/OppnetFlowBundleProcessor.h"
#include "Node/BundleProcessor/PluginAPI.h"
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
#include <functional>
#include <iterator>
#include <stdexcept>

NEW_PLUGIN(OppnetFlowBundleProcessor, "Basic bundle processor", "1.0",
           "Implements oppnetFlow protocol.")

OppnetFlowBundleProcessor::OppnetFlowBundleProcessor() {
}

OppnetFlowBundleProcessor::~OppnetFlowBundleProcessor() {
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

  } else {
    LOG(11) << "Cannot open the file " << m_config.getNodeStatePath();
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
  BundleInfo bi(bundleContainer.getBundle());
  if (bi.getLifetime()
      < (time(NULL) - g_timeFrom2000 - bi.getCreationTimestamp()))
    return true;
  else
    return false;
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
            /*
             nextHop.push_back(neighbours[0]);
             forward(bundleContainer->getBundle(), neighbours);
             */
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
  return std::unique_ptr<BundleContainer>(
      new BundleContainer(std::move(bundle)));
}
