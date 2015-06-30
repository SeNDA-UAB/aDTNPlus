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
 * FILE NeighbourDiscovery.cpp
 * AUTHOR Blackcatn13
 * DATE Jun 29, 2015
 * VERSION 1
 *
 */

#include "NeighbourDiscovery.h"
#include <string>
#include <thread>
#include <chrono>
#include "NeighbourTable.h"

NeighbourDiscovery::NeighbourDiscovery(ConfigLoader config)
    : m_stop(false),
      m_discoveryAddress(
          config.m_reader.Get("NeighbourDiscovery", "discoveryAddress", "")),
      m_discoveryPort(
          config.m_reader.GetInteger("NeighbourDiscovery", "discoveryPort",
                                     40005)),
      m_discoveryPeriod(
          config.m_reader.GetInteger("NeighbourDiscovery", "discoveryPeriod",
                                     1)),
      m_neighbourExpirationTime(
          config.m_reader.GetInteger("NeighbourDiscovery",
                                     "neighbourExpirationTime", 2)),
      m_neighbourCleanerTime(
          config.m_reader.GetInteger("NeighbourDiscovery",
                                     "neighbourCleanerTime", 1)),
      m_neighbourCleanerThread(&NeighbourDiscovery::neighbourCleaner,
                               this) {
  m_neighbourCleanerThread.detach();
}

NeighbourDiscovery::~NeighbourDiscovery() {
}

void NeighbourDiscovery::sendBeacons() {
}

void NeighbourDiscovery::receiveBeacons() {
}

void NeighbourDiscovery::neighbourCleaner() {
  while (!m_stop.load()) {
    std::this_thread::sleep_for(std::chrono::seconds(m_neighbourCleanerTime));
    NeighbourTable::getInstance()->cleanNeighbours(m_neighbourExpirationTime);
  }
}

void NeighbourDiscovery::stop() {
  m_stop = true;
}

