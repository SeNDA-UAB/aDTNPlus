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

#include "MetadataExtensionBlock.h"

#include <string>

MetadataExtensionBlock::MetadataExtensionBlock(const uint8_t metadataType,
                                               const std::string metadata)
    : m_metadataType(metadataType),
      m_metadata(metadata) {
}

MetadataExtensionBlock::MetadataExtensionBlock(const std::string &rawData)
    : m_metadataType(0),
      m_metadata() {
  /*
   * TODO to be implemented again.
   * Use CanonicalBlock(rawData) constructor and complete the parsing here knowing that the body data content starts at m_bodyDataIndex.
   */
}

MetadataExtensionBlock::~MetadataExtensionBlock() {
}

uint8_t MetadataExtensionBlock::getMetadataType() {
  return m_metadataType;
}

std::string MetadataExtensionBlock::getMetadata() {
  return m_metadata;
}
