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
#include <dlfcn.h>
#include <string>
#include <vector>
#include <sstream>
#include <cstdio>
#include <fstream>
#include <memory>
#include <algorithm>
#include "Node/Node.h"
#include "Node/Neighbour/NeighbourTable.h"
#include "Node/BundleProcessor/PluginAPI.h"
#include "Node/BundleProcessor/BundleProcessor.h"
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
  m_listeningAppsTable = std::shared_ptr<ListeningEndpointsTable>(
      new ListeningEndpointsTable());
  m_appListener = std::shared_ptr<EndpointListener>(
      new EndpointListener(m_config, m_listeningAppsTable));

  m_handle = dlopen(m_config.getBundleProcessorName().c_str(), RTLD_LAZY);
  if (!m_handle) {
    LOG(1) << "Error loading plugin " << m_config.getBundleProcessorName()
           << " reason: " << dlerror();
    g_stop = true;
  } else {
    dlerror();
    PluginInfo* info = reinterpret_cast<PluginInfo*>(dlsym(m_handle,
                                                           "information"));
    const char * error = dlerror();
    if (error != NULL) {
      LOG(1) << "Error getting object from plugin, reason: " << error;
      g_stop = true;
    } else {
      reinterpret_cast<BundleProcessor*>(info->getPlugin())->start(
          m_config, m_bundleQueue, m_neighbourTable, m_listeningAppsTable);
      // Try to restore the bundles.
      std::vector<std::string> bundles = getBundlesInFolder(
          m_config.getDataPath());
      if (m_config.getClean()) {
        // Delete all the bundles in the folder.
        std::for_each(bundles.begin(), bundles.end(),
                      [this](std::string &bundle) {
                        int val = std::remove(bundle.c_str());
                        if (val != 0) {
                          LOG(1) << "The bundle: " << bundle
                          << " cannot be deleted, reason: " << strerror(errno);
                        }
                      });
      } else {
        // Restore all the bundles in the folder.
        std::for_each(
            bundles.begin(),
            bundles.end(),
            [this, info](std::string &bundle) {
              std::ifstream bundleF(bundle);
              std::string
              rawBundleContainer((std::istreambuf_iterator<char>(bundleF))
                  , std::istreambuf_iterator<char>());
              reinterpret_cast<BundleProcessor*>(
                  info->getPlugin())->restoreRawBundleContainer(
                      rawBundleContainer);
              bundleF.close();
            });
      }
    }
  }
}

Node::~Node() {
  LOG(6) << "Closing Node...";
  if (m_handle) {
    dlclose(m_handle);
  }
  delete Logger::getInstance();
}

