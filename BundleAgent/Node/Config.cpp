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
 * FILE Config.cpp
 * AUTHOR Blackcatn13
 * DATE Nov 10, 2015
 * VERSION 1
 *
 */

#include "Config.h"
#include <string>
#include "Utils/ConfigLoader.h"

/**
 * Default values definition.
 */
const std::string Config::NODEID = "defaultNode";
const std::string Config::NODEADDRESS = "127.0.0.1";
const int Config::NODEPORT = 4556;
const std::string Config::DISCOVERYADDRESS = "239.100.100.100";
const int Config::DISCOVERYPORT = 40001;
const int Config::DISCOVERYPERIOD = 2;
const int Config::NEIGHBOUREXPIRATIONTIME = 4;
const int Config::NEIGHBOURCLEANERTIME = 2;
const std::string Config::LOGFILENAME = "/tmp/adtn.log";
const int Config::LOGLEVEL = 1;

Config::Config()
    : m_nodeId(NODEID),
      m_nodeAddress(NODEADDRESS),
      m_nodePort(NODEPORT),
      m_discoveryAddress(DISCOVERYADDRESS),
      m_discoveryPort(DISCOVERYPORT),
      m_discoveryPeriod(DISCOVERYPERIOD),
      m_neighbourExpirationTime(NEIGHBOUREXPIRATIONTIME),
      m_neighbourCleanerTime(NEIGHBOURCLEANERTIME),
      m_logFileName(LOGFILENAME),
      m_logLevel(LOGLEVEL) {
}

Config::Config(const std::string &configFilename) {
  bool m_configError = m_configLoader.load(configFilename);
  if (m_configError) {
    m_nodeId = m_configLoader.m_reader.Get("Node", "nodeId", NODEID);
    m_nodeAddress = m_configLoader.m_reader.Get("Node", "nodeAddress",
                                                NODEADDRESS);
    m_nodePort = m_configLoader.m_reader.GetInteger("Node", "nodePort",
                                                    NODEPORT);
    m_discoveryAddress = m_configLoader.m_reader.Get("NeighbourDiscovery",
                                                     "discoveryAddress",
                                                     DISCOVERYADDRESS);
    m_discoveryPort = m_configLoader.m_reader.GetInteger("NeighbourDiscovery",
                                                         "discoveryPort",
                                                         DISCOVERYPORT);
    m_discoveryPeriod = m_configLoader.m_reader.GetInteger("NeighbourDiscovery",
                                                           "discoveryPeriod",
                                                           DISCOVERYPERIOD);
    m_neighbourExpirationTime = m_configLoader.m_reader.GetInteger(
        "NeighbourDiscovery", "neighbourExpirationTime",
        NEIGHBOUREXPIRATIONTIME);
    m_neighbourCleanerTime = m_configLoader.m_reader.GetInteger(
        "NeighbourDiscovery", "neighbourCleanerTime", NEIGHBOURCLEANERTIME);
    m_logFileName = m_configLoader.m_reader.Get("Logger", "filename",
                                                LOGFILENAME);
    m_logLevel = m_configLoader.m_reader.GetInteger("Logger", "level",
                                                    LOGLEVEL);
  }
}

Config::~Config() {
}

std::string Config::getNodeId() {
  return m_nodeId;
}

std::string Config::getNodeAddress() {
  return m_nodeAddress;
}

int Config::getNodePort() {
  return m_nodePort;
}

std::string Config::getDiscoveryAddress() {
  return m_discoveryAddress;
}

int Config::getDiscoveryPort() {
  return m_discoveryPort;
}

int Config::Config::getDiscoveryPeriod() {
  return m_discoveryPeriod;
}

int Config::getNeighbourExpirationTime() {
  return m_neighbourExpirationTime;
}

int Config::getNeighbourCleanerTime() {
  return m_neighbourCleanerTime;
}

std::string Config::getLogFileName() {
  return m_logFileName;
}

int Config::getLogLevel() {
  return m_logLevel;
}
