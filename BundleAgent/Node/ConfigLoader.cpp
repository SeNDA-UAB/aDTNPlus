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
 * FILE ConfigLoader.cpp
 * AUTHOR Blackcatn13
 * DATE Jun 29, 2015
 * VERSION 1
 * This file contains the implementation of the ConfigLoader class.
 */

#include "ConfigLoader.h"
#include <string>
#include <sstream>
#include "Utils/Logger.h"

/**
 * Default values definition.
 */
const std::string NODEID = "defaultNode";
const std::string NODEADDRESS = "127.0.0.1";
const int NODEPORT = 4556;
const std::string DISCOVERYADDRESS = "239.100.100.100";
const int DISCOVERYPORT = 40001;
const int DISCOVERYPERIOD = 2;
const int NEIGHBOUREXPIRATIONTIME = 4;
const int NEIGHBOURCLEANERTIME = 2;
const std::string FILENAME = "/tmp/adtn.log";
const int LEVEL = 1;

ConfigLoader::ConfigLoader() {
}

ConfigLoader::~ConfigLoader() {
}

bool ConfigLoader::load(std::string file) {
  LOG(18) << "Loading configuration file " << file;
  m_reader.ParseINI(file);
  bool parsed = (m_reader.ParseError() == 0);
  if (parsed) {

  }
  return parsed;
}

std::string ConfigLoader::getNodeId() {
  return m_nodeId;
}

std::string ConfigLoader::getNodeAddress() {
  return m_nodeAddress;
}

int ConfigLoader::getNodePort() {
  return m_nodePort;
}

std::string ConfigLoader::getDiscoveryAddress() {
  return m_discoveryAddress;
}

int ConfigLoader::getDiscoveryPort() {
  return m_discoveryPort;
}

int ConfigLoader::ConfigLoader::getDiscoveryPeriod() {
  return m_discoveryPeriod;
}

int ConfigLoader::getNeighbourExpirationTime() {
  return m_neighbourExpirationTime;
}

int ConfigLoader::getNeighbourCleanerTime() {
  return m_neighbourCleanerTime;
}

std::string ConfigLoader::getFilename() {
  return m_filename;
}

int ConfigLoader::getLevel() {
  return m_level;
}

