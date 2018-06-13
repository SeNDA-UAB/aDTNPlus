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
 * FILE BundleInfo.cpp
 * AUTHOR Blackcatn13
 * DATE Apr 4, 2016
 * VERSION 1
 *
 */

#include "Bundle/BundleInfo.h"
#include <string>
#include <ctime>
#include <vector>
#include "Bundle/PrimaryBlock.h"
#include "Bundle/CanonicalBlock.h"
#include "Bundle/MetadataExtensionBlock.h"

const uint64_t g_timeFrom2000 = 946684800;

BundleInfo::BundleInfo(Bundle &bundle)
    : m_id(bundle.getId()),
      m_destination(bundle.getPrimaryBlock()->getDestination()),
      m_source(bundle.getPrimaryBlock()->getSource()),
      m_creationTimestamp(bundle.getPrimaryBlock()->getCreationTimestamp()),
      m_creationTimestampSeqNumber(
          bundle.getPrimaryBlock()->getCreationTimestampSeqNumber()),
      m_lifetime(bundle.getPrimaryBlock()->getLifetime()),
      m_size(bundle.toRaw().length()) {
  std::vector<std::shared_ptr<Block>> blocks = bundle.getBlocks();
  blocks.erase(blocks.begin());
  for (auto c : blocks) {
    std::shared_ptr<CanonicalBlock> canonical_block = std::static_pointer_cast<
        CanonicalBlock>(c);
    m_canoniclaTypeBlocks.insert(canonical_block->getBlockType());
    if (static_cast<CanonicalBlockTypes>(canonical_block->getBlockType())
        == CanonicalBlockTypes::METADATA_EXTENSION_BLOCK) {
      std::shared_ptr<MetadataExtensionBlock> meb = std::static_pointer_cast<
                MetadataExtensionBlock>(canonical_block);
      m_metadataTypeBlocks.insert(meb->getMetadataType());
    }
  }
}

BundleInfo::~BundleInfo() {
}

std::string BundleInfo::getId() const {
  return m_id;
}

std::string BundleInfo::getDestination() const {
  return m_destination;
}

std::string BundleInfo::getSource() const {
  return m_source;
}

uint64_t BundleInfo::getCreationTimestamp() const {
  return m_creationTimestamp;
}

uint64_t BundleInfo::getCreationTimestampSeqNumber() const {
  return m_creationTimestampSeqNumber;
}

uint64_t BundleInfo::getLifetime() const {
  return m_lifetime;
}

uint64_t BundleInfo::getCurrentLifetime() const {
  return (time(NULL) - g_timeFrom2000 - m_creationTimestamp - m_lifetime);
}

uint64_t BundleInfo::getSize() const {
  return m_size;
}

bool BundleInfo::hasMetadataTypeBlock(uint8_t type) const {
  auto it = m_metadataTypeBlocks.find(type);
  return (it != m_metadataTypeBlocks.end());
}

bool BundleInfo::hasCanonicalTypeBlock(uint8_t type) const {
  auto it = m_canoniclaTypeBlocks.find(type);
  return (it != m_canoniclaTypeBlocks.end());
}

