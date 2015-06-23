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
 * FILE Bundle.cpp
 * AUTHOR Blackcatn13
 * DATE Jun 15, 2015
 * VERSION 1
 * This file contains the Bundle class implementation.
 */

#include "Bundle/Bundle.h"
#include <string>
#include <vector>
#include <utility>
#include <sstream>
#include "Bundle/PrimaryBlock.h"
#include "Bundle/Block.h"
#include "Bundle/PayloadBlock.h"
#include "Utils/TimestampManager.h"
#include "Utils/SDNV.h"

Bundle::Bundle(const std::string &rawData)
    : m_raw(rawData),
      m_primaryBlock(nullptr) {
  /**
   * A bundle is formed by a PrimaryBlock, and other blocks.
   * In this other blocks one of it must be a PayloadBlock.
   */
  // First generate a PrimaryBlock with the data.
  m_primaryBlock = new PrimaryBlock(rawData);
  // Skip the PrimaryBlock (1 byte, a SDNV, and we get the block length)
  std::string data = rawData.substr(1);
  size_t offset = 1;
  size_t length = getLength(data);
  offset += length;
  data = data.substr(length);
  length = getLength(data);
  uint64_t blockSize = decode(data);
  offset += length + blockSize;
  data = rawData.substr(offset);
  // We now can start to generate the known blocks.
  while (data.size() != 0) {
    switch (static_cast<BlockTypes>(data[0])) {
      case BlockTypes::PAYLOAD_BLOCK: {
        Block* b = new PayloadBlock(data);
        m_blocks.push_back(b);
        break;
      }
      case BlockTypes::METADATA_EXTENSION_BLOCK: {
        // This is an abstraction of the metadata block, so we need to create
        // a derived block of it.
      }
    }
    size_t size = Block::getFirstBlockLength(data);
    data = data.substr(size);
  }
}

Bundle::Bundle(std::string origin, std::string destination, std::string payload)
    : m_raw() {
  TimestampManager *tm = TimestampManager::getInstance();
  std::pair<uint64_t, uint64_t> timestampValue = tm->getTimestamp();
  m_primaryBlock = new PrimaryBlock(origin, destination, timestampValue.first,
                                    timestampValue.second);
  Block* pb = new PayloadBlock();
  (static_cast<PayloadBlock*>(pb))->setPayload(payload);
  m_blocks.push_back(pb);
}

Bundle::~Bundle() {
  if (m_primaryBlock != nullptr)
    delete m_primaryBlock;
  for (std::vector<Block*>::iterator it = m_blocks.begin();
      it != m_blocks.end(); ++it) {
    if (*it != nullptr)
      delete (*it);
  }
}

std::string Bundle::getRaw() {
  std::string raw = m_raw;
  if (raw == "") {
    std::stringstream ss;
    ss << m_primaryBlock->getRaw();
    for (std::vector<Block*>::iterator it = m_blocks.begin();
        it != m_blocks.end(); ++it) {
      ss << (*it)->getRaw();
    }
    raw = ss.str();
  }
  return raw;
}

PrimaryBlock* Bundle::getPrimaryBlock() {
  return m_primaryBlock;
}

std::vector<Block *> Bundle::getBlocks() {
  return m_blocks;
}

void Bundle::addBlock(Block *newBlock) {
  // Check if the block type is a PayloadBlock
  // only one can be present into a bundle.
  if (newBlock->getBlockType()
      != static_cast<uint8_t>(BlockTypes::PAYLOAD_BLOCK)) {
    m_blocks.push_back(newBlock);
  }
}

