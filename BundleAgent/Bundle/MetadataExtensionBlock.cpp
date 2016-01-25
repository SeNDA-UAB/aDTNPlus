/*
 * Copyright (c) 2015 SeNDA
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
 * FILE MetadataExtensionBlock.cpp
 * AUTHOR Blackcatn13
 * DATE Jun 15, 2015
 * VERSION 1
 * This file contains the implementation of the metadata extension block
 */

#include "Bundle/MetadataExtensionBlock.h"

#include <string>
#include <stdexcept>
#include "Utils/SDNV.h"
#include "Utils/Logger.h"

MetadataExtensionBlock::MetadataExtensionBlock(const uint8_t metadataType,
                                               const std::string metadata)
    : m_metadataType(metadataType),
      m_metadata(metadata) {
}

MetadataExtensionBlock::MetadataExtensionBlock(const std::string &rawData)
    : CanonicalBlock(rawData),
      m_metadataType(0),
      m_metadata() {
  try {
    std::string data = m_raw.substr(m_bodyDataIndex);
    size_t metadataTypeSize = SDNV::getLength(data);
    m_metadataType = SDNV::decode(data);
    m_metadata = data.substr(metadataTypeSize);
  } catch (const std::out_of_range& e) {
    throw BlockConstructionException("[MetadataExtensionBlock] Bad raw format");
  }
}

MetadataExtensionBlock::~MetadataExtensionBlock() {
}

std::string MetadataExtensionBlock::toRaw() {
  /**
   * The MetadataExtension block contains
   * according to http://www.rfc-base.org/txt/rfc-6258.txt
   * ignoring EIDs
   *
   * Block Type 1 byte
   * Proc. Flags as SDNV
   * Metadata Type as SDNV
   * Metadata variable length
   */
  LOG(39) << "Generating raw data from metadataExtension block";
  std::stringstream ss;
  ss << m_blockType;
  ss << SDNV::encode(m_procFlags.to_ulong());
  ss << SDNV::encode(m_metadataType);
  ss << m_metadata;
  Block::m_raw = ss.str();
  return Block::m_raw;
}

uint8_t MetadataExtensionBlock::getMetadataType() {
  return m_metadataType;
}

std::string MetadataExtensionBlock::getMetadata() {
  return m_metadata;
}

std::string MetadataExtensionBlock::toString() {
  std::stringstream ss;
  ss << "Metadata Extension Block:" << std::endl;
  return ss.str();
}
