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
 * AUTHOR AngelaFabregues
 * DATE Oct 20, 2015
 * VERSION 1
 * This file contains the implementation of the Block class.
 */

#include "Block.h"

#include <string>
#include "Utils/SDNV.h"
#include "Utils/Logger.h"

Block::Block()
    : m_raw("") {
}

Block::Block(std::string rawData)
    : m_raw(rawData) {
}

Block::~Block() {
}

std::string Block::getRaw() {
  return m_raw;
}

size_t Block::getLength() {
  return m_raw.size();
}
