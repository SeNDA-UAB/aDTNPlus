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
 * FILE Block.cpp
 * AUTHOR Blackcatn13
 * DATE Jun 15, 2015
 * VERSION 1
 * This file contains the implementation of the Block class.
 */

#include "Bundle/CanonicalBlock.h"
#include <string>
#include <sstream>
#include <stdexcept>
#include "Utils/SDNV.h"
#include "Utils/Logger.h"

CanonicalBlock::CanonicalBlock()
    : m_blockType(0),
      m_bodyDataIndex(0),
      m_procFlags() {
}

CanonicalBlock::CanonicalBlock(const std::string &rawData)
    : m_blockType(0),
      m_procFlags() {
  /**
   * The canonical block contains
   *
   * Block Type 1 byte
   * Proc. Flags as SDNV
   * Block Length as SDNV
   * BodyDataContent variable length
   */
  initFromRaw(rawData);
}

CanonicalBlock::~CanonicalBlock() {
}

void CanonicalBlock::initFromRaw(const std::string &rawData) {
  LOG(39) << "Generating canonicalblock from raw data";
  // Get the Block Type
  try {
    m_blockType = static_cast<uint8_t>(rawData[0]);
    size_t blockLength = 1;
    std::string data = rawData.substr(1);
    // Get the proc flags.
    size_t dataSize = SDNV::getLength(data);
    blockLength += dataSize;
    uint64_t procFlags = SDNV::decode(data);
    m_procFlags = std::bitset<7>(procFlags);
    data = data.substr(dataSize);
    if (m_procFlags.test(static_cast<uint32_t>(CanonicalBlockControlFlags::EID_FIELD))) {
      dataSize = SDNV::getLength(data);
      blockLength += dataSize;
      int numberOfEID = SDNV::decode(data);
      data = data.substr(dataSize);
      for (int i = 0; i < numberOfEID; ++i) {
        // Every EID consists of two SDNV fields
        dataSize = SDNV::getLength(data);
        blockLength += dataSize;
        data = data.substr(dataSize);
        dataSize = SDNV::getLength(data);
        blockLength += dataSize;
        data = data.substr(dataSize);
      }
    }
    // Block data Length
    dataSize = SDNV::getLength(data);
    blockLength += dataSize;
    m_bodyDataIndex = blockLength;
    uint64_t blockDataSize = SDNV::decode(data);
    blockLength += blockDataSize;
    m_raw = rawData.substr(0, blockLength);
  } catch (const std::out_of_range& e) {
    throw BlockConstructionException("[CanonicalBlock] Bad raw format");
  }
}

std::string CanonicalBlock::toRaw() {
  std::stringstream ss;
  ss << m_blockType;
  ss << SDNV::encode(m_procFlags.to_ulong());
  std::string data = m_raw.substr(m_bodyDataIndex);
  ss << SDNV::encode(data.size());
  ss << data;
  m_raw = ss.str();
  return m_raw;
}

uint8_t CanonicalBlock::getBlockType() {
  return m_blockType;
}

void CanonicalBlock::setProcFlag(CanonicalBlockControlFlags procFlag) {
  LOG(38) << "Setting Flag " << static_cast<int>(procFlag);
  m_procFlags.set(static_cast<uint32_t>(procFlag));
}

void CanonicalBlock::unsetProcFlag(CanonicalBlockControlFlags procFlag) {
  LOG(38) << "Clearing Flag " << static_cast<int>(procFlag);
  m_procFlags.reset(static_cast<uint32_t>(procFlag));
}

bool CanonicalBlock::checkProcFlag(CanonicalBlockControlFlags procFlag) {
  LOG(38) << "Testing Flag " << static_cast<int>(procFlag);
  return m_procFlags.test(static_cast<uint32_t>(procFlag));
}
