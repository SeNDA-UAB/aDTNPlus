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
 * FILE ControlDirectiveMEB.cpp
 * AUTHOR MCarmen
 * DATE Dec 13, 2017
 * VERSION 1
 *
 */

#include "Bundle/NumericMEB.h"
#include "Bundle/Block.h"
#include "Bundle/BundleTypes.h"
#include "ControlDirectiveMEB.h"
#include "Utils/SDNV.h"
#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <string>


ControlDirectiveMEB::ControlDirectiveMEB(uint8_t numberOfFields, Code *codes,
                                         uint64_t *values)
    : NumericMEB(MetadataTypes::CONTROL_DIRECTIVE_MEB, numberOfFields, codes, values) {

}

ControlDirectiveMEB::ControlDirectiveMEB(const std::string& rawData)
    : NumericMEB(rawData) {
  std::string metadata = m_metadata;
  try {
    m_nrofFields = SDNV::decode(metadata);
    metadata = metadata.substr(SDNV::getLength(metadata));
    for (int i = 0; i < m_nrofFields; i++) {
      Code code = static_cast<Code>(SDNV::decode(metadata));
      metadata = metadata.substr(SDNV::getLength(metadata));
      switch (code) {
        case NumericMEB::Code::NROFCOPIES:
          m_nrofCopies = SDNV::decode(metadata);
          break;
        case NumericMEB::Code::CTRL_REPORT_FREQUENCY:
          m_nrofCopies = SDNV::decode(metadata);
          break;
        default:
          throw std::invalid_argument("Code");
      }
      metadata = metadata.substr(SDNV::getLength(metadata));
    }
  } catch (const std::invalid_argument& e) {
    throw BlockConstructionException("[OppNetMetricsMEB decode from raw error");
  }
}

ControlDirectiveMEB::~ControlDirectiveMEB() {
  // TODO Auto-generated destructor stub
}

std::string ControlDirectiveMEB::toString() {
  std::stringstream ss;
  ss << "Control Directive block:" << std::endl
      << MetadataExtensionBlock::toString() << "\tNumber of copies: "
      << static_cast<int>(m_nrofCopies) << std::endl
      << "\tReport frequency time: "  << m_ctl_reportFrequency << std::endl;
  return ss.str();
}

MetadataTypes ControlDirectiveMEB::getMetadataType() {
  return MetadataTypes::CONTROL_DIRECTIVE_MEB;
}
