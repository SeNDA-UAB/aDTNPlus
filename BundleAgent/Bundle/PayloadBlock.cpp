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
 * FILE PayloadBlock.cpp
 * AUTHOR Blackcatn13
 * DATE Jun 16, 2015
 * VERSION 1
 * This file contains the implementation of the PayloadBlock class.
 */

#include "PayloadBlock.h"

#include <string>
#include <sstream>
#include "Bundle/BundleTypes.h"
#include "Utils/SDNV.h"
#include "Utils/Logger.h"

PayloadBlock::PayloadBlock(const std::string &payload, bool ok)
    : m_payload(payload) {
  LOG(39) << "Generating new payload block";
  m_blockType = static_cast<uint8_t>(BlockTypes::PAYLOAD_BLOCK);
}

PayloadBlock::PayloadBlock(const std::string &rawData) {
  /*
   * TODO to be implemented again.
   * Use CanonicalBlock(rawData) constructor and complete the parsing here knowing that the body data content starts at m_bodyDataIndex.
   */
  /**
   * The payload block contains
   *
   * Block Type 1 byte
   * Proc. Flags as SDNV
   * Block Length as SDNV
   * Payload variable length
   */
  LOG(39) << "Generating payload from raw data";
  // Get the Block Type
  uint8_t blockType = static_cast<uint8_t>(rawData[0]);
  // If the block type is a paylod block
  if (blockType == static_cast<uint8_t>(BlockTypes::PAYLOAD_BLOCK)) {
    // Set the block type to payload
    m_blockType = static_cast<uint8_t>(BlockTypes::PAYLOAD_BLOCK);
    std::string data = rawData.substr(1);
    // Get the proc flags.
    size_t procFlagsSize = getLength(data);
    uint64_t procFlags = decode(data);
    m_procFlags = std::bitset<7>(procFlags);
    data = data.substr(procFlagsSize);
    // Jump the payload length, the rawData only contains this block
    size_t payloadSize = getLength(data);
    data = data.substr(payloadSize);
    m_payload = data;
  }
}

PayloadBlock::~PayloadBlock() {
}

std::string PayloadBlock::toRaw() {
  /**
   * The payload block contains
   *
   * Block Type 1 byte
   * Proc. Flags as SDNV
   * Block Length as SDNV
   * Payload variable length
   */
  LOG(39) << "Generating raw data from payload block";
  std::stringstream ss;
  ss << m_blockType;
  ss << encode(m_procFlags.to_ulong());
  ss << encode(m_payload.size());
  ss << m_payload;
  Block::m_raw = ss.str();
  return Block::m_raw;
}

std::string PayloadBlock::getPayload() {
  return m_payload;
}

