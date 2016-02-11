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

#include <cstdint>
#include <string>

#include "Bundle/CanonicalBlock.h"

/**
 * CLASS MetadataExtensionBlock
 * This class represents a Metadata extension block for the bundle protocol
 * as described into the RFC 6258.
 */
class MetadataExtensionBlock : public CanonicalBlock {
 public:
  MetadataExtensionBlock();
  /**
   * @brief Empty constructor.
   *
   * This will generate an empty Metadata extension block.
   */
  MetadataExtensionBlock(const uint8_t metadataType,
                         const std::string metadata);
  /**
   * @brief Raw constructor.
   *
   * This will generate Metadata extension block from raw data.
   *
   * @param rawData the raw data that contains the metadata block.
   */
  explicit MetadataExtensionBlock(const std::string &rawData);
  /**
   * Destructor of the class.
   */
  virtual ~MetadataExtensionBlock();
  /**
   * @brief Convert the Metadata extension block to raw format.
   *
   * Virtual function, all the blocks that derive from this class must implement
   *it.
   * This function must return the block in raw format.
   *
   * @return This block in raw format.
   */
  virtual std::string toRaw();
  /**
   * Function to get the metadata.
   *
   * @return The content of the block.
   */
  std::string getMetadata();
  /**
   * Function to get the type of the metadata extension block.
   *
   * @return The type of the block.
   */
  uint8_t getMetadataType();
  /**
   * @brief Returns an string with a nice view of the block information.
   *
   * @return The string with the block information.
   */
  std::string toString();
  /**
   * @brief Parses a Metadata Extension Block from raw.
   *
   * This function parses a metadata extension block from raw.
   * Saving the raw data into the block.
   *
   * @parm rawData Raw data to parse.
   */
  void initFromRawMEB(const std::string &rawData);

 protected:
  /**
   * Type of the metadata block.
   */
  uint8_t m_metadataType;
  /**
   * Metadata contained in the block.
   */
  std::string m_metadata;
};

#endif  // BUNDLEAGENT_BUNDLE_METADATAEXTENSIONBLOCK_H_
