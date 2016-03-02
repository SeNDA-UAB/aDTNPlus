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
 * FILE RouteReportingMEB.cpp
 * AUTHOR clacambra
 * DATE 24/02/2016
 * VERSION 1
 * This file contains the RouteReportingMEB class.
 */
#include <string>
#include <ctime>
#include <sstream>
#include <iostream>
#include "RouteReportingMEB.h"
#include "Bundle/MetadataExtensionBlock.h"
#include "Bundle/BundleTypes.h"

RouteReportingMEB::RouteReportingMEB(std::string nodeId,
                   std::time_t arrivalTime, std::time_t departureTime)
: MetadataExtensionBlock(
    static_cast<uint8_t>(MetadataTypes::ROUTE_REPORTING_MEB),
    nodeId + "," + std::to_string(arrivalTime) + "," +
    std::to_string(departureTime)) {
  m_routeReporting = m_metadata;
}

RouteReportingMEB::RouteReportingMEB(const std::string &rawData)
  : MetadataExtensionBlock(rawData) {
      m_routeReporting = m_metadata;
}

RouteReportingMEB::RouteReportingMEB()
: MetadataExtensionBlock() {
  m_metadataType = static_cast<uint8_t>(MetadataTypes::ROUTE_REPORTING_MEB);
  m_routeReporting = m_metadata;
}

RouteReportingMEB::~RouteReportingMEB() {
}

std::string RouteReportingMEB::getRouteReporting() {
  return m_metadata;
}

void RouteReportingMEB::addRouteInformation(std::string nodeId,
                     std::time_t arrivalTime, std::time_t departureTime) {
  std::string routeInformation = nodeId + "," + std::to_string(arrivalTime) +
      "," + std::to_string(departureTime);
  if (m_metadata == "") {
    m_metadata = routeInformation;
  } else {
    m_metadata = m_metadata + "\n" + routeInformation;
  }
}

std::string RouteReportingMEB::toString() {
  std::stringstream ss;
  ss << m_metadata;
  return ss.str();
}

