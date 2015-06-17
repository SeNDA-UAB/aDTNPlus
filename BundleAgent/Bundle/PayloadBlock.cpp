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

#include "Bundle/PayloadBlock.h"
#include <string>
#include "Bundle/BundleTypes.h"

PayloadBlock::PayloadBlock()
    : m_payload(nullptr) {
  m_blockType = static_cast<uint8_t>(BlockTypes::PAYLOAD_BLOCK);
}

PayloadBlock::PayloadBlock(uint8_t* rawData) {
}

PayloadBlock::PayloadBlock(const std::string &payload) {
  char* payloadCharArray = const_cast<char*>(payload.c_str());
  m_payload = reinterpret_cast<uint8_t*>(payloadCharArray);
  m_blockType = static_cast<uint8_t>(BlockTypes::PAYLOAD_BLOCK);
}

PayloadBlock::~PayloadBlock() {
}

uint8_t* PayloadBlock::getRaw() {
  return 0;
}

uint8_t* PayloadBlock::getPayload() {
  return m_payload;
}

