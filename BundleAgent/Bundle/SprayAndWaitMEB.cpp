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
 * FILE SprayAndWaitMEB.cpp
 * AUTHOR MCarmen
 * DATE Nov 22, 2017
 * VERSION 1
 *
 */

#include "Bundle/SprayAndWaitMEB.h"
#include <cstdint>
#include <string>
#include <sstream>

SprayAndWaitMEB::SprayAndWaitMEB(const uint16_t nrofCopies)
    : MetadataExtensionBlock(static_cast<uint8_t>(MetadataTypes::SPRAYANDWAIT_MEB),
    std::to_string(nrofCopies)), m_nrofCopies(nrofCopies) {}

SprayAndWaitMEB::SprayAndWaitMEB(const std::string& rawData)
    : MetadataExtensionBlock(rawData) {
  try{
    m_nrofCopies = static_cast<uint16_t>(std::stoi(m_metadata));
  }catch (const std::invalid_argument& e){
    throw BlockConstructionException("[SprayAndWait] bad number of copies");
  }
}

SprayAndWaitMEB::~SprayAndWaitMEB() {
}

std::string SprayAndWaitMEB::toString() {
  std::stringstream ss;
    ss << "Spray and Wait block:" << std::endl
       << MetadataExtensionBlock::toString() << "\tNumber of copies: "
       << static_cast<int>(m_nrofCopies) << std::endl;
    return ss.str();
}

uint16_t SprayAndWaitMEB::getNrofCopies() const {
  return m_nrofCopies;
}

void SprayAndWaitMEB::setNrofCopies(uint16_t _nrofCopies) {
  m_nrofCopies = _nrofCopies;
  m_metadata = std::to_string(m_nrofCopies);
}

