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
 * FILE RoutingSelectionMEB.cpp
 * AUTHOR clacambra
 * DATE 03/02/2016
 * VERSION 1
 * This file contains the implementation of Routing Selection MEB.
 */

#include "RoutingSelectionMEB.h"
#include <sstream>
#include <string>
#include <exception>
#include "Utils/Logger.h"
#include "Bundle/BundleTypes.h"

RoutingSelectionMEB::RoutingSelectionMEB(const uint8_t selection)
    : MetadataExtensionBlock(
        static_cast<uint8_t>(MetadataTypes::ROUTING_SELECTION_MEB),
        std::to_string(selection)) {
  m_selection = selection;
}

RoutingSelectionMEB::RoutingSelectionMEB(const std::string& rawData)
    : MetadataExtensionBlock(rawData) {
  try {
    m_selection = static_cast<uint8_t>(std::stoi(m_metadata));
  } catch (const std::invalid_argument& e) {
    throw BlockConstructionException(m_metadata);
  }
}

RoutingSelectionMEB::~RoutingSelectionMEB() {
}

uint8_t RoutingSelectionMEB::getSelection() {
  return m_selection;
}

std::string RoutingSelectionMEB::toString() {
  std::stringstream ss;
  ss << "Routing Selection block:" << std::endl
     << MetadataExtensionBlock::toString() << "\tRouting algorithm: "
     << static_cast<int>(m_selection) << std::endl;
  return ss.str();
}
