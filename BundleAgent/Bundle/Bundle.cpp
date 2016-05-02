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
#include <map>
#include "Bundle/BundleTypes.h"
#include "Bundle/Block.h"
#include "Bundle/PrimaryBlock.h"
#include "Bundle/CanonicalBlock.h"
#include "Bundle/MetadataExtensionBlock.h"
#include "Bundle/RoutingSelectionMEB.h"
#include "Bundle/ForwardingMEB.h"
#include "Bundle/RouteReportingMEB.h"
#include "Bundle/CodeDataCarrierMEB.h"
#include "Bundle/PayloadBlock.h"
#include "Utils/TimestampManager.h"
#include "Utils/SDNV.h"
#include "Utils/Logger.h"
#include "Bundle/FrameworkMEB.h"
#include "Bundle/FrameworkExtension.h"

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
          auto m = MetadataExtensionBlock(data);
          LOG(35) << std::to_string(m.getMetadataType());
          switch (static_cast<MetadataTypes>(m.getMetadataType())) {
            case MetadataTypes::ROUTING_SELECTION_MEB: {
              b = std::make_shared<RoutingSelectionMEB>(
                  RoutingSelectionMEB(data));
              break;
            }
            case MetadataTypes::FORWARDING_MEB: {
              LOG(35) << "Generating ForwardingMEB Block.";
              b = std::make_shared<ForwardingMEB>(ForwardingMEB(data, true));
              break;
            }
            case MetadataTypes::ROUTE_REPORTING_MEB: {
              LOG(35) << "Generating RouteReporting Metadata Extension Block";
              b = std::make_shared<RouteReportingMEB>(RouteReportingMEB(data));
              break;
            }
            case MetadataTypes::CODE_DATA_CARRIER_MEB: {
              LOG(35) << "Generating New Metadata Extension Block";
              b = std::make_shared<CodeDataCarrierMEB>(
                  CodeDataCarrierMEB(data));
              break;
            }
            case MetadataTypes::FRAMEWORK_MEB: {
              LOG(35) << "Generating New Framework Metadata Extension Block";
              b = std::make_shared<FrameworkMEB>(FrameworkMEB(data));
              break;
            }
          }
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
  std::stringstream ss;
  LOG(36) << "Getting the primary block in raw";
  std::vector<std::shared_ptr<Block>>::reverse_iterator finalBlock = m_blocks
      .rbegin();
  std::static_pointer_cast<CanonicalBlock>(*finalBlock)->setProcFlag(
      CanonicalBlockControlFlags::LAST_BLOCK);
  for (std::vector<std::shared_ptr<Block>>::iterator it = m_blocks.begin();
      it != m_blocks.end(); ++it) {
    ss << (*it)->toRaw();
  }
  raw = ss.str();
  m_raw = raw;
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

std::shared_ptr<FrameworkMEB> Bundle::getFwk(uint8_t fwkId) {
  auto it = m_blocks.begin();
  for (++it; it != m_blocks.end(); ++it) {
    auto cb = std::static_pointer_cast<CanonicalBlock>(*it);
    if (static_cast<CanonicalBlockTypes>(cb->getBlockType())
        == CanonicalBlockTypes::METADATA_EXTENSION_BLOCK) {
      auto meb = std::static_pointer_cast<MetadataExtensionBlock>(cb);
      if (static_cast<MetadataTypes>(meb->getMetadataType())
          == MetadataTypes::FRAMEWORK_MEB) {
        auto fmeb = std::static_pointer_cast<FrameworkMEB>(meb);
        if (fmeb->getFwkId() == fwkId) {
          return fmeb;
        }
      }
    }
  }
  throw FrameworkNotFoundException("[Bundle] Framework not found.");
}

std::shared_ptr<FrameworkExtension> Bundle::getFwkExt(uint8_t fwkId,
                                                      uint8_t fwkExtId) {
  try {
    auto fmeb = getFwk(fwkId);
    return fmeb->getFwkExt(fwkExtId);
  } catch (const ExtensionNotFoundException &e) {
    throw FrameworkNotFoundException("[Bundle] Framework extension not found.");
  } catch (const FrameworkNotFoundException &e) {
    throw;
  }
}
