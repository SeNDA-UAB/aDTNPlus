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
 * FILE ForwardingMEB.cpp
 * AUTHOR clacambra
 * DATE 10/02/2016
 * VERSION 1
 * This file contains the implementation of Forwarding MEB.
 */

#include <string>
#include "ForwardingMEB.h"
#include "Bundle/BundleTypes.h"

ForwardingMEB::ForwardingMEB(const std::string &softCode, bool isRaw)
  : MetadataExtensionBlock() {
    if (isRaw) {
      try {
        initFromRawMEB(softCode);
        m_softCode = m_metadata;
      } catch (...) {
        throw BlockConstructionException("[ForwardingMEB] Bar raw format");
      }
    } else {
      m_blockType = static_cast<uint8_t>(
          CanonicalBlockTypes::METADATA_EXTENSION_BLOCK);
      m_metadataType = static_cast<uint8_t>(MetadataTypes::FORWARDING_MEB);
      m_metadata = softCode;
      m_softCode = softCode;
    }
}

ForwardingMEB::~ForwardingMEB() {
}

std::string ForwardingMEB::getSoftCode() {
  return m_softCode;
}
