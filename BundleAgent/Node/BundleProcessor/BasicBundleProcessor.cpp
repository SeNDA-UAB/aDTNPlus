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
#include "Node/Config.h"
#include "Node/BundleQueue/BundleContainer.h"
#include "Bundle/Bundle.h"
#include "Bundle/PrimaryBlock.h"

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
  if (bundleContainer->getBundle().getPrimaryBlock()->getDestination()
      == m_config.getNodeId()) {
    std::vector<std::string> destinations = checkDestination();
    if (destinations.size() > 0) {
      dispatch(bundleContainer->getBundle(), destinations);
      discard(std::move(bundleContainer));
    } else {
      restore(std::move(bundleContainer));
    }
  } else {
    if (checkLifetime()) {
      discard(std::move(bundleContainer));
    } else {
      std::vector<std::string> neighbours = checkForward();
      auto it = std::find(neighbours.begin(), neighbours.end(),
                          bundleContainer->getFrom());
      if (it != neighbours.end()) {
        neighbours.erase(it);
      }
      if (neighbours.size() > 0) {
        it = std::find(
            neighbours.begin(), neighbours.end(),
            bundleContainer->getBundle().getPrimaryBlock()->getDestination());
        if (it != neighbours.end()) {
          std::vector<std::string> nextHop = std::vector<std::string>();
          nextHop.push_back(*it);
          forward(bundleContainer->getBundle(), nextHop);
        } else {
          forward(bundleContainer->getBundle(), neighbours);
        }
        discard(std::move(bundleContainer));
      } else {
        restore(std::move(bundleContainer));
      }
    }
  }
}

void BasicBundleProcessor::createBundleContainer(
    std::shared_ptr<Neighbour> from, std::unique_ptr<Bundle> bundle) {
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
