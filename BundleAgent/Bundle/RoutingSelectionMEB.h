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
 * FILE RoutingSelectionMEB.h
 * AUTHOR clacambra
 * DATE 03/02/2016
 * VERSION 1
 * This file contains RoutingSelectionMEB class
 */

#ifndef BUNDLEAGENT_BUNDLE_ROUTINGSELECTIONMEB_H_
#define BUNDLEAGENT_BUNDLE_ROUTINGSELECTIONMEB_H_

#include <string>
#include <cstdint>

#include "MetadataExtensionBlock.h"

class RoutingSelectionMEB : public MetadataExtensionBlock {
 public:
  /**
   * @brief Constructor.
   *
   * This will generate an empty Routing Selection MEB.
   */
  explicit RoutingSelectionMEB(const uint8_t selection);
  /**
   * @brief Raw constructor.
   *
   * This will generate Routing selection MEB from raw data.
   *
   */
  explicit RoutingSelectionMEB(const std::string &rawData);
  /**
   * Destructor of the class.
   */
  virtual ~RoutingSelectionMEB();
  /**
   * Function to get the routing algorithm.
   *
   * @return The routing algorithm.
   */
  uint8_t getSelection();

 private:
  /**
   * Type of routing algorithm.
   */
  uint8_t m_selection;
};

#endif  // BUNDLEAGENT_BUNDLE_ROUTINGSELECTIONMEB_H_
