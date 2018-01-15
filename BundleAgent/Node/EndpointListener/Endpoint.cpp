/*
 * Copyright (c) 2016 SeNDA
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
 * FILE Endpoint.cpp
 * AUTHOR Blackcatn13
 * DATE Jan 19, 2016
 * VERSION 1
 * This file contains the implementation of the Endpoint class.
 */

#include "Node/EndpointListener/Endpoint.h"
#include <string>
#include <cstdint>
#include <chrono>
#include "Utils/Logger.h"
#include "Utils/Socket.h"

Endpoint::Endpoint(const std::string &endpointId, const std::string &endpointAddress,
         const uint16_t &endpointPort, Socket socket)
    : m_id(endpointId),
      m_address(endpointAddress),
      m_port(endpointPort),
      m_socket(socket),
      m_lastActivity(std::chrono::steady_clock::now()) {
  LOG(69) << "Creating new endpoint from parameters [endpointId: " << endpointId
          << "][endpointAddress: " << endpointAddress << "][endpointPort: " << endpointPort << "]";
}

Endpoint::~Endpoint() {
}

int Endpoint::getElapsedActivityTime() {
  LOG(69) << "Getting last activity time";
  std::chrono::nanoseconds now = std::chrono::steady_clock::now()
      - m_lastActivity;
  return now.count() * std::chrono::nanoseconds::period::num
      / std::chrono::nanoseconds::period::den;
}

void Endpoint::update(std::shared_ptr<Endpoint> endpoint) {
  LOG(69) << "Updating endpoint last activity time to now";
  m_address = endpoint->getAddress();
  m_port = endpoint->getPort();
  m_lastActivity = std::chrono::steady_clock::now();
  m_socket = endpoint->getSocket();
}

bool Endpoint::operator ==(const Endpoint &endpoint) const {
  bool equals = m_id == endpoint.m_id;
  equals &= m_address == endpoint.m_address;
  equals &= m_port == endpoint.m_port;
  equals &= m_socket == endpoint.m_socket;
  return equals;
}

std::string Endpoint::getId() {
  return m_id;
}

std::string Endpoint::getAddress() {
  return m_address;
}

uint16_t Endpoint::getPort() {
  return m_port;
}

Socket Endpoint::getSocket() {
  return m_socket;
}

bool Endpoint::checkDeliveredId(const std::string &id) {
  return (m_deliveredIds.find(id) != m_deliveredIds.end());
}

void Endpoint::addDeliveredId(const std::string &id) {
  m_deliveredIds.insert(id);
}
