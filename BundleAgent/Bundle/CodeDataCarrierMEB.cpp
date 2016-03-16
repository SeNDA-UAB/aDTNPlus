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
 * FILE NewMEB.cpp
 * AUTHOR clacambra
 * DATE 11/03/2016
 * VERSION 1
 * This file contains the implementation of the CodeDataCarrierMEB class.
 */

#include <string>
#include <sstream>
#include <iostream>
#include <vector>

#include "Bundle/CodeDataCarrierMEB.h"
#include "Bundle/BundleTypes.h"
#include "Bundle/MetadataExtensionBlock.h"
#include "Bundle/CanonicalBlock.h"
#include "Utils/Logger.h"
#include "Utils/SDNV.h"

CodeDataCarrierMEB::CodeDataCarrierMEB(std::string code, std::string data)
: MetadataExtensionBlock(static_cast<uint8_t>(
    MetadataTypes::CODE_DATA_CARRIER_MEB),
                       std::to_string(code.length()) + code +
                       data) {
  m_codeLength = static_cast<uint16_t>(code.length());
  m_code = code;
  m_data = data;
}

CodeDataCarrierMEB::CodeDataCarrierMEB(const std::string& rawData) {
  try {
    initFromRaw(rawData);
  } catch (...) {
    throw BlockConstructionException("[NewMEB] Bad raw format");
  }
}

CodeDataCarrierMEB::~CodeDataCarrierMEB() {
}

void CodeDataCarrierMEB::initFromRaw(const std::string& rawData) {
  MetadataExtensionBlock::initFromRaw(rawData);
  try {
    std::stringstream ss(m_metadata);
    ss >> m_codeLength;
    std::stringstream size;
    size << m_codeLength;
    int codeLength = size.str().length();
    m_code = m_metadata.substr(codeLength, m_codeLength);
    m_data = m_metadata.substr(
        codeLength + m_codeLength, m_metadata.size());
  } catch (const std::out_of_range& e) {
    throw BlockConstructionException("[NewMEB] Bad raw format");
  }
}

uint16_t CodeDataCarrierMEB::getCodeLength() {
  return m_codeLength;
}

std::string CodeDataCarrierMEB::getCode() {
  return m_code;
}

std::string CodeDataCarrierMEB::getData() {
  return m_data;
}

