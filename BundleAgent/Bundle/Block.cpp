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

#include "Bundle/Block.h"
#include <string>
#include "Utils/SDNV.h"

Block::Block()
    : m_blockType(0),
      m_procFlags() {
}

Block::Block(const std::string &rawData)
    : m_blockType(0),
      m_procFlags() {
}

Block::~Block() {
}

size_t Block::getFirstBlockLength(const std::string &rawData) {
  // BlockType 1 byte
  size_t blockLength = 1;
  std::string data = rawData.substr(1);
  // Proc. flags SDNV
  size_t dataSize = getLength(data);
  blockLength += dataSize;
  uint64_t procFlags = decode(data);
  std::bitset<7> blockFlags = std::bitset<7>(procFlags);
  data = data.substr(dataSize);
  // Check if EID fields are present
  if (blockFlags.test(static_cast<ulong>(BlockControlFlags::EID_FIELD))) {
    dataSize = getLength(data);
    blockLength += dataSize;
    int numberOfEID = decode(data);
    data = data.substr(dataSize);
    for (int i = 0; i < numberOfEID; ++i) {
      // Every EID consists of two SDNV fields
      dataSize = getLength(data);
      blockLength += dataSize;
      data = data.substr(dataSize);
      dataSize = getLength(data);
      blockLength += dataSize;
      data = data.substr(dataSize);
    }
  }
  // Block data Length
  dataSize = getLength(data);
  blockLength += dataSize;
  uint64_t blockDataSize = decode(data);
  blockLength += blockDataSize;
  return blockLength;
}

void Block::setProcFlag(BlockControlFlags procFlag) {
  m_procFlags.set(static_cast<ulong>(procFlag));
}

void Block::clearProcFlag(BlockControlFlags procFlag) {
  m_procFlags.reset(static_cast<ulong>(procFlag));
}

bool Block::testProcFlag(BlockControlFlags procFlag) {
  return m_procFlags.test(static_cast<ulong>(procFlag));
}

uint8_t Block::getBlockType() {
  return m_blockType;
}
