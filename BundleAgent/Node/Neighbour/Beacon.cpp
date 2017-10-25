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
 * FILE Beacon.cpp
 * AUTHOR Blackcatn13
 * DATE Jun 29, 2015
 * VERSION 1
 * This file contains the implementation of the Beacon class.
 */

#include "Node/Neighbour/Beacon.h"
#include <string>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include "Utils/Logger.h"

Beacon::Beacon(std::string rawData) {
  LOG(90) << "Generating beacon from raw Data";
  char buff[2048];
  const char* dataChar = rawData.c_str();
  strcpy(&buff[0], dataChar);
  m_nodeId = std::string(buff);
  int size = strlen(buff) + 1;
  strcpy(&buff[0], dataChar + size * sizeof(char));
  m_nodeAddress = std::string(buff);
  size += strlen(buff) + 1;
  strcpy(&buff[0], dataChar + size * sizeof(char));
  m_nodePort = static_cast<uint16_t>(atoi(buff));
  size += strlen(buff) + 1;
  strcpy(&buff[0], dataChar + size * sizeof(char));
  int endpoints = static_cast<size_t>(atoi(buff));
  size += strlen(buff) + 1;
  dataChar += size;
  for (int i = 0; i < endpoints; ++i) {
    m_endpoints.push_back(dataChar);
    dataChar += m_endpoints.back().size() + 1;
  }
  m_raw = rawData;
}

Beacon::Beacon(const std::string &nodeId, const std::string &nodeAddress,
               const uint16_t &nodePort, std::vector<std::string> endpoints)
    : m_nodeId(nodeId),
      m_nodeAddress(nodeAddress),
      m_nodePort(nodePort),
      m_endpoints(endpoints) {
  LOG(90) << "Generating beacon from parameters [nodeId: " << nodeId
          << "][nodeAddress: " << nodeAddress << "][nodePort: " << nodePort
          << "]";
  std::stringstream ss;
  ss << m_nodeId.c_str();
  ss << std::ends;
  ss << m_nodeAddress.c_str();
  ss << std::ends;
  ss << m_nodePort;
  ss << std::ends;
  ss << m_endpoints.size() << std::ends;
  for (auto s : m_endpoints) {
    ss << s << std::ends;
  }
  m_raw = ss.str();
}

Beacon::~Beacon() {
}

std::string Beacon::getRaw() {
  return m_raw;
}

std::string Beacon::getNodeId() const {
  return m_nodeId;
}

std::string Beacon::getNodeAddress() const {
  return m_nodeAddress;
}

uint16_t Beacon::getNodePort() const {
  return m_nodePort;
}

std::vector<std::string> Beacon::getEndpoints() const {
  return m_endpoints;
}

