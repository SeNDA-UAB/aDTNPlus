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
 * FILE Block.cpp
 * AUTHOR Blackcatn13
 * DATE Jun 15, 2015
 * VERSION 1
 * This file contains the implementation of the Block class.
 */

#include "Bundle/Block.h"

Block::Block()
    : m_blockType(0),
      m_procFlags() {
}

Block::Block(uint8_t* rawData)
    : m_blockType(0),
      m_procFlags() {
}

Block::~Block() {
}

void Block::setProcFlag(BlockControlFlags procFlag) {
  m_procFlags.set(static_cast<ulong>(procFlag));
}

void Block::clearProcFlag(BlockControlFlags procFlag) {
  m_procFlags.reset(static_cast<ulong>(procFlag));
}

bool Block::testProcFlag(BlockControlFlags procFlag) {
  return m_procFlags.test(static_cast<ulong>(procFlag));
}

uint8_t Block::getBlockType() {
  return m_blockType;
}
