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
 * FILE NumericMEB.cpp
 * AUTHOR MCarmen
 * DATE Dec 20, 2017
 * VERSION 1
 *
 */

#include <Bundle/NumericMEB.h>
#include "Utils/SDNV.h"
#include <sstream>


NumericMEB::NumericMEB(uint8_t numberOfFields, NumericMEB::Code *codes, uint64_t *values)
    : MetadataExtensionBlock(),
      m_nrofFields(numberOfFields) {

  m_ss_rawData << SDNV::encode(m_nrofFields);
  m_metadataType = static_cast<uint8_t>(getMetadataType());
  for(int i=0; i < m_nrofFields; i++){
    addField(codes[i], values[i]);
  }
  m_metadata = m_ss_rawData.str();

}

NumericMEB::NumericMEB(const std::string& rawData)
    : MetadataExtensionBlock(rawData) {
}

NumericMEB::~NumericMEB() {
  // TODO Auto-generated destructor stub
}

void NumericMEB::addField(Code code, uint64_t value) {
  m_ss_rawData << SDNV::encode(static_cast<uint8_t>(code));
  m_ss_rawData << SDNV::encode(value);
}



