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
 * AUTHOR BLackcatn13
 * DATE Jun 15, 2015
 * VERSION 1
 *
 */
#include <string>
#include <vector>
#include "Bundle.h"

Bundle::Bundle()
    : m_raw(nullptr),
      m_primaryBlock(nullptr) {
}

Bundle::Bundle(uint8_t *rawData)
    : m_raw(rawData),
      m_primaryBlock(nullptr) {
}

Bundle::Bundle(std::string origin, std::string destiantion, std::string payload)
    : m_raw(nullptr) {
  m_primaryBlock = new PrimaryBlock();
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

