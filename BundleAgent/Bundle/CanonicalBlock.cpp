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

#include "CanonicalBlock.h"

#include <string>
#include "Utils/SDNV.h"
#include "Utils/Logger.h"

CanonicalBlock::CanonicalBlock(const std::string &rawData)
    : Block::m_raw(rawData),
	  m_blockType(0),
      m_procFlags() {
	/*
	 * TODO Method implementation to be finished.
	 * Assume that the Block is unknown (not Primary, not Payload, not MetadataExtension).
	 * Get from rawData content the m_blockType, m_procFlags and length.
	 * Save at m_bodyDataIndex the cardinality of the first bit containing body data (after length).
	 * Save at Block::m_raw the substring of rawData corresponding to this block (from 0 to length).
	 * Save m_blockType and m_procFlags.
	 */
  /**
   * The canonical block contains
   *
   * Block Type 1 byte
   * Proc. Flags as SDNV
   * Block Length as SDNV
   * BodyDataContent variable length
   */
  LOG(39) << "Generating canonicalblock from raw data";
  // Get the Block Type
  m_blockType = static_cast<uint8_t>(rawData[0]);
  std::string data = rawData.substr(1);
  // Get the proc flags.
  size_t procFlagsSize = getLength(data);
  uint64_t procFlags = decode(data);
  m_procFlags = std::bitset<7>(procFlags);
  data = data.substr(procFlagsSize);
  // Jump the payload length, the rawData only contains this block
  size_t payloadSize = getLength(data);
  data = data.substr(payloadSize);
  m_bodyDataIndex = 0;
}

CanonicalBlock::~CanonicalBlock() {
}

std::string PrimaryBlock::toRaw() {
  std::stringstream ss;
  ss << Block::m_raw;
 /*
  * TODO Method to be implemented.
  * Assume that the Block is unknown (not Primary, not Payload, not MetadataExtension).
  * Take Block::m_raw and update its flags. The rest must for sure be OK because it is not possible to be changed.
  * Return the updated Block::m_raw.
  */
  Block::m_raw = ss.str();
  return Block::m_raw;
}

uint8_t CanonicalBlock::getBlockType() {
  return m_blockType;
}

void CanonicalBlock::setProcFlag(BlockControlFlags procFlag) {
  LOG(38) << "Setting Flag " << static_cast<int>(procFlag);
  m_procFlags.set(static_cast<ulong>(procFlag));
}

void CanonicalBlock::unsetProcFlag(BlockControlFlags procFlag) {
  LOG(38) << "Clearing Flag " << static_cast<int>(procFlag);
  m_procFlags.reset(static_cast<ulong>(procFlag));
}

bool CanonicalBlock::checkProcFlag(BlockControlFlags procFlag) {
  LOG(38) << "Testing Flag " << static_cast<int>(procFlag);
  return m_procFlags.test(static_cast<ulong>(procFlag));
}
