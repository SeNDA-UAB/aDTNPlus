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

PayloadBlock::PayloadBlock(const std::string &payload, bool isRaw)
    : CanonicalBlock() {
  LOG(39) << "Generating new payload block";
  if (isRaw) {
    initFromRaw(payload);
    m_payload = payload.substr(m_bodyDataIndex);
  } else {
    m_payload = payload;
    m_blockType = static_cast<uint8_t>(BlockTypes::PAYLOAD_BLOCK);
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
  ss << CanonicalBlock::toRaw();
  ss << SDNV::encode(m_payload.size());
  ss << m_payload;
  m_raw = ss.str();
  return m_raw;
}

std::string PayloadBlock::getPayload() {
  return m_payload;
}

