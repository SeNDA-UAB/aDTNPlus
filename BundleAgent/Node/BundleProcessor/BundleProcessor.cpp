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
#include "Node/BundleQueue/BundleQueue.h"
#include "Node/Neighbour/NeighbourTable.h"
#include "Node/Config.h"
#include "Node/BundleQueue/BundleContainer.h"
#include "Bundle/Bundle.h"

BundleProcessor::BundleProcessor(
    Config config, std::shared_ptr<BundleQueue> bundleQeuue,
    std::shared_ptr<NeighbourTable> neighbourTable
    /*, std::shared_ptr<ListeningAppsTable> listeningAppsTable*/)
    : m_config(config),
      m_bundleQueue(bundleQeuue),
      m_neighbourTable(neighbourTable) {
}

BundleProcessor::~BundleProcessor() {
}

void BundleProcessor::processBundles() {
}

void BundleProcessor::receiveBundles() {
}

void dispatch(std::shared_ptr<Bundle> bundle,
              std::vector<std::string> destinations) {
}

void forward(std::shared_ptr<Bundle> bundle, std::vector<std::string> nextHop) {
}

void discard(std::shared_ptr<Bundle> bundle) {
}

void restore(std::shared_ptr<Bundle> bundle) {
}
