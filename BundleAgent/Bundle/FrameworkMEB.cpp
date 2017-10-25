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

FrameworkMEB::FrameworkMEB(
    uint8_t fwkId,
    std::map<uint8_t, std::shared_ptr<FrameworkExtension>> extensions,
    nlohmann::json state)
    : MetadataExtensionBlock(),
      m_fwkId(fwkId),
      m_fwkExts(extensions),
      m_bundleState(state) {
  std::stringstream ss;
  m_metadataType = static_cast<uint8_t>(MetadataTypes::FRAMEWORK_MEB);
  ss << m_fwkId << std::to_string(static_cast<uint8_t>(m_fwkExts.size()));
  for (auto& ext : m_fwkExts) {
    ss << ext.second->getFwkExtId() << ext.second->getCodeLength()
       << ext.second->getSwSrcCode();
  }
  ss << m_bundleState;
  m_metadata = ss.str();
}

FrameworkMEB::FrameworkMEB(uint8_t fkwId)
    : MetadataExtensionBlock(),
      m_fwkId(fkwId),
      m_fwkExts(),
      m_bundleState() {
  std::stringstream ss;
  m_metadataType = static_cast<uint8_t>(MetadataTypes::FRAMEWORK_MEB);
  ss << m_fwkId << std::to_string(static_cast<uint8_t>(m_fwkExts.size()));
  for (auto& ext : m_fwkExts) {
    ss << ext.second->getFwkExtId() << ext.second->getCodeLength()
       << ext.second->getSwSrcCode();
  }
  ss << m_bundleState;
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
    std::stringstream ss(m_metadata);
    ss >> m_fwkId;
    std::stringstream size;
    size << m_fwkId;
    int length = size.str().length();
    std::string data = m_metadata.substr(length, m_metadata.size());
    ss.str(data);
    uint16_t numExtensions;
    ss >> numExtensions;
    size.clear();
    size.str(std::string());
    size << numExtensions;
    length = size.str().length();
    data = data.substr(length, data.size());
    for (uint16_t i = 0; i < numExtensions; ++i) {
      uint8_t extensionId;
      ss.str(data);
      ss >> extensionId;
      size.clear();
      size.str(std::string());
      size << extensionId;
      length = size.str().length();
      data = data.substr(length, data.size());
      ss.str(data);
      uint16_t codeLength;
      ss >> codeLength;
      size.clear();
      size.str(std::string());
      size << codeLength;
      length = size.str().length();
      std::string code = data.substr(length, codeLength);
      data = data.substr(length + codeLength, data.size());
      std::shared_ptr<FrameworkExtension> extension = std::shared_ptr<
          FrameworkExtension>(new FrameworkExtension(extensionId, code));
      m_fwkExts.insert(
          std::pair<uint8_t, std::shared_ptr<FrameworkExtension>>(extensionId,
                                                                  extension));
    }
    try {
      m_bundleState = nlohmann::json::parse(data);
    } catch (const std::invalid_argument &e) {
      throw BlockConstructionException(
          "[FrameworkMEB] Bad Bundle State format");
    }
  } catch (const std::out_of_range &e) {
    throw BlockConstructionException("[FrameworkMEB] Bad raw format");
  }
}

FrameworkMEB::~FrameworkMEB() {
}

std::string FrameworkMEB::toRaw() {
  LOG(87) << "Generating raw data from Framework MEB";
  std::stringstream ss1;
  ss1 << m_fwkId << std::to_string(static_cast<uint8_t>(m_fwkExts.size()));
  for (auto &ext : m_fwkExts) {
    ss1 << ext.second->getFwkExtId() << ext.second->getCodeLength()
        << ext.second->getSwSrcCode();
  }
  ss1 << m_bundleState;
  m_metadata = ss1.str();
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

std::map<uint8_t, std::shared_ptr<FrameworkExtension>> FrameworkMEB::getFwkExts() {
  return m_fwkExts;
}

nlohmann::json& FrameworkMEB::getBundleState() {
  return m_bundleState;
}

std::shared_ptr<FrameworkExtension> FrameworkMEB::getFwkExt(uint8_t fwkExtId) {
  auto it = m_fwkExts.find(fwkExtId);
  if (it != m_fwkExts.end()) {
    return it->second;
  }
  throw ExtensionNotFoundException("[FrameworkMEB] Extension not found");
}

void FrameworkMEB::setBundleState(nlohmann::json state) {
  m_bundleState = state;
}

void FrameworkMEB::addExtension(uint8_t extId,
                                std::string code) {
  auto ext = std::make_shared<FrameworkExtension>(extId, code);
  m_fwkExts[extId] = ext;
}

std::string FrameworkMEB::toString() {
  std::stringstream ss;
  ss << "Framework block:" << std::endl << MetadataExtensionBlock::toString()
     << "\tFramework id: " << static_cast<int>(m_fwkId) << std::endl
     << "\tBundle State: " << m_bundleState.dump(2) << std::endl
     << "\tExtensions:" << std::endl;
  for (auto it = m_fwkExts.begin(); it != m_fwkExts.end(); ++it) {
    ss << "\t\tExtension id: " << static_cast<int>(it->second->getFwkExtId())
       << std::endl << "\t\tExtension code: " << it->second->getSwSrcCode()
       << std::endl;
  }
  return ss.str();
}
