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
#include "Node/BundleProcessor/BundleProcessor.h"
#include "Node/BundleQueue/BundleQueue.h"
#include "Node/Neighbour/NeighbourTable.h"
#include "Node/Neighbour/Neighbour.h"
#include "Node/Config.h"
#include "Node/BundleQueue/BundleContainer.h"
#include "Bundle/Bundle.h"
#include "Bundle/PrimaryBlock.h"
#include "Utils/Logger.h"

BasicBundleProcessor::BasicBundleProcessor(
    Config config, std::shared_ptr<BundleQueue> bundleQueue,
    std::shared_ptr<NeighbourTable> neighbourTable
    /*, std::shared_ptr<ListeningAppsTable> listeningAppsTable*/)
    : BundleProcessor(config, bundleQueue, neighbourTable) {
}

BasicBundleProcessor::~BasicBundleProcessor() {
}

void BasicBundleProcessor::processBundle(
    std::unique_ptr<BundleContainer> bundleContainer) {
  LOG(51) << "Processing a bundle container.";
  LOG(55) << "Checking destination node.";
  if (bundleContainer->getBundle().getPrimaryBlock()->getDestination()
      == m_config.getNodeId()) {
    LOG(55) << "We are the destination node.";
    LOG(55) << "Checking destination app listening.";
    std::vector<std::string> destinations = checkDestination();
    if (destinations.size() > 0) {
      LOG(55) << "There is a listening app, dispatching the bundle.";
      dispatch(bundleContainer->getBundle(), destinations);
      discard(std::move(bundleContainer));
    } else {
      LOG(55) << "No listening app, restoring the bundle.";
      restore(std::move(bundleContainer));
    }
  } else {
    LOG(55) << "We are not the destination node.";
    LOG(55) << "Checking lifetime.";
    if (checkLifetime()) {
      LOG(55) << "Bundle expired, discarding it.";
      discard(std::move(bundleContainer));
    } else {
      LOG(55) << "Bundle is not expired yet.";
      LOG(55) << "Checking neighbours. "
              << "(Removing bundle source if we have it as neighbour)";
      std::vector<std::string> neighbours = checkForward();
      auto it = std::find(neighbours.begin(), neighbours.end(),
                          bundleContainer->getFrom());
      if (it != neighbours.end()) {
        neighbours.erase(it);
      }
      if (neighbours.size() > 0) {
        LOG(55) << "There are some neighbours.";
        LOG(55) << "Checking if one of them is the bundle destination.";
        it = std::find(
            neighbours.begin(), neighbours.end(),
            bundleContainer->getBundle().getPrimaryBlock()->getDestination());
        if (it != neighbours.end()) {
          LOG(55) << "Destination found, sending the bundle to it.";
          std::vector<std::string> nextHop = std::vector<std::string>();
          nextHop.push_back(*it);
          forward(bundleContainer->getBundle(), nextHop);
        } else {
          LOG(55) << "Destination not found, "
                  << "sending the bundle to all the neighbours.";
          forward(bundleContainer->getBundle(), neighbours);
        }
        LOG(55) << "Discarding the bundle.";
        discard(std::move(bundleContainer));
      } else {
        LOG(55) << "No neighbours found, restoring the bundle.";
        restore(std::move(bundleContainer));
      }
    }
  }
}

std::unique_ptr<BundleContainer> BasicBundleProcessor::createBundleContainer(
    std::shared_ptr<Neighbour> from, std::unique_ptr<Bundle> bundle) {
  return std::unique_ptr<BundleContainer>(
      new BundleContainer(from->getNodeId(), std::move(bundle)));
}

std::vector<std::string> BasicBundleProcessor::checkDestination() {
  return std::vector<std::string>();
}

std::vector<std::string> BasicBundleProcessor::checkForward() {
  return std::vector<std::string>();
}

bool BasicBundleProcessor::checkLifetime() {
  return false;
}
