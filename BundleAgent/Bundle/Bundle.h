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
 * AUTHOR Blackcatn13
 * DATE Jun 15, 2015
 * VERSION 1
 * This file contains the class Bundle.
 */
#ifndef BUNDLEAGENT_BUNDLE_BUNDLE_H_
#define BUNDLEAGENT_BUNDLE_BUNDLE_H_

#include <cstdint>
#include <string>
#include <vector>

class PrimaryBlock;
class Block;
/**
 * CLASS Bundle
 * This class represents a Bundle as defined into the RFC 5050.
 */
class Bundle {
 public:
  /**
   * @brief Raw constructor.
   **
   * This constructor will take a raw bundle and reconstruct the bundle from it.
   *
   * @param rawData the bundle in raw to convert to a Bundle class.
   */
  explicit Bundle(const std::string &rawData);
  /**
   * @brief Constructs a bundle with the provided information.
   *
   * This constructor will create a bundle with a source, a destination and a payload.
   *
   * @param origin of the bundle to generate.
   * @param destination to send the bundle.
   * @param payload information to add to the bundle.
   */
  Bundle(std::string origin, std::string destination, std::string payload);
  /**
   * Destructor of the class.
   */
  virtual ~Bundle();
  /**
   * @brief Function to get the bundle in raw format.
   *
   * This function will generate a raw version of the current bundle.
   *
   * @return the bundle in raw format.
   */
  std::string getRaw();
  /**
   * @brief Function to get the PrimaryBlock.
   *
   * This function returns a pointer to the primary block of the bundle.
   *
   * @return a pointer to the primary block.
   */
  PrimaryBlock* getPrimaryBlock();
  /**
   * @brief Function to get the other blocks of the bundle.
   *
   * This function returns all the blocks that the bundle holds.
   *
   * @return a vector with all the blocks.
   */
  std::vector<Block *> getBlocks();
  /**
   * @brief Function to add a block to the bundle.
   *
   * This functions takes the block passed by parameter, and adds it to the
   * blocks list.
   *
   * @param A pointer to the block.
   */
  void addBlock(Block *newBlock);

 private:
  /**
   * Byte array containing the raw bundle.
   */
  std::string m_raw;
  /**
   * Pointer to the primary block of the bundle.
   */
  PrimaryBlock* m_primaryBlock;
  /**
   * Vector containing the pointers to all the blocks that the bundle holds.
   */
  std::vector<Block *> m_blocks;
};

#endif  // BUNDLEAGENT_BUNDLE_BUNDLE_H_
