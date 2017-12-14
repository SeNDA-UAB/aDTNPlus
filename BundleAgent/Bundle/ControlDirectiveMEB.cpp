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

#include "Bundle/Block.h"
#include "Bundle/BundleTypes.h"
#include "ControlDirectiveMEB.h"
#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <string>


ControlDirectiveMEB::ControlDirectiveMEB(uint16_t nrOfCopies)
    : MetadataExtensionBlock(
          static_cast<uint8_t>(MetadataTypes::CONTROL_DIRECTIVE_MEB),
          std::to_string(nrOfCopies)),
      m_nrofCopies(nrOfCopies) {}

ControlDirectiveMEB::ControlDirectiveMEB(const std::string& rawData)
    : MetadataExtensionBlock(rawData) {
  try {
    m_nrofCopies = static_cast<uint16_t>(std::stoi(m_metadata));
  } catch (const std::invalid_argument& e) {
    throw BlockConstructionException("[ControlDirective] bad number of copies");
  }
}

ControlDirectiveMEB::~ControlDirectiveMEB() {
  // TODO Auto-generated destructor stub
}

std::string ControlDirectiveMEB::toString() {
  std::stringstream ss;
  ss << "Control Directive block:" << std::endl
      << MetadataExtensionBlock::toString() << "\tNumber of copies: "
      << static_cast<int>(m_nrofCopies) << std::endl;
  return ss.str();
}
