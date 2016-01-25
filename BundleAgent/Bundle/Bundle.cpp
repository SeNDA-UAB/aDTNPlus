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
#include "Bundle/CanonicalBlock.h"
#include "Bundle/MetadataExtensionBlock.h"
#include "Bundle/Block.h"
#include "Bundle/PayloadBlock.h"
#include "Utils/TimestampManager.h"
#include "Utils/SDNV.h"
#include "Utils/Logger.h"

Bundle::Bundle(const std::string &rawData)
    : m_raw(rawData),
      m_primaryBlock(nullptr),
      m_payloadBlock(nullptr) {
  /**
   * A bundle is formed by a PrimaryBlock, and other blocks.
   * In this other blocks one of it must be a PayloadBlock.
   */
  LOG(35) << "New Bundle from raw Data";
  // First generate a PrimaryBlock with the data.
  LOG(35) << "Generating Primary Block";
  try {
    m_primaryBlock = std::shared_ptr<PrimaryBlock>(new PrimaryBlock(rawData));
    m_blocks.push_back(m_primaryBlock);
    // Skip the PrimaryBlock
    std::string data = rawData.substr(m_primaryBlock->getLength());
    // We now can start to generate the known blocks.
    std::shared_ptr<Block> b;
    while (data.size() != 0) {
      switch (static_cast<CanonicalBlockTypes>(data[0])) {
        case CanonicalBlockTypes::PAYLOAD_BLOCK: {
          // Check if another payload block is present
          if (m_payloadBlock == nullptr) {
            LOG(35) << "Generating Payload Block";
            b = std::shared_ptr<PayloadBlock>(new PayloadBlock(data, true));
            m_payloadBlock = std::static_pointer_cast<PayloadBlock>(b);
          } else {
            throw BundleCreationException(
                "[Bundle] More than one payload found");
          }
          break;
        }
        case CanonicalBlockTypes::METADATA_EXTENSION_BLOCK: {
          // This is an abstraction of the metadata block, so we need to create
          // a derived block of it.
          LOG(35) << "Generating Metadata Extension Block";
          b = std::shared_ptr<MetadataExtensionBlock>(
              new MetadataExtensionBlock(data));
          break;
        }
        default: {
          LOG(35) << "Generating Canonical Block";
          b = std::shared_ptr<CanonicalBlock>(new CanonicalBlock(data));
          break;
        }
      }
      m_blocks.push_back(b);
      size_t blockSize = b->getLength();
      if (blockSize <= 0)
        throw BundleCreationException("[Bundle] Bad raw format");
      data = data.substr(blockSize);
    }
    std::vector<std::shared_ptr<Block>>::reverse_iterator finalBlock = m_blocks
        .rbegin();
    if (!std::static_pointer_cast<CanonicalBlock>(*finalBlock)->checkProcFlag(
        CanonicalBlockControlFlags::LAST_BLOCK)) {
      throw BundleCreationException("[Bundle] Last block not marked as such");
    }
  } catch (const BlockConstructionException &e) {
    throw BundleCreationException(e.what());
  } catch (const std::exception &e) {
    throw BundleCreationException("[Bundle] Bad raw format");
  }
}

Bundle::Bundle(std::string origin, std::string destination, std::string payload)
    : m_raw() {
  LOG(34) << "Generating new bundle with parameters [Source: " << origin
          << "][Destination: " << destination << "][Payload: " << payload
          << "]";
  TimestampManager *tm = TimestampManager::getInstance();
  std::pair<uint64_t, uint64_t> timestampValue = tm->getTimestamp();
  m_primaryBlock = std::shared_ptr<PrimaryBlock>(
      new PrimaryBlock(origin, destination, timestampValue.first,
                       timestampValue.second));
  m_payloadBlock = std::shared_ptr<PayloadBlock>(new PayloadBlock(payload));
  m_blocks.push_back(m_primaryBlock);
  m_blocks.push_back(m_payloadBlock);
}

Bundle::~Bundle() {
  LOG(36) << "Deleting Bundle";
}

std::string Bundle::getRaw() {
  return m_raw;
}

std::string Bundle::toRaw() {
  LOG(36) << "Generating bundle in raw format";
  std::string raw = m_raw;
  if (raw == "") {
    std::stringstream ss;
    LOG(36) << "Getting the primary block in raw";
    std::vector<std::shared_ptr<Block>>::reverse_iterator finalBlock = m_blocks
        .rbegin();
    std::static_pointer_cast<CanonicalBlock>(*finalBlock)->setProcFlag(
        CanonicalBlockControlFlags::LAST_BLOCK);
    for (std::vector<std::shared_ptr<Block>>::iterator it = m_blocks.begin();
        it != m_blocks.end(); ++it) {
      LOG(36) << "Getting the next block in raw";
      ss << (*it)->toRaw();
    }
    raw = ss.str();
  }
  return raw;
}

std::shared_ptr<PrimaryBlock> Bundle::getPrimaryBlock() {
  return m_primaryBlock;
}

std::shared_ptr<PayloadBlock> Bundle::getPayloadBlock() {
  return m_payloadBlock;
}

std::vector<std::shared_ptr<Block>> Bundle::getBlocks() {
  return m_blocks;
}

void Bundle::addBlock(std::shared_ptr<CanonicalBlock> newBlock) {
// Check if the block type is a PayloadBlock
// only one can be present into a bundle.
  LOG(37) << "Adding new Block to the bundle";
  if (newBlock->getBlockType()
      != static_cast<uint8_t>(CanonicalBlockTypes::PAYLOAD_BLOCK)) {
    m_blocks.push_back(newBlock);
  } else {
    LOG(3) << "Some one is trying to add another Payload block";
    throw BundleException("[Bundle] a paylod block is present");
  }
}

std::string Bundle::getId() {
  std::stringstream ss;
  ss << m_primaryBlock->getSource() << m_primaryBlock->getCreationTimestamp()
     << m_primaryBlock->getCreationTimestampSeqNumber();
  return ss.str();
}

std::string Bundle::toString() {
  std::stringstream ss;
  for (size_t i = 0; i < m_blocks.size(); ++i) {
    ss << m_blocks[i]->toString();
  }
  return ss.str();
}
