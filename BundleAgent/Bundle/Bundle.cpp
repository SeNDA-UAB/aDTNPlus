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
#include "Bundle/PrimaryBlock.h"
#include "Bundle/Block.h"
#include "Bundle/PayloadBlock.h"
#include "Utils/TimestampManager.h"

Bundle::Bundle()
    : m_raw(nullptr),
      m_primaryBlock(nullptr) {
}

Bundle::Bundle(uint8_t *rawData)
    : m_raw(rawData),
      m_primaryBlock(nullptr) {
}

Bundle::Bundle(std::string origin, std::string destination, std::string payload)
    : m_raw(nullptr) {
  TimestampManager *tm = TimestampManager::getInstance();
  std::pair<uint64_t, uint64_t> timestampValue = tm->getTimestamp();
  m_primaryBlock = new PrimaryBlock(origin, destination, timestampValue.first,
                                    timestampValue.second);
  Block* pb = new PayloadBlock(payload);
  m_blocks.push_back(pb);
}

Bundle::~Bundle() {
}

uint8_t* Bundle::getRaw() {
  return m_raw;
}

PrimaryBlock* Bundle::getPrimaryBlock() {
  return m_primaryBlock;
}

std::vector<Block *> Bundle::getBlocks() {
  return m_blocks;
}

void Bundle::addBlock(Block *newBlock) {
  // Check if the block type is a PayloadBlock
  // only one can be pressent into a bundle.
  if (newBlock->getBlockType()
      != static_cast<uint8_t>(BlockTypes::PAYLOAD_BLOCK)) {
    m_blocks.push_back(newBlock);
  }
}

