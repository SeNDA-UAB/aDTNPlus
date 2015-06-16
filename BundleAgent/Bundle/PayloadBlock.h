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
 * FILE PayloadBlock.h
 * AUTHOR Blackcatn13
 * DATE Jun 16, 2015
 * VERSION 1
 * This file contains the PayloadBlock class.
 */
#ifndef BUNDLEAGENT_BUNDLE_PAYLOADBLOCK_H_
#define BUNDLEAGENT_BUNDLE_PAYLOADBLOCK_H_

#include "Block.h"
#include <string>
#include <cstdint>

class PayloadBlock : public Block {
 public:
  /**
   * @brief Empty constructor.
   *
   * Constructs an empty payload block.
   */
  PayloadBlock();
  /**
   * @brief Raw constructor.
   *
   * Generates a Payload block from a raw data.
   *
   * @param rawData that contains the payload block.
   */
  explicit PayloadBlock(uint8_t* rawData);
  /**
   * @brief String constructor.
   *
   * This function generates a payload block from a string.
   *
   * @param payload string that contains the paylaod information.
   */
  explicit PayloadBlock(const std::string &payload);
  /**
   * Destructor of the class
   */
  virtual ~PayloadBlock();
  /**
   * @brief Get the payload block in raw format.
   *
   * This functions converts the payload block to raw format.
   *
   * @return The block in raw format.
   */
  uint8_t getRaw();
  /**
   * Function to get the payload value.
   *
   * @return The payload value.
   */
  uint8_t* getPayload();

 private:
  /**
   * Payload of the block.
   */
  uint8_t* m_payload;
};

#endif  // BUNDLEAGENT_BUNDLE_PAYLOADBLOCK_H_
