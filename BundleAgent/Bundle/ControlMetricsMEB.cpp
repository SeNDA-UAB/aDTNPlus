/*
 * Copyright (c) 2017 SeNDA
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
 * FILE ControlMetricsMEB.cpp
 * AUTHOR MCarmen
 * DATE Dec 12, 2017
 * VERSION 1
 *
 */
#include "Bundle/ControlMetricsMEB.h"
#include "Utils/SDNV.h"
#include <cstdint>
#include <string>
#include <sstream>



ControlMetricsMEB::ControlMetricsMEB(const uint32_t nDrops, const uint32_t nDelivered)
    : MetadataExtensionBlock(),
      m_nrOfDrops(nDrops),
      m_nrOfDelivered(nDelivered) {
  std::stringstream ss;

  m_metadataType = static_cast<uint8_t>(MetadataTypes::CONTROL_METRICS_MEB);
  ss << SDNV::encode(m_nrOfDrops);
  ss << SDNV::encode(m_nrOfDelivered);
  m_metadata = ss.str();
}

ControlMetricsMEB::ControlMetricsMEB(const std::string& rawData)
    : MetadataExtensionBlock(rawData) {
  std::string metadata = m_metadata;
  try {
    m_nrOfDrops = SDNV::decode(metadata);
    metadata = metadata.substr(SDNV::getLength(metadata));
    m_nrOfDelivered = SDNV::decode(metadata);
    metadata = metadata.substr(SDNV::getLength(metadata));
  } catch (const std::invalid_argument& e) {
    throw BlockConstructionException(
        "[ControlMetricsMEB] decode from raw error");
  }
}

ControlMetricsMEB::~ControlMetricsMEB() {
  // TODO Auto-generated destructor stub
}

std::string ControlMetricsMEB::toString() {
  std::stringstream ss;
  ss << "Control Metrics block:" << std::endl
     << MetadataExtensionBlock::toString() << "\tNumber of drops: "
     << m_nrOfDrops << std::endl
     << MetadataExtensionBlock::toString() << "\tNumber of delivered: "
     << m_nrOfDelivered << std::endl;
  return ss.str();
}
