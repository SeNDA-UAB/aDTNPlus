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

#include "Node/Config.h"
#include <string>
#include <sstream>
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
const std::string Config::LOGFILENAME = "/tmp/adtn/adtn.log";
const int Config::LOGLEVEL = 1;
const int Config::SOCKETTIMEOUT = 20;
const bool Config::NEIGHBOURTESTMODE = false;
const std::string Config::DATAPATH = "/tmp/adtn/bundles/";
const std::string Config::LISTENERADDRESS = "127.0.0.1";
const int Config::LISTENERPORT = 50000;
const bool Config::CLEAN = false;
const std::string Config::BUNDLEPROCESSORNAME =
    "libaDTNPlus_BasicBundleProcessor.so";
const std::string Config::NODESTATEPATH = "BundleAgent/NodeState.json";
const std::string Config::CODESPATH = "./";
const std::string Config::DELIVEREDPATH = "/tmp/adtn/delivered/";
const std::string Config::TRASHDELIVERYPATH =
    "/tmp/adtn/trash/aggregation/delivery/";
const std::string Config::TRASHRECEPTIONPATH =
    "/tmp/adtn/trash/aggregation/reception";
const std::string Config::TRASHDROPPATH = "/tmp/adtn/trash/drop";
const std::string Config::QUEUEBYTESIZE = "100M";
const uint64_t Config::QUEUEBYTESIZEVALUE = 100 * 1024 * 1024;
const int Config::PROCESSTIMEOUT = 20;

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
      m_logLevel(LOGLEVEL),
      m_socketTimeout(SOCKETTIMEOUT),
      m_neighbourTestMode(NEIGHBOURTESTMODE),
      m_dataPath(DATAPATH),
      m_listenerAddress(LISTENERADDRESS),
      m_listenerPort(LISTENERPORT),
      m_clean(CLEAN),
      m_bundleProcessorName(BUNDLEPROCESSORNAME),
      m_nodeStatePath(NODESTATEPATH),
      m_codesPath(CODESPATH),
      m_deliveredPath(DELIVEREDPATH),
      m_trashDeliveryPath(TRASHDELIVERYPATH),
      m_trashReceptionPath(TRASHRECEPTIONPATH),
      m_trashDropPath(TRASHDROPPATH),
      m_queueByteSize(QUEUEBYTESIZEVALUE),
      m_processTimeout(PROCESSTIMEOUT) {
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
    m_socketTimeout = m_configLoader.m_reader.GetInteger("Constants", "timeout",
                                                         SOCKETTIMEOUT);
    m_neighbourTestMode = m_configLoader.m_reader.GetBoolean(
        "NeighbourDiscovery", "testMode", NEIGHBOURTESTMODE);
    m_dataPath = m_configLoader.m_reader.Get("BundleProcess", "dataPath",
                                             DATAPATH);
    m_listenerAddress = m_configLoader.m_reader.Get("AppListener",
                                                    "listenerAddress",
                                                    LISTENERADDRESS);
    m_listenerPort = m_configLoader.m_reader.GetInteger("AppListener",
                                                        "listenerPort",
                                                        LISTENERPORT);
    m_clean = m_configLoader.m_reader.GetBoolean("Node", "clean", CLEAN);
    m_bundleProcessorName = m_configLoader.m_reader.Get("BundleProcess",
                                                        "bundleProcessName",
                                                        BUNDLEPROCESSORNAME);
    m_nodeStatePath = m_configLoader.m_reader.Get("NodeState", "path",
                                                  NODESTATEPATH);
    m_codesPath = m_configLoader.m_reader.Get("BundleProcess", "codePath",
                                              CODESPATH);
    m_deliveredPath = m_configLoader.m_reader.Get("BundleProcess",
                                                  "deliveryPath",
                                                  DELIVEREDPATH);
    m_trashDeliveryPath = m_configLoader.m_reader.Get(
        "BundleProcess", "trashAggregationDelivery", TRASHDELIVERYPATH);
    m_trashReceptionPath = m_configLoader.m_reader.Get(
        "BundleProcess", "trashAggregationReception", TRASHRECEPTIONPATH);
    m_trashDropPath = m_configLoader.m_reader.Get("BundleProcess", "trashDropp",
                                                  TRASHDROPPATH);
    std::string queueByteSize = m_configLoader.m_reader.Get("Constants",
                                                            "queueByteSize",
                                                            QUEUEBYTESIZE);
    std::stringstream ss(queueByteSize);
    char exponent;
    ss >> m_queueByteSize >> exponent;
    if (exponent == 'K' || exponent == 'k')
      m_queueByteSize *= 1024;
    else if (exponent == 'M' || exponent == 'm')
      m_queueByteSize = m_queueByteSize * 1024 * 1024;
    else if (exponent == 'G' || exponent == 'g')
      m_queueByteSize = m_queueByteSize * 1024 * 1024 * 1024;
    m_processTimeout = m_configLoader.m_reader.GetInteger("Constants",
                                                   "processTimeout",
                                                   PROCESSTIMEOUT);
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

int Config::getSocketTimeout() {
  return m_socketTimeout;
}

bool Config::getNeighbourTestMode() {
  return m_neighbourTestMode;
}

std::string Config::getDataPath() {
  return m_dataPath;
}

std::string Config::getListenerAddress() {
  return m_listenerAddress;
}

int Config::getListenerPort() {
  return m_listenerPort;
}

bool Config::getClean() {
  return m_clean;
}

std::string Config::getBundleProcessorName() {
  return m_bundleProcessorName;
}

std::string Config::getNodeStatePath() {
  return m_nodeStatePath;
}

std::string Config::getCodesPath() {
  return m_codesPath;
}

std::string Config::getDeliveryPath() {
  return m_deliveredPath;
}

std::string Config::getTrashDelivery() {
  return m_trashDeliveryPath;
}

std::string Config::getTrashReception() {
  return m_trashReceptionPath;
}

std::string Config::getTrashDrop() {
  return m_trashDropPath;
}

uint64_t Config::getQueueByteSize() {
  return m_queueByteSize;
}

int Config::getProcessTimeout() {
  return m_processTimeout;
}
