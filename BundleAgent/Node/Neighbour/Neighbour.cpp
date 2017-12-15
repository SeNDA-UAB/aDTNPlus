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
 * FILE Neighbour.cpp
 * AUTHOR Blackcatn13
 * DATE Jun 29, 2015
 * VERSION 1
 * This file contains the implementation of the Neighbour class.
 */

#include "Node/Neighbour/Neighbour.h"
#include <string>
#include <cstdint>
#include <vector>
#include <chrono>
#include "Utils/Logger.h"

Neighbour::Neighbour(const std::string &nodeId, const std::string &nodeAddress,
                     const uint16_t &nodePort,
                     std::vector<std::string> endpoints)
    : m_nodeId(nodeId),
      m_nodeAddress(nodeAddress),
      m_nodePort(nodePort),
      m_endpoints(endpoints),
      m_lastActivity(std::chrono::steady_clock::now()) {
  LOG(69) << "Creating new neighbour from parameters [nodeId: " << nodeId

  << "][nodeAddress: "
          << nodeAddress << "][nodePort: " << nodePort << "]";
}

Neighbour::~Neighbour() {
}

int Neighbour::getElapsedActivityTime() {
  LOG(69) << "Getting last activity time";
  std::chrono::nanoseconds now = std::chrono::steady_clock::now()
      - m_lastActivity;
  return now.count() * std::chrono::nanoseconds::period::num
      / std::chrono::nanoseconds::period::den;
}

void Neighbour::update(std::shared_ptr<Neighbour> neighbour) {
  LOG(69) << "Updating neighbour last activity time to now";
  m_nodeAddress = neighbour->getNodeAddress();
  m_nodePort = neighbour->getNodePort();
  m_endpoints = neighbour->getEndpoints();
  m_lastActivity = std::chrono::steady_clock::now();
}

bool Neighbour::operator ==(const Neighbour &neighbour) const {
  bool equals = m_nodeId == neighbour.m_nodeId;
  equals &= m_nodeAddress == neighbour.m_nodeAddress;
  equals &= m_nodePort == neighbour.m_nodePort;
  equals &= m_endpoints == neighbour.m_endpoints;
  return equals;
}

std::string Neighbour::getId() {
  return m_nodeId;
}

std::string Neighbour::getNodeAddress() {
  return m_nodeAddress;
}

uint16_t Neighbour::getNodePort() {
  return m_nodePort;
}

std::vector<std::string> Neighbour::getEndpoints() {
  return m_endpoints;
}
