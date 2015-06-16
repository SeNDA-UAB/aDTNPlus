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
 * AUTHOR Blackcatn13
 * DATE Jun 15, 2015
 * VERSION 1
 * This file contains the Block class.
 */
#ifndef BUNDLEAGENT_BUNDLE_BLOCK_H_
#define BUNDLEAGENT_BUNDLE_BLOCK_H_

#include <cstdio>
#include <bitset>

/**
 * CLASS Block
 * This class represents a canonical block as described into the RFC 5050.
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
   * @brief Raw constructor.
   *
   * Generates a block from a raw data.
   *
   * @param rawData the raw containing the block information.
   */
  explicit Block(uint8_t* rawData);
  /**
   * Destructor of the class.
   */
  virtual ~Block();
  /**
   * @brief Function to transform this block to raw format.
   *
   * Virtual function, all the blocks that derives from this class must implement it.
   * This function must return the block in raw format.
   *
   * @return The block in raw format.
   */
  virtual uint8_t* getRaw() = 0;
  /**
   * @brief Sets the given flag.
   *
   * This function sets the given flag to value 1.
   *
   * @param procFlag the flag to set.
   *
   * @sa BlockControlFlags
   */
  void setProcFlag(BlockControlFlags procFlag);
  /**
   * @brief Clears the given flag.
   *
   * This function sets the given flag to value 0.
   *
   * @param procFlag the flag to clear.
   *
   * @sa BlockControlFlags
   */
  void clearProcFlag(BlockControlFlags procFlag);
  /**
   * @brief Test if a flag is active or not.
   *
   * This function tests if the given flag is active or not.
   *
   * @param procFlag the flag to test.
   * @return True if the flag is active, false otherwise.
   *
   * @sa BlockControlFlags
   */
  bool testProcFlag(BlockControlFlags procFlag);
  /**
   * This function returns the block type value.
   *
   * @return The block type field.
   */
  uint8_t getBlockType();

 private:
  /**
   * Variable that holds the Block Type value.
   */
  uint8_t m_blockType;
  /**
   * Block processing control flags, as described into the RFC 5050.
   */
  std::bitset<7> m_procFlags;
};

#endif  // BUNDLEAGENT_BUNDLE_BLOCK_H_
