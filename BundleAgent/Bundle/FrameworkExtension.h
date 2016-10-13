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
 * FILE FrameworkExtension.h
 * AUTHOR clacambra
 * DATE 29/03/2016
 * VERSION 1
 * This file contains the specification of the FrameworkExtension class.
 */

#ifndef BUNDLEAGENT_BUNDLE_FRAMEWORKEXTENSION_H_
#define BUNDLEAGENT_BUNDLE_FRAMEWORKEXTENSION_H_

#include <string>

class FrameworkExtension {
 public:
  /**
   * @brief Constructor.
   *
   * Creates a new FrameworkExtension class.
   */
  FrameworkExtension(uint8_t fwkExtId, std::string swSrcCode);
  /**
   * Destructor of the class.
   */
  virtual ~FrameworkExtension();
  /**
   * Function to get the framework extension id.
   *
   * @return The framework extension id.
   */
  uint8_t getFwkExtId();
  /**
   * Function to get the code length.
   *
   * @return The length of the code.
   */
  uint16_t getCodeLength();
  /**
   * Function to get the code of the extension.
   *
   * @return The code to execute instead of the default.
   */
  std::string getSwSrcCode();
  /**
   * Function that checks if two FrameworkExtensions are the same.
   *
   * @param b A FrameworExtension
   * @return A boolean value describing if the FrameworkExtension is equal
   * to the given one.
   */
  bool operator ==(const FrameworkExtension &b) const;

 private:
  /**
   * Defines the framework extension id.
   */
  uint8_t m_fwkExtId;
  /**
   * Defines the framework extension code which will be executed instead of
   * the default.
   */
  std::string m_swSrcCode;
};

#endif  // BUNDLEAGENT_BUNDLE_FRAMEWORKEXTENSION_H_
