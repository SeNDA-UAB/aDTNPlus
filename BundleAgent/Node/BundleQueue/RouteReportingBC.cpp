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
 * FILE RouteReportingBC.cpp
 * AUTHOR clacambra
 * DATE 26/02/2016
 * VERSION 1
 * This file contains the implementation of the
 * Route Reporting Bundle Container.
 */

#include <string>
#include <ctime>
#include <sstream>
#include <memory>
#include <cstdlib>
#include <cstring>
#include <exception>

#include "RouteReportingBC.h"
#include "Node/BundleQueue/BundleContainer.h"
#include "Bundle/Bundle.h"


RouteReportingBC::RouteReportingBC(std::string nodeId, time_t arrivalTime,
                                   time_t departureTime,
                                   std::unique_ptr<Bundle> bundle)
: BundleContainer(nodeId, std::move(bundle)) {
  m_nodeId = nodeId;
  m_arrivalTime = arrivalTime;
  m_departureTime = departureTime;
}

RouteReportingBC::RouteReportingBC(const std::string &data)
:BundleContainer() {
  deserialize(data);
}

RouteReportingBC::~RouteReportingBC() {
}

std::string RouteReportingBC::getNodeId() {
  return m_nodeId;
}

time_t RouteReportingBC::getArrivalTime() {
  return m_arrivalTime;
}

time_t RouteReportingBC::getDepartureTime() {
  return m_departureTime;
}

std::string RouteReportingBC::serialize() {
  std::stringstream ss;
  ss << m_header << m_from << std::ends << m_bundle->toRaw()
     << std::endl << std::to_string(m_arrivalTime)
     << std::endl << std::to_string(m_departureTime) << m_footer;
  return ss.str();
}

void RouteReportingBC::deserialize(const std::string &data) {
  // BundleContainer::deserialize(data);
  // Check header
  std::stringstream size;
  // Get the header size in chars
  size << m_header;
  int headerSize = size.str().length();
  uint16_t header = std::atoi(data.substr(0, size.str().length()).c_str());
  if (header == m_header) {
    std::string newData = data.substr(headerSize);
    size.clear();
    size.str(std::string());
    // Get the neighbour id
    char buff[1024];
    snprintf(&buff[0], sizeof(buff), "%s", newData.c_str());
    int length = strlen(buff);
    m_from = std::string(buff);
    // Remove node id and \0
    newData = newData.substr(length + 1);
    size_t position = newData.find("\n");
    size << m_footer;
    int footerSize = size.str().length();
    // The raw bundle is from the start to the size - footer size
    // std::string bundleData = newData.substr(
    // 0, newData.length() - (footerSize));
    std::string bundleData = newData.substr(0, position);
    std::unique_ptr<Bundle> b;
    try {
      m_bundle = std::unique_ptr<Bundle>(new Bundle(bundleData));
    } catch (const std::exception &e) {
      throw BundleContainerCreationException(
          "[BundleContainer] Bad bundle raw format");
    }
    m_nodeId = m_from;
    // int pos = newData.find("\n");
    std::string subData = newData.substr(position + 1, data.size());
    position = subData.find("\n");
    std::string arrivalTime = subData.substr(0, position);
    m_arrivalTime = atoi(arrivalTime.c_str());
    subData = subData.substr(position + 1, subData.size());
    std::string departureTime = subData.substr(
        0, subData.size() - (footerSize));
    m_departureTime = atoi(departureTime.c_str());
    uint16_t footer = std::atoi(
        newData.substr(newData.length() - (footerSize)).c_str());
    if (footer != m_footer) {
      throw BundleContainerCreationException(
          "[BundleContainer] Bad footer in bundle container");
    }
  } else {
    throw BundleContainerCreationException(
        "[BundleContainer] Bad header in bundle container");
  }
}

std::string RouteReportingBC::toString() {
  std::stringstream ss;
  ss << "From: " << m_from << std::endl << "Bundle: " << std::endl
     << m_bundle->toString() << std::endl << "Arrival time: "
     << std::asctime(std::localtime(&m_arrivalTime)) << std::endl
     << "Departure time: " << std::asctime(std::localtime(&m_departureTime));
  return ss.str();
}

void RouteReportingBC::setDepartureTime(time_t departureTime) {
  m_departureTime = departureTime;
}

