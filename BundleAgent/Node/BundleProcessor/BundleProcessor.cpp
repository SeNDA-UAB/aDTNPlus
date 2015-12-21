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
 * FILE BundleProcessor.cpp
 * AUTHOR Blackcatn13
 * DATE Dec 17, 2015
 * VERSION 1
 *
 */

#include "Node/BundleProcessor/BundleProcessor.h"
#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <cstdio>
#include "Node/BundleQueue/BundleQueue.h"
#include "Node/Neighbour/NeighbourTable.h"
#include "Node/Config.h"
#include "Node/BundleQueue/BundleContainer.h"
#include "Bundle/Bundle.h"
#include "Bundle/PrimaryBlock.h"
#include "Utils/globals.h"
#include "Utils/Logger.h"

BundleProcessor::BundleProcessor(
    Config config, std::shared_ptr<BundleQueue> bundleQueue,
    std::shared_ptr<NeighbourTable> neighbourTable
    /*, std::shared_ptr<ListeningAppsTable> listeningAppsTable*/)
    : m_config(config),
      m_bundleQueue(bundleQueue),
      m_neighbourTable(neighbourTable) {
  std::thread t = std::thread(&BundleProcessor::processBundles, this);
  t.detach();
  t = std::thread(&BundleProcessor::receiveBundles, this);
  t.detach();
}

BundleProcessor::~BundleProcessor() {
}

void BundleProcessor::processBundles() {
  while (!g_stop.load()) {
    try {
      std::unique_ptr<BundleContainer> bc = m_bundleQueue->dequeue();
      processBundle(std::move(bc));
    } catch (const std::exception &e) {
    }
  }
}

void BundleProcessor::receiveBundles() {
}

void BundleProcessor::dispatch(const Bundle& bundle,
                               std::vector<std::string> destinations) {
}

void BundleProcessor::forward(const Bundle& bundle,
                              std::vector<std::string> nextHop) {
}

void BundleProcessor::discard(
    std::unique_ptr<BundleContainer> bundleContainer) {
  std::stringstream ss;
  ss << m_config.getDataPath()
     << bundleContainer->getBundle().getPrimaryBlock()->getCreationTimestamp()
     << "_"
     << bundleContainer->getBundle().getPrimaryBlock()
         ->getCreationTimestampSeqNumber()
     << ".bundle";
  int success = std::remove(ss.str().c_str());
  if (success != 0) {
    LOG(3) << "Cannot delete bundle " << ss.str();
  }
  LOG(41) << "Deleting bundleContainer.";
  bundleContainer.reset();
}

void BundleProcessor::restore(
    std::unique_ptr<BundleContainer> bundleContainer) {
  m_bundleQueue->enqueue(std::move(bundleContainer));
}
