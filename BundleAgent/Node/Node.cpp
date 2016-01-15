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
 * FILE Node.cpp
 * AUTHOR Blackcatn13
 * DATE Jun 29, 2015
 * VERSION 1
 *
 */

#include <string>
#include "Node/Node.h"
#include "Node/Neighbour/NeighbourTable.h"
#include "Node/BundleProcessor/BasicBundleProcessor.h"
#include "Utils/Logger.h"
#include "Utils/globals.h"

Node::Node(std::string filename) {
  m_config = Config(filename);
  Logger::getInstance()->setLoggerConfigAndStart(m_config.getLogFileName());
  Logger::getInstance()->setLogLevel(m_config.getLogLevel());
  LOG(6) << "Starting Node...";
  LOG(6) << "Starting NeighbourDiscovery";
  m_neighbourTable = std::unique_ptr<NeighbourTable>(new NeighbourTable());
  m_neighbourDiscovery = std::shared_ptr<NeighbourDiscovery>(
      new NeighbourDiscovery(m_config, m_neighbourTable));
  m_bundleQueue = std::shared_ptr<BundleQueue>(new BundleQueue());
  m_bundleProcessor = std::shared_ptr<BundleProcessor>(
      new BasicBundleProcessor(m_config, m_bundleQueue, m_neighbourTable));
}

Node::~Node() {
  LOG(6) << "Closing Node...";
  delete Logger::getInstance();
}

