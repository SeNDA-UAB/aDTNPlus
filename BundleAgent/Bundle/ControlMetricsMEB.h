/*
 * Copyright (c) 2017 SeNDA
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
 * FILE ControlMetricsMEB.h
 * AUTHOR MCarmen
 * DATE Dec 12, 2017
 * VERSION 1
 *
 */
#ifndef BUNDLEAGENT_BUNDLE_CONTROLMETRICSMEB_H_
#define BUNDLEAGENT_BUNDLE_CONTROLMETRICSMEB_H_

#include "MetadataExtensionBlock.h"

class ControlMetricsMEB : public MetadataExtensionBlock {
 public:
  explicit ControlMetricsMEB(const uint16_t nDrops, const uint16_t nDelivered);
  /**
   * @brief Raw constructor.
   *
   * This will generate SprayAndWait MEB from raw data.
   *
   */
  explicit ControlMetricsMEB(const std::string &rawData);

  virtual ~ControlMetricsMEB();

  /**
   * @brief Returns an string with a nice view of the block information.
   *
   * @return The string with the block information.
   */
  std::string toString();



 private:
  /**
   * Counter of the messages dropped.
   */
  uint32_t m_nrOfDrops;
  /**
   * Number of messages received and I am the final destination.
   */
  uint32_t m_nrOfDelivered;
};

#endif  // BUNDLEAGENT_BUNDLE_CONTROLMETRICSMEB_H_
