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

#include "Bundle/Block.h"
#include "Bundle/NumericMEB.h"
#include "Utils/SDNV.h"
#include "Utils/Logger.h"
#include <stdexcept>
#include <utility>


NumericMEB::NumericMEB(const MetadataTypes mebType, const uint8_t numberOfFields, const std::map<uint8_t, value_t> fields)
    : MetadataExtensionBlock(),
      m_metadataType(static_cast<uint8_t>(mebType)),
      m_nrofFields(numberOfFields),
      m_fields(fields){}

NumericMEB::NumericMEB(const std::string& rawData)
    : MetadataExtensionBlock(rawData) {
  try {
    initFromRaw(rawData);
  } catch (const std::out_of_range& e) {
    throw BlockConstructionException("[Framework MEB] Bad raw format");
  }
}

NumericMEB::~NumericMEB() {
  // TODO Auto-generated destructor stub
}

void resetFields(){
  for(const auto& entry : m_fields){
    m_fields[entry.first] = -1;
  }
}

void initFromRaw(const std::string &rawData){
  MetadataExtensionBlock::initFromRaw(rawData);
  std::string metadata = m_metadata;
  uint8_t code;
  value_t value;
  resetFields();
  try {
    m_nrofFields = SDNV::decode(metadata);
    metadata = metadata.substr(SDNV::getLength(metadata));
    for(int i=0; i<m_nrofFields; i++){
      code = SDNV::decode(metadata);
      metadata = metadata.substr(SDNV::getLength(metadata));
      value = SDNV::decode(metadata);
      metadata = metadata.substr(SDNV::getLength(metadata));
      m_fields[code] = value;
    }
  } catch (const std::out_of_range &e) {
    throw BlockConstructionException("[FrameworkMEB] Bad raw format");
  }
}

void NumericMEB::addField(uint8_t code, uint64_t value, std::stringstream& ss){
  ss << SDNV::encode(code);
  ss << SDNV::encode(value);
}

const value_t NumericMEB::getField(const uint8_t code) const {
  return m_fields.at(code);
}

std::string NumericMEB::toRaw(){
  LOG(87) << "Generating raw data from NumericMEB MEB";
  std::stringstream ss;
  ss << SDNV::encode(m_nrofFields);
  for(const auto& entry : m_fields){
    if (entry.second != -1){
      addField(entry.first, entry.second, ss);
    }
  }
  m_metadata = ss.str();
  //setting the headers to the metadata.
  return MetadataExtensionBlock::toRaw();
}

std::string NumericMEB::toString() {
  std::stringstream ss;
  ss << "NumericMEB block:" << std::endl << MetadataExtensionBlock::toString()
     << "Number Of Fields: " << m_nrofFields << std::endl;
  for (const auto& entry : m_fields) {
    if (entry.second != -1) {
      ss << "Code: " << entry.first << " Value: "
         << entry.second << std::endl;
    }
  }
  return ss.str();
}
