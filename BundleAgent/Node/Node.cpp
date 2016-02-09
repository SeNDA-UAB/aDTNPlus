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

#include <dirent.h>
#include <string>
#include <vector>
#include <sstream>
#include <cstdio>
#include <fstream>
#include <memory>
#include <algorithm>
#include "Node/Node.h"
#include "Node/Neighbour/NeighbourTable.h"
#include "Node/BundleProcessor/RoutingSelectionBundleProcessor.h"
#include "Node/BundleQueue/BundleContainer.h"
#include "Utils/Logger.h"
#include "Utils/globals.h"


std::vector<std::string> getBundlesInFolder(std::string folder) {
  DIR *dir = NULL;
  std::vector<std::string> files;
  if ((dir = opendir(folder.c_str())) == NULL) {
    LOG(1) << "Cannot open bundles directory, reason: " << strerror(errno);
  } else {
    struct dirent *ent = NULL;
    while ((ent = readdir(dir)) != NULL) {
      if (ent->d_type != DT_REG)
        continue;
      std::stringstream ss;
      ss << folder << ent->d_name;
      files.push_back(ss.str());
    }
    closedir(dir);
  }
  return files;
}

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
  std::vector<std::string> bundles = getBundlesInFolder(m_config.getDataPath());
  if (m_config.getClean()) {
    // Delete all the bundles in the folder.
    std::for_each(bundles.begin(), bundles.end(), [this](std::string &bundle) {
      int val = std::remove(bundle.c_str());
      if (val != 0) {
        LOG(1) << "The bundle: " << bundle << " cannot be deleted, reason: "
        << strerror(errno);
      }
    });
  } else {
    // Restore all the bundles in the folder.
    std::for_each(bundles.begin(), bundles.end(), [this](std::string &bundle) {
      std::ifstream bundleF(bundle);
      std::string rawBundleContainer((std::istreambuf_iterator<char>(bundleF))
          , std::istreambuf_iterator<char>());
      std::unique_ptr<BundleContainer> bundleContainer =
      BundleContainer::deserialize(rawBundleContainer);
      m_bundleQueue->enqueue(std::move(bundleContainer));
      bundleF.close();
    });
  }
  m_listeningAppsTable = std::shared_ptr<ListeningAppsTable>(
      new ListeningAppsTable());
  m_appListener = std::shared_ptr<AppListener>(
      new AppListener(m_config, m_listeningAppsTable));
  m_bundleProcessor = std::shared_ptr<BundleProcessor>(
      new RoutingSelectionBundleProcessor(m_config, m_bundleQueue,
                                          m_listeningAppsTable,
                                          m_neighbourTable));
}

Node::~Node() {
  LOG(6) << "Closing Node...";
  delete Logger::getInstance();
}

