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
 * FILE Bundle.h
 * AUTHOR BLackcatn13
 * DATE Jun 15, 2015
 * VERSION 1
 * This file contains the class Bundle
 */
#ifndef BUNDLEAGENT_BUNDLE_BUNDLE_H_
#define BUNDLEAGENT_BUNDLE_BUNDLE_H_

#include <cstdint>
#include <string>
#include <vector>

#include "PrimaryBlock.h"
#include "Block.h"
/**
 * CLASS Bundle
 * This class represents a Bundle as defined into the RFC 5050
 */
class Bundle {
 public:
  Bundle();
  explicit Bundle(uint8_t* rawData);
  Bundle(std::string origin, std::string destination, std::string payload);
  virtual ~Bundle();
  uint8_t* getRaw();
  PrimaryBlock* getPrimaryBlock();
  std::vector<Block *> getBlocks();
 private:
  PrimaryBlock* m_primaryBlock;
  std::vector<Block *> m_blocks;
  uint8_t* m_raw;
};

#endif  // BUNDLEAGENT_BUNDLE_BUNDLE_H_
