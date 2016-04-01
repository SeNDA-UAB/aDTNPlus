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
 * FILE FrameworkMEB.cpp
 * AUTHOR clacambra
 * DATE 29/03/2016
 * VERSION 1
 * This file contains the implementation of the FrameworkMEB class.
 */

#include <vector>
#include <memory>
#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <utility>

#include "Bundle/FrameworkMEB.h"
#include "Bundle/FrameworkExtension.h"
#include "BundleTypes.h"
#include "Bundle/MetadataExtensionBlock.h"
#include "Bundle/CanonicalBlock.h"
#include "Utils/Logger.h"
#include "Utils/SDNV.h"

/*std::string mapToStr(
    std::map<uint8_t, std::shared_ptr<FrameworkExtension>> extensions) {
  std::stringstream ss;
  for (auto& ext : extensions) {
    ss << ext.second->getFwkExtId();
    ss << ext.second->getCodeLength();
    ss << ext.second->getSwSrcCode();
  }
  return ss.str();
}*/

FrameworkMEB::FrameworkMEB(
    uint8_t fwkId,
    std::map<uint8_t, std::shared_ptr<FrameworkExtension>> extensions,
    std::string state)
    : MetadataExtensionBlock() {
  m_fwkId = fwkId;
  m_fwkExts = extensions;
  m_bundleState = state;
  m_nExtensions = extensions.size();
  std::stringstream ss;
  m_metadataType = static_cast<uint8_t>(MetadataTypes::FRAMEWORK_MEB);
  ss << fwkId << std::to_string(static_cast<uint8_t>(extensions.size()));
     // << mapToStr(extensions) << state;
  for (auto& ext : extensions) {
    ss << ext.second->getFwkExtId() << ext.second->getCodeLength()
        << ext.second->getSwSrcCode();
  }
  ss << state;
  m_metadata = ss.str();
}

FrameworkMEB::FrameworkMEB(const std::string& rawData) {
  try {
    initFromRaw(rawData);
  } catch (const std::out_of_range& e) {
    throw BlockConstructionException("[Framework MEB] Bad raw format");
  }
}

void FrameworkMEB::initFromRaw(const std::string& rawData) {
  MetadataExtensionBlock::initFromRaw(rawData);
  try {
    // m_fwkId = static_cast<uint8_t>(m_metadata[0]);
    // std::cout << "Res: " << m_fwkId << "\n";
    // std::cout << m_metadata << "\n";
    std::stringstream ss(m_metadata);
    ss >> m_fwkId;
    // std::cout << "id: " << static_cast<uint8_t>(m_fwkId) << "\n";
    std::stringstream frameworkId;
    frameworkId << m_fwkId;
    int length = frameworkId.str().length();
    std::string data = m_metadata.substr(length, m_metadata.size());
    std::stringstream ss2(data);
    ss2 >> m_nExtensions;
    std::stringstream nExtensions;
    nExtensions << m_nExtensions;
    length = nExtensions.str().length();
    int numExtensions = std::stoi(data.substr(0, length));
    data = data.substr(length, data.size());
    std::map<uint8_t, std::shared_ptr<FrameworkExtension>> extensions;
    // std::cout << "EXTENSIONS: " << data << "\n";
    // int i = static_cast<int>(m_nExtensions);
    // std::cout << i;
    // std::cout << "N_EXTENSIONS: " << m_nExtensions << "\n";
    while (numExtensions > 0) {
      uint8_t extensionId;
      std::stringstream s(data);
      s >> extensionId;
      std::stringstream s2;
      s2 << extensionId;
      length = s2.str().length();
      data = data.substr(length, data.size());
      std::stringstream s3(data);
      uint16_t codeLength;
      s3 >> codeLength;
      std::stringstream s4;
      s4 << codeLength;
      length = s4.str().length();
      std::string code = data.substr(length, codeLength);
      data = data.substr(length + codeLength, data.size());
      std::shared_ptr<FrameworkExtension> extension = std::shared_ptr<
          FrameworkExtension>(new FrameworkExtension(extensionId, code));
      // std::cout << extensionId << ";" << code << ".\n";
      extensions.insert(
          std::pair<uint8_t, std::shared_ptr<FrameworkExtension>>(extensionId,
                                                                  extension));
      numExtensions--;
      // std::cout << "[InitFromRaw] " << extension->getFwkExtId()
      //    << " " << extension->getSwSrcCode() << "\n";
    }
    m_fwkExts = extensions;
    m_bundleState = data;
  } catch (const std::out_of_range& e) {
    throw BlockConstructionException("[FrameworkMEB] Bad raw format");
  }
}

FrameworkMEB::~FrameworkMEB() {
}

std::string FrameworkMEB::toRaw() {
  LOG(39) << "Generating raw data from Framework MEB";
  std::stringstream ss;
  ss << m_blockType;
  ss << SDNV::encode(m_procFlags.to_ulong());
  ss << SDNV::encode(SDNV::getLength(m_metadataType) + m_metadata.length());
  ss << SDNV::encode(m_metadataType);
  ss << m_metadata;
  m_raw = ss.str();
  return m_raw;
}

uint8_t FrameworkMEB::getFwkId() {
  return m_fwkId;
}

std::map<uint8_t, std::shared_ptr<FrameworkExtension>>
FrameworkMEB::getFwkExts() {
  return m_fwkExts;
}

std::string FrameworkMEB::getBundleState() {
  return m_bundleState;
}

std::shared_ptr<FrameworkExtension> FrameworkMEB::getFwkExt(uint8_t fwkExtId) {
  /*std::shared_ptr<FrameworkExtension> fwkExt = m_fwkExts[fwkExtId];
  if (fwkExt == NULL) {
    return std::nullptr_t();
  }
  return fwkExt;*/
  return m_fwkExts[fwkExtId];
}

