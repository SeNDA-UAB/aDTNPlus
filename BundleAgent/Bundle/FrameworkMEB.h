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
 * FILE FrameworkMEB.h
 * AUTHOR clacambra
 * DATE 29/03/2016
 * VERSION 1
 * This file contains the specification of the FrameworkMEB class.
 */

#ifndef BUNDLEAGENT_BUNDLE_FRAMEWORKMEB_H_
#define BUNDLEAGENT_BUNDLE_FRAMEWORKMEB_H_

#include <vector>
#include <memory>
#include <string>
#include <map>
#include "Bundle/MetadataExtensionBlock.h"
#include "ExternTools/json/json.hpp"

class FrameworkExtension;


class FrameworkMEB : public MetadataExtensionBlock {
 public:
  /**
   * @brief Constructor
   *
   * Creates a new FrameworkMEB.
   *
   * @param fwkId Defines id of the framework.
   * @param extensions Defines the extensions of the framework.
   * @param state Describes the bundle state.
   */
  FrameworkMEB(uint8_t fwkId, std::map<uint8_t,
               std::shared_ptr<FrameworkExtension>> extensions,
               std::string state);
  /**
   * @brief Raw constructor.
   *
   * Creates a new FrameworkMeb from raw data.
   *
   * @param rawData The raw data that contains the FrameworkMEB.
   */
  explicit FrameworkMEB(const std::string& rawData);
  /**
   * Destructor of the class.
   */
  virtual ~FrameworkMEB();
  /**
   * Converts the FrameworkMEB in raw format.
   *
   * @return This block in raw format.
   */
  std::string toRaw();
  /**
   * Function to get the framework id.
   *
   * @return The id of the framework.
   */
  uint8_t getFwkId();
  /**
   * Function to get the extensions of the framework.
   *
   * @return The framework extensions.
   */
  std::map<uint8_t, std::shared_ptr<FrameworkExtension>> getFwkExts();
  /**
   * Function to get the bundle state.
   *
   * @return The bundle state.
   */
  nlohmann::json& getBundleState();
  /**
   * Function to set a new BundleState to the block.
   *
   * @param state The new state.
   */
  void setBundleState(nlohmann::json state);
  /**
   *
   */
  void initFromRaw(const std::string& rawData);
  /**
   *
   */
  std::shared_ptr<FrameworkExtension> getFwkExt(uint8_t fwkExtId);

 private:
  /**
   * Defines the id of the framework.
   */
  uint8_t m_fwkId;
  /**
   * Defines the framework extensions.
   */
  std::map<uint8_t, std::shared_ptr<FrameworkExtension>> m_fwkExts;
  /**
   * Describes the state of the bundle.
   */
  nlohmann::json m_bundleState;
};

#endif  // BUNDLEAGENT_BUNDLE_FRAMEWORKMEB_H_
