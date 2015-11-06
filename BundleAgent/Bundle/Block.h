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
 * FILE Block.h
 * AUTHOR AngelaFabregues
 * DATE Oct 29, 2015
 * VERSION 1
 * This file contains the Block class.
 */
#ifndef BUNDLEAGENT_BUNDLE_BLOCK_H_
#define BUNDLEAGENT_BUNDLE_BLOCK_H_

#include <cstdint>
#include <bitset>
#include <string>
#include <stdexcept>
#include "Bundle/BundleTypes.h"

class BlockConstructionException : public std::runtime_error {
 public:
  explicit BlockConstructionException(const std::string &what)
      : runtime_error(what) {
  }
};

/**
 * CLASS Block
 * This class represents a block as described into the RFC 5050.
 * This class is a base class for all the blocks that a bundle can hold.
 */
class Block {
 public:
  /**
   * @brief Empty constructor.
   *
   * Generates an empty block.
   */
  Block();
  /**
   * @brief Constructor.
   *
   * Generates a block.
   *
   * @param rawData
   */
  explicit Block(std::string rawData);
  /**
   * Destructor of the class.
   */
  virtual ~Block();
  /**
   * @brief Function to get the block in raw format.
   *
   * This function will provide the last version of the piece of bundle raw that
   * corresponds to this block. Notice that it may not be up to date.
   *
   * @return the block in raw format.
   */
  std::string getRaw();
  /**
   * @brief Function to update the block in raw format.
   *
   * This function will generate a raw version of the piece of bundle raw that
   * corresponds to this block.
   * Pure virtual function, all the blocks must implement it.
   *
   * @return the block in raw format.
   */
  virtual std::string toRaw() = 0;
  /**
   * @brief Returns the size of this block.
   *
   * @return the size of the block.
   */
  size_t getLength();

 protected:
  /**
   * Byte array containing the piece of raw bundle that corresponds to this block.
   */
  std::string m_raw;
};

#endif  // BUNDLEAGENT_BUNDLE_BLOCK_H_
