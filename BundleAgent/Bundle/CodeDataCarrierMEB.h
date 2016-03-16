/*
 * Copyright (c) 2016 SeNDA
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
 * FILE NewMEB.h
 * AUTHOR clacambra
 * DATE 11/03/2016
 * VERSION 1
 * This file contains the implementation of CodeDataCarrierMEB class.
 */

#ifndef BUNDLEAGENT_BUNDLE_CODEDATACARRIERMEB_H_
#define BUNDLEAGENT_BUNDLE_CODEDATACARRIERMEB_H_

#include <string>

#include "MetadataExtensionBlock.h"

class CodeDataCarrierMEB : public MetadataExtensionBlock {
 public:
  /**
   * @brief Constructor.
   *
   * This will generate a new NewMEB.
   *
   * @param code String that contains the code to forward the bundle.
   * @param data
   */
  CodeDataCarrierMEB(std::string code, std::string data);
  /**
   * @brief Raw constructor.
   *
   * This will generate NewMEB from raw data.
   *
   * @param rawData The raw data that contains the NewMEB.
   */
  explicit CodeDataCarrierMEB(const std::string& rawData);
  /**
   * Destructor of the class.
   */
  virtual ~CodeDataCarrierMEB();
  /**
   * @brief Parses a NewMEB from raw.
   *
   * This function parses a NewMeb from raw saving the raw data into the block.
   *
   * @param rawData Raw data to parse.
   */
  void initFromRaw(const std::string& rawData);
  /**
   * Function to get the length of the code.
   *
   * @return The length of the code.
   */
  uint16_t getCodeLength();
  /**
   * Function to get the forward code.
   *
   * @return The code contained in the block.
   */
  std::string getCode();
  /**
   * Function to get the data.
   *
   * @return The data contained in the block.
   */
  std::string getData();


 private:
  /**
   * Length of the code contained in this MEB.
   */
  uint16_t m_codeLength;
  /**
   * Code which can be executed to forward the bundle.
   */
  std::string m_code;
  /**
   *
   */
  std::string m_data;
};

#endif  // BUNDLEAGENT_BUNDLE_CODEDATACARRIERMEB_H_
