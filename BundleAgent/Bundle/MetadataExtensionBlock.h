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
 * FILE MetadataExtensionBlock.h
 * AUTHOR Blackcatn13
 * DATE Jun 15, 2015
 * VERSION 1
 * This file contains the MetadataExtensionBlock class.
 */
#ifndef BUNDLEAGENT_BUNDLE_METADATAEXTENSIONBLOCK_H_
#define BUNDLEAGENT_BUNDLE_METADATAEXTENSIONBLOCK_H_

#include "Block.h"
#include <cstdint>

/**
 * CLASS MetadataExtensionBlock
 * This class represents a Metadata extension block for the bundle protocol
 * as described into the RFC 6258.
 */
class MetadataExtensionBlock : public Block {
 public:
  /**
   * @brief Empty constructor.
   *
   * This will generate an empty Metadata extension block.
   */
  MetadataExtensionBlock();
  /**
   * @brief Raw constructor.
   *
   * This will generate Metadata extension block from raw data.
   *
   * @param rawData the raw data that contains the metadata block.
   */
  explicit MetadataExtensionBlock(uint8_t rawData);
  /**
   * Destructor of the class.
   */
  virtual ~MetadataExtensionBlock();
  /**
   * @brief Convert the Metadata extension block to raw format.
   *
   * Virtual function, all the blocks that derive from this class must implement it.
   * This function must return the block in raw format.
   *
   * @return This block in raw format.
   */
  virtual uint8_t* getRaw() = 0;
  /**
   * Function to get the type of the metadata extension block.
   *
   * @return The type of the block.
   */
  uint8_t getMetadataType();
  /**
   * @brief Get the content of the Metadata extnsion block.
   *
   * Virtual function, all the blocks that derive form this class must implement it.
   * This function must return the content of the Metadata extension block.
   *
   * @return The content of the block.
   */
  virtual uint8_t* getMetadataContent() = 0;

 private:
  /**
   * Type of the metadata block.
   */
  uint8_t m_metadataType;
  /**
   * Metadata contained in the block.
   */
  uint8_t*  m_metadata;
};

#endif  // BUNDLEAGENT_BUNDLE_METADATAEXTENSIONBLOCK_H_
