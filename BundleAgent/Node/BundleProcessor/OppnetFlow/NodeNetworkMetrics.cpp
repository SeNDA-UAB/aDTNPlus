/*
 * Copyright (c) 2018 SeNDA
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
 * FILE NodeNetworkMetrics.cpp
 * AUTHOR MCarmen
 * DATE Jan 11, 2018
 * VERSION 1
 *
 */

#include "Node/BundleProcessor/OppnetFlow/NodeNetworkMetrics.h"
#include <sstream>
#include <utility>

NodeNetworkMetrics::NodeNetworkMetrics() {
  reset();
}

NodeNetworkMetrics::~NodeNetworkMetrics() {
}

void NodeNetworkMetrics::reset() {
  for (const auto& entry : m_metrics) {
    entry.second = -1;
  }
}

void NodeNetworkMetrics::setField(NetworkMetricsControlCode key,
                                  value_t value) {
  m_metrics[key] = value;
}

value_t& NodeNetworkMetrics::getFieldAt(
    NetworkMetricsControlCode key) {
  return m_metrics.at(key);
}

const uint8_t NodeNetworkMetrics::getNumberOfSpecifiedValues() const {
  uint8_t specifiedMetrics = 0;
  for (const auto& entry : m_metrics) {
    if (entry.second >= 0){
      specifiedMetrics++;
    }
  }

  return specifiedMetrics;
}

const std::map<NetworkMetricsControlCode, value_t>& NodeNetworkMetrics::getMetrics() {
  return m_metrics;
}

std::string NodeNetworkMetrics::toString() {
  std::stringstream ss;

  for (const std::pair<NetworkMetricsControlCode, value_t>& entry : m_metrics) {
    ss << entry.first << " " << std::endl;
  }

  return ss.str();
}

uint64_t NodeNetworkMetrics::incrementField(const NetworkMetricsControlCode key) {
  m_metrics[key]++;

  return m_metrics[key];
}
