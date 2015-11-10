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
const std::string ConfigLoader::NODEID = "defaultNode";
const std::string ConfigLoader::NODEADDRESS = "127.0.0.1";
const int ConfigLoader::NODEPORT = 4556;
const std::string ConfigLoader::DISCOVERYADDRESS = "239.100.100.100";
const int ConfigLoader::DISCOVERYPORT = 40001;
const int ConfigLoader::DISCOVERYPERIOD = 2;
const int ConfigLoader::NEIGHBOUREXPIRATIONTIME = 4;
const int ConfigLoader::NEIGHBOURCLEANERTIME = 2;
const std::string ConfigLoader::FILENAME = "/tmp/adtn.log";
const int ConfigLoader::LEVEL = 1;

ConfigLoader::ConfigLoader()
    : m_nodeId(NODEID),
      m_nodeAddress(NODEADDRESS),
      m_nodePort(NODEPORT),
      m_discoveryAddress(DISCOVERYADDRESS),
      m_discoveryPort(DISCOVERYPORT),
      m_discoveryPeriod(DISCOVERYPERIOD),
      m_neighbourExpirationTime(NEIGHBOUREXPIRATIONTIME),
      m_neighbourCleanerTime(NEIGHBOURCLEANERTIME),
      m_filename(FILENAME),
      m_level(LEVEL) {
}

ConfigLoader::~ConfigLoader() {
}

bool ConfigLoader::load(std::string file) {
  LOG(18) << "Loading configuration file " << file;
  m_reader.ParseINI(file);
  bool parsed = (m_reader.ParseError() == 0);
  if (parsed) {
    m_nodeId = m_reader.Get("Node", "nodeId", NODEID);
    m_nodeAddress = m_reader.Get("Node", "nodeAddress", NODEADDRESS);
    m_nodePort = m_reader.GetInteger("Node", "nodePort", NODEPORT);
    m_discoveryAddress = m_reader.Get("NeighbourDiscovery", "discoveryAddress",
                                      DISCOVERYADDRESS);
    m_discoveryPort = m_reader.GetInteger("NeighbourDiscovery", "discoveryPort",
                                          DISCOVERYPORT);
    m_discoveryPeriod = m_reader.GetInteger("NeighbourDiscovery",
                                            "discoveryPeriod", DISCOVERYPERIOD);
    m_neighbourExpirationTime = m_reader.GetInteger("NeighbourDiscovery",
                                                    "neighbourExpirationTime",
                                                    NEIGHBOUREXPIRATIONTIME);
    m_neighbourCleanerTime = m_reader.GetInteger("NeighbourDiscovery",
                                                 "neighbourCleanerTime",
                                                 NEIGHBOURCLEANERTIME);
    m_filename = m_reader.Get("Logger", "filename", FILENAME);
    m_level = m_reader.GetInteger("Logger", "level", LEVEL);
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

